#include "AxisLinkage.h"
#include "Parameter.h"
#include "AxisControl.h"
#include "math.h"
#include "WorkControl.h"
#include "LineRunin.h"

//单轴下，判断并在满足条件时执行指定轴的所有动作
void RunningAction(AxisIDEnum axisID,s32 oldPlusNum,AxisRunningActionStruct* actions,u8 actionCount)
{
	if(actions==NULL || actionCount<=0 || !GetAxisRunState(axisID))//如果相应电机已停止，则不执行事情
		return;	
	//MotorIDEnum motorID=AxisList[axisID].MotorID;	
	for(u8 i=0;i<actionCount;i++)
	{
		if(actions[i].ActionFunc==NULL)
			continue;
		if(actions[i].ActionType==MultiAction)//多次事情
		{			
			if(actions[i].MultiActionPara.StartPulse>0 )
			{
				//从小到大范围移动
				if(actions[i].MultiActionPara.StartPulse < actions[i].MultiActionPara.EndPulse && 
					GetAxisRealPulse(axisID) < actions[i].MultiActionPara.StartPulse)
						continue;
				//从大到小范围移动
				if(actions[i].MultiActionPara.StartPulse > actions[i].MultiActionPara.EndPulse && 
					GetAxisRealPulse(axisID) > actions[i].MultiActionPara.StartPulse)
						continue;
			}
			if(actions[i].MultiActionPara.EndPulse>0)
			{
				//从小到大范围移动
				if(actions[i].MultiActionPara.StartPulse < actions[i].MultiActionPara.EndPulse && 
					GetAxisRealPulse(axisID) > actions[i].MultiActionPara.EndPulse)
						continue;
				//从大到小范围移动
				if(actions[i].MultiActionPara.StartPulse > actions[i].MultiActionPara.EndPulse && 
					GetAxisRealPulse(axisID) < actions[i].MultiActionPara.EndPulse)
						continue;
			}
			actions[i].ActionFunc();
		}
		else if(actions[i].ActionType==SingleAction)//单次事情
		{
			if(!actions[i].SingleActionPara.isCallAction && GetAxisRealPulse(axisID)>=actions[i].SingleActionPara.ActionPulse)
			{
				actions[i].SingleActionPara.isCallAction=true;//每个动作在一次过程中只做一次
				actions[i].ActionFunc();
			}
		}
	}
}

//判断并在满足条件时执行多轴的所有动作，多轴
void RunningActionMultiAxis(LinkageMotorStruct* linkageMotors,u8 count)
{
	for(u8 actionMotorIndex=0;actionMotorIndex<count;actionMotorIndex++)
	{
		LinkageMotorStruct linkMotor=linkageMotors[actionMotorIndex];
		RunningAction(linkMotor.AxisID,linkMotor.OldPlusNum,linkMotor.RunningActions,linkMotor.ActionCount);
	}
}


//单轴电机运转并插入一个或多个动作，同步的。
//MotorRunningActions:要插入的动作列表，要求按动作坐标从小到大(往正方向时)或从大到小(入负方向走)排列
//actionCount:动作个数
void MoveToCoordRunAction(AxisIDEnum axisID,float destCoord,AxisRunningActionStruct* motorRunningActions,u8 actionCount)
{
	MotorIDEnum motorID=AxisList[axisID].MotorID;
	if(!GetAxisRunState(axisID) && destCoord>=0)
	{
		s32 oldPlusNum=GetAxisRealPulse(axisID);		
		s32 stepsTemp = (s32)(destCoord /Parameter.PulseK[motorID]);
		s32 plusNum=stepsTemp-oldPlusNum;
		
		MotorIDEnum motorList[1]={motorID};
		s32 pulseList[1]={plusNum};	
		MotorRun(motorList,pulseList,1,false);	
		while(GetAxisRunState(axisID))
		{		
			RunningAction(axisID,oldPlusNum,motorRunningActions,actionCount);
		}	
	}
}

