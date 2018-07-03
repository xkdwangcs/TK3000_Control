#include "AxisLinkage.h"
#include "Parameter.h"
#include "AxisControl.h"
#include "math.h"
#include "WorkControl.h"
#include "LineRunin.h"

//�����£��жϲ�����������ʱִ��ָ��������ж���
void RunningAction(AxisIDEnum axisID,s32 oldPlusNum,AxisRunningActionStruct* actions,u8 actionCount)
{
	if(actions==NULL || actionCount<=0 || !GetAxisRunState(axisID))//�����Ӧ�����ֹͣ����ִ������
		return;	
	//MotorIDEnum motorID=AxisList[axisID].MotorID;	
	for(u8 i=0;i<actionCount;i++)
	{
		if(actions[i].ActionFunc==NULL)
			continue;
		if(actions[i].ActionType==MultiAction)//�������
		{			
			if(actions[i].MultiActionPara.StartPulse>0 )
			{
				//��С����Χ�ƶ�
				if(actions[i].MultiActionPara.StartPulse < actions[i].MultiActionPara.EndPulse && 
					GetAxisRealPulse(axisID) < actions[i].MultiActionPara.StartPulse)
						continue;
				//�Ӵ�С��Χ�ƶ�
				if(actions[i].MultiActionPara.StartPulse > actions[i].MultiActionPara.EndPulse && 
					GetAxisRealPulse(axisID) > actions[i].MultiActionPara.StartPulse)
						continue;
			}
			if(actions[i].MultiActionPara.EndPulse>0)
			{
				//��С����Χ�ƶ�
				if(actions[i].MultiActionPara.StartPulse < actions[i].MultiActionPara.EndPulse && 
					GetAxisRealPulse(axisID) > actions[i].MultiActionPara.EndPulse)
						continue;
				//�Ӵ�С��Χ�ƶ�
				if(actions[i].MultiActionPara.StartPulse > actions[i].MultiActionPara.EndPulse && 
					GetAxisRealPulse(axisID) < actions[i].MultiActionPara.EndPulse)
						continue;
			}
			actions[i].ActionFunc();
		}
		else if(actions[i].ActionType==SingleAction)//��������
		{
			if(!actions[i].SingleActionPara.isCallAction && GetAxisRealPulse(axisID)>=actions[i].SingleActionPara.ActionPulse)
			{
				actions[i].SingleActionPara.isCallAction=true;//ÿ��������һ�ι�����ֻ��һ��
				actions[i].ActionFunc();
			}
		}
	}
}

//�жϲ�����������ʱִ�ж�������ж���������
void RunningActionMultiAxis(LinkageMotorStruct* linkageMotors,u8 count)
{
	for(u8 actionMotorIndex=0;actionMotorIndex<count;actionMotorIndex++)
	{
		LinkageMotorStruct linkMotor=linkageMotors[actionMotorIndex];
		RunningAction(linkMotor.AxisID,linkMotor.OldPlusNum,linkMotor.RunningActions,linkMotor.ActionCount);
	}
}


//��������ת������һ������������ͬ���ġ�
//MotorRunningActions:Ҫ����Ķ����б�Ҫ�󰴶��������С����(��������ʱ)��Ӵ�С(�븺������)����
//actionCount:��������
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

//���������ṹ�����е�һ����Ĳ���
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
		calcRunOtherCoord=vertAxisLink->SafeHeight;//����Ǵ�ֱ�����ð�ȫ�߶ȼ�������ʱ�İ�ȫƽ������ֵ
		oneLinkage->VertAxisLink.SafeHeight=vertAxisLink->SafeHeight;
		oneLinkage->VertAxisLink.UpActions=vertAxisLink->UpActions;
		oneLinkage->VertAxisLink.UpActionCount=vertAxisLink->UpActionCount;
		oneLinkage->VertAxisLink.DownActions=vertAxisLink->DownActions;
		oneLinkage->VertAxisLink.DownActionCount=vertAxisLink->DownActionCount;
	}
	float moveK= Parameter.LinkageSafeSmooth;//��ȫƽ��ϵ��,���Էŵ���������У�ÿ�ᶼ�������ò�һ��
	s32 moveLengPlus=abs(((s32)(calcRunOtherCoord/Parameter.PulseK[AxisList[axisID].MotorID])-oneLinkage->OldPlusNum));
	oneLinkage->PlusNum_OtherRun=(u32)(moveLengPlus - moveLengPlus*moveK/100);
}