void MoveToCoordRunActionPara(AxisIDEnum axisID,float destCoord,AxisRunningActionStruct* motorRunningActions,u8 actionCount,float acc,float speed)
{
	MotorIDEnum motorID=AxisList[axisID].MotorID;
	if(!GetAxisRunState(axisID) && destCoord>=0)
	{
		s32 oldPlusNum = GetAxisRealPulse(axisID);		
		s32 stepsTemp = (s32)(destCoord /Parameter.PulseK[motorID]);	
		MotorRun_OutSpeed(motorID,stepsTemp-oldPlusNum,speed,acc);
		while(GetAxisRunState(axisID))
		{		
			RunningAction(axisID,oldPlusNum,motorRunningActions,actionCount);
		}	
	}
}

//创建联动结构数组中的一根轴的参数
void CreateLinkageOneAxis(LinkageMotorStruct* oneLinkage,AxisIDEnum axisID,float destCoord,
	AxisRunningActionStruct* actionList,u8 actionCount,VerticalAxisLinkStruct* vertAxisLink)
{
	oneLinkage->AxisID=axisID;
	oneLinkage->DestCoord=destCoord;
	oneLinkage->RunningActions=actionList;
	oneLinkage->ActionCount=actionCount;
	oneLinkage->OldPlusNum=GetAxisRealPulse(axisID);
	float calcRunOtherCoord=destCoord;
	if(AxisList[axisID].AxisType==VerticalAxis)
	{
		calcRunOtherCoord=vertAxisLink->SafeHeight;//如果是垂直轴则用安全高度计算上升时的安全平滑脉冲值
		oneLinkage->VertAxisLink.SafeHeight=vertAxisLink->SafeHeight;
		oneLinkage->VertAxisLink.UpActions=vertAxisLink->UpActions;
		oneLinkage->VertAxisLink.UpActionCount=vertAxisLink->UpActionCount;
		oneLinkage->VertAxisLink.DownActions=vertAxisLink->DownActions;
		oneLinkage->VertAxisLink.DownActionCount=vertAxisLink->DownActionCount;
	}
	float moveK= Parameter.LinkageSafeSmooth;//安全平滑系数,可以放到电机参数中，每轴都可以设置不一样
	s32 moveLengPlus=abs(((s32)(calcRunOtherCoord/Parameter.PulseK[AxisList[axisID].MotorID])-oneLinkage->OldPlusNum));
	oneLinkage->PlusNum_OtherRun=(u32)(moveLengPlus - moveLengPlus*moveK/100);
}

bool _isStopLinkRun=false;//是否停止联动的标志
//多轴同时到达的应用层使用
void MultiAxisSameTimeGoto(LinkageMotorStruct* linkageMotors,u8 count,LinkageMotorStruct *vertAxis,bool isSynch)
{
	AxisIDEnum stAxiss[count];
	float stCoords[count];
	u8 stCount=0;
	for(u8 i=0;i<count;i++)
	{	
		if(vertAxis!=NULL && linkageMotors[i].AxisID==vertAxis->AxisID) //垂直轴不参于同时到达
			continue;
		stAxiss[stCount]=linkageMotors[i].AxisID;
		stCoords[stCount]=linkageMotors[i].DestCoord;
		stCount++;
	}
	//LineRunin_Multi(stAxiss,stCoords,stCount,isSynch);	//插补时有时会有卡顿的现象
	MoveToCoord_Multi(stAxiss,stCoords,stCount,isSynch);
}

//多轴联动，此函数考虑了一个完整的轨迹（在安全高度左右移动其它轴）UUU
//现在只支持Z向下运动插入动作，如有需要加方向标志后可扩展到每轴都能插入动作
void MultiAxisLinkage(LinkageMotorStruct* linkageMotors,u8 count)
{
	_isStopLinkRun=false;
	LinkageMotorStruct *vertAxis=NULL;
	AxisIDEnum vertAxisID;
	for(u8 i=0;i<count;i++)
	{
		LinkageMotorStruct* linkMotor=&linkageMotors[i];
		if(AxisList[linkMotor->AxisID].AxisType==VerticalAxis && count>1) //如果轴数有两个以上，且有垂直轴
		{
			vertAxis=linkMotor;
			vertAxisID=linkMotor->AxisID;
		}
	}
	if(vertAxis==NULL)//如果联动中没有垂直轴或者只有一根轴
	{
//		for(u8 i=0;i<count;i++)
//		{
//			MoveToCoord(linkageMotors[i].AxisID,linkageMotors[i].DestCoord,false);
//		}
		MultiAxisSameTimeGoto(linkageMotors,count,vertAxis,false);//多轴同时到达
		for(u8 i=0;i<count;i++)
		{
			while(GetAxisRunState(linkageMotors[i].AxisID))
			{	
				if(_isStopLinkRun)
					return;
				RunningActionMultiAxis(linkageMotors,count);
			}
		}
	}
	else//如果多轴中有垂直轴
	{
		bool isOtherRun=false; //其它轴是否运行
		bool isVertDowned=false;//垂直轴是否已经向下运行

		MoveToCoord(vertAxisID,vertAxis->VertAxisLink.SafeHeight,false); //第一步： 移动到安全高度，垂直轴向上移
		while(GetAxisRunState(vertAxisID) || !isOtherRun)
		{
			if(_isStopLinkRun)
				return;
			if(vertAxis->VertAxisLink.UpActions!=NULL)	//上升中的动作
			{
				for(u8 i=0;i<vertAxis->VertAxisLink.UpActionCount;i++)
					RunningAction(vertAxisID,vertAxis->OldPlusNum,vertAxis->VertAxisLink.UpActions,vertAxis->VertAxisLink.UpActionCount);
			}
			s32 currPlusCount=abs(GetAxisRealPulse(vertAxisID)- vertAxis->OldPlusNum);//已经走了多少个脉冲
			if(!isOtherRun && currPlusCount>= vertAxis->PlusNum_OtherRun)
			{
				isOtherRun=true;
//				for(u8 i=0;i<count;i++)//第二步：垂直轴外的所有轴运行
//				{
//					if(linkageMotors[i].AxisID!=vertAxis->AxisID)
//						MoveToCoord(linkageMotors[i].AxisID,linkageMotors[i].DestCoord,false);
//				}
				MultiAxisSameTimeGoto(linkageMotors,count,vertAxis,false); //多轴同时到达
				bool isSetVertDown=true;//垂直轴是否可以下移
				while(!isVertDowned)
				{
					if(_isStopLinkRun)
						return;
					RunningActionMultiAxis(linkageMotors,count);//其它轴运动时的动作
					for(u8 i=0;i<count;i++)//检测垂直轴以外所有轴是否到了安全平滑点
					{
						if(linkageMotors[i].AxisID==vertAxis->AxisID)//垂直轴不检测
							continue;
						s32 currPlusCount=abs(GetAxisRealPulse(linkageMotors[i].AxisID)- linkageMotors[i].OldPlusNum);
						if(currPlusCount >= linkageMotors[i].PlusNum_OtherRun || !GetAxisRunState(linkageMotors[i].AxisID))
						{
							isSetVertDown=true;
						}
						else
						{
							isSetVertDown=false;
							break;
						}
							
					}
					if(!isVertDowned && isSetVertDown && !GetAxisRunState(vertAxisID))//要等垂直轴往上走完
					{
						isVertDowned=true;
						vertAxis->OldPlusNum=GetAxisRealPulse(vertAxisID);
						MoveToCoord(vertAxisID,vertAxis->DestCoord,false);//第三步：垂直轴移动到目标点
						while(GetAxisRunState(vertAxisID))
						{
							if(_isStopLinkRun)
								return;
							if(vertAxis->VertAxisLink.DownActions!=NULL)	//下降中的动作
							{
								for(u8 i=0;i<vertAxis->VertAxisLink.DownActionCount;i++)
									RunningAction(vertAxisID,vertAxis->OldPlusNum,vertAxis->VertAxisLink.DownActions,vertAxis->VertAxisLink.DownActionCount);
							}
							RunningActionMultiAxis(linkageMotors,count);//继续检测其它轴的事情，担心其它轴还没走完								
						}
						for(u8 i=0;i<count;i++)//等待所有的联动轴全部停止
						{
							while(GetAxisRunState(linkageMotors[i].AxisID));				
						}						
					}
				}
			}
		}
	}
}