bool _isStopLinkRun=false;//�Ƿ�ֹͣ�����ı�־
//����ͬʱ�����Ӧ�ò�ʹ��
void MultiAxisSameTimeGoto(LinkageMotorStruct* linkageMotors,u8 count,LinkageMotorStruct *vertAxis,bool isSynch)
{
	AxisIDEnum stAxiss[count];
	float stCoords[count];
	u8 stCount=0;
	for(u8 i=0;i<count;i++)
	{	
		if(vertAxis!=NULL && linkageMotors[i].AxisID==vertAxis->AxisID) //��ֱ�᲻����ͬʱ����
			continue;
		stAxiss[stCount]=linkageMotors[i].AxisID;
		stCoords[stCount]=linkageMotors[i].DestCoord;
		stCount++;
	}
	//LineRunin_Multi(stAxiss,stCoords,stCount,isSynch);	//�岹ʱ��ʱ���п��ٵ�����
	MoveToCoord_Multi(stAxiss,stCoords,stCount,isSynch);
}

//�����������˺���������һ�������Ĺ켣���ڰ�ȫ�߶������ƶ������ᣩUUU
//����ֻ֧��Z�����˶����붯����������Ҫ�ӷ����־�����չ��ÿ�ᶼ�ܲ��붯��
void MultiAxisLinkage(LinkageMotorStruct* linkageMotors,u8 count)
{
	_isStopLinkRun=false;
	LinkageMotorStruct *vertAxis=NULL;
	AxisIDEnum vertAxisID;
	for(u8 i=0;i<count;i++)
	{
		LinkageMotorStruct* linkMotor=&linkageMotors[i];
		if(AxisList[linkMotor->AxisID].AxisType==VerticalAxis && count>1) //����������������ϣ����д�ֱ��
		{
			vertAxis=linkMotor;
			vertAxisID=linkMotor->AxisID;
		}
	}
	if(vertAxis==NULL)//���������û�д�ֱ�����ֻ��һ����
	{
//		for(u8 i=0;i<count;i++)
//		{
//			MoveToCoord(linkageMotors[i].AxisID,linkageMotors[i].DestCoord,false);
//		}
		MultiAxisSameTimeGoto(linkageMotors,count,vertAxis,false);//����ͬʱ����
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
	else//����������д�ֱ��
	{
		bool isOtherRun=false; //�������Ƿ�����
		bool isVertDowned=false;//��ֱ���Ƿ��Ѿ���������

		MoveToCoord(vertAxisID,vertAxis->VertAxisLink.SafeHeight,false); //��һ���� �ƶ�����ȫ�߶ȣ���ֱ��������
		while(GetAxisRunState(vertAxisID) || !isOtherRun)
		{
			if(_isStopLinkRun)
				return;
			if(vertAxis->VertAxisLink.UpActions!=NULL)	//�����еĶ���
			{
				for(u8 i=0;i<vertAxis->VertAxisLink.UpActionCount;i++)
					RunningAction(vertAxisID,vertAxis->OldPlusNum,vertAxis->VertAxisLink.UpActions,vertAxis->VertAxisLink.UpActionCount);
			}
			s32 currPlusCount=abs(GetAxisRealPulse(vertAxisID)- vertAxis->OldPlusNum);//�Ѿ����˶��ٸ�����
			if(!isOtherRun && currPlusCount>= vertAxis->PlusNum_OtherRun)
			{
				isOtherRun=true;
//				for(u8 i=0;i<count;i++)//�ڶ�������ֱ���������������
//				{
//					if(linkageMotors[i].AxisID!=vertAxis->AxisID)
//						MoveToCoord(linkageMotors[i].AxisID,linkageMotors[i].DestCoord,false);
//				}
				MultiAxisSameTimeGoto(linkageMotors,count,vertAxis,false); //����ͬʱ����
				bool isSetVertDown=true;//��ֱ���Ƿ��������
				while(!isVertDowned)
				{
					if(_isStopLinkRun)
						return;
					RunningActionMultiAxis(linkageMotors,count);//�������˶�ʱ�Ķ���
					for(u8 i=0;i<count;i++)//��ⴹֱ�������������Ƿ��˰�ȫƽ����
					{
						if(linkageMotors[i].AxisID==vertAxis->AxisID)//��ֱ�᲻���
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
					if(!isVertDowned && isSetVertDown && !GetAxisRunState(vertAxisID))//Ҫ�ȴ�ֱ����������
					{
						isVertDowned=true;
						vertAxis->OldPlusNum=GetAxisRealPulse(vertAxisID);
						MoveToCoord(vertAxisID,vertAxis->DestCoord,false);//����������ֱ���ƶ���Ŀ���
						while(GetAxisRunState(vertAxisID))
						{
							if(_isStopLinkRun)
								return;
							if(vertAxis->VertAxisLink.DownActions!=NULL)	//�½��еĶ���
							{
								for(u8 i=0;i<vertAxis->VertAxisLink.DownActionCount;i++)
									RunningAction(vertAxisID,vertAxis->OldPlusNum,vertAxis->VertAxisLink.DownActions,vertAxis->VertAxisLink.DownActionCount);
							}
							RunningActionMultiAxis(linkageMotors,count);//�����������������飬���������ỹû����								
						}
						for(u8 i=0;i<count;i++)//�ȴ����е�������ȫ��ֹͣ
						{
							while(GetAxisRunState(linkageMotors[i].AxisID));				
						}						
					}
				}
			}
		}
	}
}

//ֹͣ��ǰ������ָֹͣ�������е�������˳�
void StopAxisLinkageRun(LinkageMotorStruct* linkageMotors,u8 count)
{
	ShowDebugMsg("ֹͣ����");
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

//������������������
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

//Z���ƶ���ָ���������,ͬ����
void ZMoveToCoord(float zCoord)
{
	MoveToCoord(Z,zCoord,true);
}

//X��Z�������������˶��켣�������˹����߶ȣ���
void XZLinkageToCoord(float xCoord, float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount)
{
	VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zActions,zActionCount,NULL,0};
	LinkageMotorStruct xzAxis[2];
	CreateLinkageOneAxis(&xzAxis[0],X,xCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xzAxis[1],Z,zCoord,NULL,0,&zAxisLink);
	MultiAxisLinkage(xzAxis,2);
}

//X��Y��������(��ƽ̨)��
void XYLinkageToCoord(float xCoord, float yCoord)
{
	LinkageMotorStruct xyAxis[2];
	CreateLinkageOneAxis(&xyAxis[0],X,xCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyAxis[1],Y1,yCoord,NULL,0,NULL);
	MultiAxisLinkage(xyAxis,2);
}

//X��Y2��������(��ƽ̨)
void XY2LinkageToCoord(float xCoord, float yCoord)
{
	LinkageMotorStruct xyAxis[2];
	CreateLinkageOneAxis(&xyAxis[0],X,xCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyAxis[1],Y2,yCoord,NULL,0,NULL);
	MultiAxisLinkage(xyAxis,2);
}

//X��Y��Z��������(��ƽ̨)�����˶��켣�������˹����߶ȣ���
void XYZLinkageToCoord(float xCoord, float yCoord,float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount)
{
	VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zActions,zActionCount,NULL,0};
	LinkageMotorStruct xyzAxis[3];
	CreateLinkageOneAxis(&xyzAxis[0],X,xCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyzAxis[1],Y1,yCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyzAxis[2],Z,zCoord,NULL,0,&zAxisLink);
	MultiAxisLinkage(xyzAxis,3);
}

//X��Y2��Z��������(��ƽ̨)�����˶��켣�������˹����߶ȣ���
void XY2ZLinkageToCoord(float xCoord,float y2Coord,float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount)
{
	VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zActions,zActionCount,NULL,0};
	LinkageMotorStruct xyzAxis[3];
	CreateLinkageOneAxis(&xyzAxis[0],X,xCoord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyzAxis[1],Y2,y2Coord,NULL,0,NULL);
	CreateLinkageOneAxis(&xyzAxis[2],Z,zCoord,NULL,0,&zAxisLink);
	MultiAxisLinkage(xyzAxis,3);
}