//停止当前联动。停止指定的所有电机，并退出
void StopAxisLinkageRun(LinkageMotorStruct* linkageMotors,u8 count)
{
	ShowDebugMsg("停止联动");
	for(u8 i=0;i<count;i++)
	{
		StopAxis(linkageMotors[i].AxisID);
	}
	for(u8 i=0;i<count;i++)
	{
		while(GetAxisRunState(linkageMotors[i].AxisID));
	}	
	_isStopLinkRun=true;
}

//多轴联动，不带动作
void MultiAxisLinkageNoAction(AxisIDEnum* axisIDs,float* coords, u8 count)
{
	if(count==0)
		return;
	if(count==1)
	{
		MoveToCoord(axisIDs[0],coords[0],true);	
	}
	else
	{
		LinkageMotorStruct linkageAxis[count];
		for(u8 i=0;i<count;i++)
		{
			if(AxisList[axisIDs[i]].AxisType==VerticalAxis)
			{
				VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,NULL,0,NULL,0};
				CreateLinkageOneAxis(&linkageAxis[i],axisIDs[i],coords[i],NULL,0,&zAxisLink);
			}
			else
				CreateLinkageOneAxis(&linkageAxis[i],axisIDs[i],coords[i],NULL,0,NULL);
		}
		MultiAxisLinkage(linkageAxis,count);
	}
}

//Z轴移动到指定的坐标点,同步的
void ZMoveToCoord(float zCoord)
{
	MoveToCoord(Z,zCoord,true);
}

//X、Z两轴联动，带运动轨迹（处理了工作高度）。
void XZLinkageToCoord(float xCoord, float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount)
{
	VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zActions,zActionCount,NULL,0};
	LinkageMotorStruct xzAxis[2];
	CreateLinkageOneAxis(&xzAxis[0],X,xCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xzAxis[1],Z,zCoord,NULL,0,&zAxisLink);
	MultiAxisLinkage(xzAxis,2);
}

//X、Y两轴联动(左平台)，
void XYLinkageToCoord(float xCoord, float yCoord)
{
	LinkageMotorStruct xyAxis[2];
	CreateLinkageOneAxis(&xyAxis[0],X,xCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyAxis[1],Y1,yCoord,NULL,0,NULL);
	MultiAxisLinkage(xyAxis,2);
}

//X、Y2两轴联动(右平台)
void XY2LinkageToCoord(float xCoord, float yCoord)
{
	LinkageMotorStruct xyAxis[2];
	CreateLinkageOneAxis(&xyAxis[0],X,xCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyAxis[1],Y2,yCoord,NULL,0,NULL);
	MultiAxisLinkage(xyAxis,2);
}

//X、Y、Z三轴联动(左平台)，带运动轨迹（处理了工作高度）。
void XYZLinkageToCoord(float xCoord, float yCoord,float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount)
{
	VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zActions,zActionCount,NULL,0};
	LinkageMotorStruct xyzAxis[3];
	CreateLinkageOneAxis(&xyzAxis[0],X,xCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyzAxis[1],Y1,yCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyzAxis[2],Z,zCoord,NULL,0,&zAxisLink);
	MultiAxisLinkage(xyzAxis,3);
}

//X、Y2、Z三轴联动(右平台)，带运动轨迹（处理了工作高度）。
void XY2ZLinkageToCoord(float xCoord,float y2Coord,float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount)
{
	VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zActions,zActionCount,NULL,0};
	LinkageMotorStruct xyzAxis[3];
	CreateLinkageOneAxis(&xyzAxis[0],X,xCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyzAxis[1],Y2,y2Coord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyzAxis[2],Z,zCoord,NULL,0,&zAxisLink);
	MultiAxisLinkage(xyzAxis,3);
}
