#include "WorkControl.h"
#include "WorkControlBlow.h"
#include "AppTasks.h"
#include "AxisLinkage.h"
#include "main.h"

//测试坐标相关的动作
void TestCoordMoveBlow(void)
{
	u16 leftCoordCount=0;
	CoordStruct* leftCoordList = GetCoordList(LeftPlatform, &leftCoordCount);	
	u16 rightCoordCount=0;
	CoordStruct* rightCoordList = GetCoordList(RightPlatform, &rightCoordCount);	
	while(true)
	{
		ZMoveToCoord(Parameter.WorkHeight);		
		for (int i = 0; i < leftCoordCount;i++ )//左平台
		{				
			XYZLinkageToCoord(leftCoordList[i].X, leftCoordList[i].Y, leftCoordList[i].Z,NULL,0);//目标点
		}	
		XYZLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.GetScrewPara.Coord.Y, Parameter.WorkHeight,NULL,0); //放件位置：X轴在取料点,Y轴在零位,Z轴在工作高度

		if(Parameter.ProdType==SinglePlatform)
		  continue;
		for (int i = 0; i < rightCoordCount;i++ )//右平台
		{					
			XY2ZLinkageToCoord(rightCoordList[i].X, rightCoordList[i].Y, rightCoordList[i].Z,NULL,0);//目标点
		}			
		XY2ZLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.GetScrewPara.Coord.Y, Parameter.WorkHeight,NULL,0); //放件位置：X轴在取料点,Y轴在零位,Z轴在工作高度
	}
}


//取镙丝过程函数
bool _screwIsOK=false;//吹料是否成功
u32 _blowTimeStart=0;//开始吹料所在的计时器值
bool GetScrewBlow(void)
{
	if(Parameter.IsGetScrew)
	{
		_screwIsOK=false;
		SetOutPortState(BlowMaterPort,true);//打开吹料控制		
		_blowTimeStart=GetTimekeeperValue();
	}
	return true; 
}

//吹料检查
bool BlowCheck(void)
{
//	_screwIsOK=true;//模拟推料成功
//	return true;
	if(Parameter.IsGetScrew)
	{
		ShowDebugMsg("吹料检测");
		u32 blowDelayms=Parameter.BlowDelay*1000;
		while(true)
		{			
			u32 blowTimeLeng=GetTimekeeperValue()-_blowTimeStart;
			if(!GetInPortState(ScrewInhalePort))//如果有吹料成功信号
			{
				_screwIsOK=true;
				break;
			}
			if(blowTimeLeng>=blowDelayms)
			{				
				break;
			}
		}
		SetOutPortState(BlowMaterPort,false);//关闭吹料控制		
		if(!_screwIsOK)	//没有螺丝
		{
			char* msg="螺丝推送异常！";
			//ShowWorkMsg(msg);
			if(Parameter.FaultMode==WAIT)
			{
				//WriteLog(msg);
				OpenAlarm();
				return false;
			}
		}
		return true;
	}
	return true;
}

//弹簧、气缸共用函数
bool TH_QGDownCommBlow(CoordStruct destCoord,float openScrewDriverCoord,float startJSCoord)
{
	AxisRunningActionStruct zDownAction[3];
	CreateSingAction(&zDownAction[0],Z,openScrewDriverCoord,OpenScrewDriver);	//开电批事情
	CreateSingAction(&zDownAction[1],Z,startJSCoord,StartJS);	//开始计时事情
	CreateMultiAction(&zDownAction[2],Z,openScrewDriverCoord,destCoord.Z,CheckTorquePort);//开电批就开始检测扭力
	//CreateMultiAction(&zDownAction[2],Z,-1,-1,CheckTorquePort);
	VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zDownAction,3,NULL,0};
		
	CreateLinkageOneAxis(&_linkages[0],X,destCoord.X,NULL,0,NULL);
	CreateLinkageOneAxis(&_linkages[1],_currYAxis,destCoord.Y,NULL,0,NULL);
	CreateLinkageOneAxis(&_linkages[2],Z,destCoord.Z,NULL,0,&zAxisLink);
	_isTorqueOK=false;		//扭力信号标志
	ShowDebugMsg("联动到锁付坐标");
	MultiAxisLinkage(_linkages,3);
	
	if(Parameter.IsGetScrew)
	{
		if(!BlowCheck())
			return false;
	}
	SetOutPortState(BlowMaterPort,false);//关闭吹料控制
	return true;
}

//锁镙丝过程
bool ClampScrewBlow(CoordStruct destCoord)
{		
	float _clampZSpeed = Parameter.ScrewLockSpeed;//锁螺丝速度，Z轴下压模式时用
	float openScrewDriverCoord = destCoord.Z - Parameter.DPTQLength- Parameter.ScrewPara.length;//开电批的位置
	float startJSCoord = destCoord.Z - Parameter.ScrewPara.length;//开始计时的位置,气缸下压模式时用
	if(Parameter.LockScrewType==THDown)//弹簧下压锁螺丝模式
	{
		bool bl=TH_QGDownCommBlow(destCoord,openScrewDriverCoord,startJSCoord);
		if(!bl)	return false;
	}
	else if(Parameter.LockScrewType==QGDown)//气缸下压锁螺丝模式
	{
		bool bl = TH_QGDownCommBlow(destCoord,openScrewDriverCoord,startJSCoord);
		if(!bl)	return false;
		if(Parameter.IsGetScrew)
		{
			ShowDebugMsg("打开锁付气缸");
			SetOutPortState(LockQGPort,true);//打开Z轴气缸		
			float nlDelay=Parameter.LockQGDelay*1000;//Z轴气缸延时
			u32 delayCount=(u32)(nlDelay/10);			
			for(u32 i=0;i<delayCount;i++)
			{
				Delay_ms_OS(10);
				if(!GetInPortState(TorquePort))
				{		
					ShowDebugMsg("检测到扭力!关电批、锁付气缸");					
					SetOutPortState(ScrewDriverPort,false);//关电批
					SetOutPortState(LockQGPort,false); //关闭Z轴气缸
					//Delay_ms_OS(800);  //为什么会加这个？会导致卡顿！！浙江客户定制时用
					_isTorqueOK=true;
					CalcLockTime();
					break;
				}
			}
		}		
	}
	else //Z轴电机下压锁螺丝模式
	{
		AxisRunningActionStruct zDownAction[1];
		CreateSingAction(&zDownAction[0],Z,openScrewDriverCoord,OpenScrewDriver);	//开电批事情
		VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zDownAction,1,NULL,0};
			
		CreateLinkageOneAxis(&_linkages[0],X,destCoord.X,NULL,0,NULL);
		CreateLinkageOneAxis(&_linkages[1],_currYAxis,destCoord.Y,NULL,0,NULL);
		CreateLinkageOneAxis(&_linkages[2],Z,destCoord.Z- Parameter.ScrewPara.length,NULL,0,&zAxisLink);			
		MultiAxisLinkage(_linkages,3);
		if(!BlowCheck())
			return false;
		SetOutPortState(ScrewDriverPort,true);//开电批。防止电批提前为0时，电批没有打开
		if(Parameter.IsGetScrew)
		{
			_isTorqueOK=false;
			StartJS();//开始计时
			AxisRunningActionStruct actions1[1];
			CreateMultiAction(&actions1[0],Z,-1,-1,CheckTorquePort);		
			MoveToCoordRunActionPara(Z,destCoord.Z,actions1,1,10*_clampZSpeed,_clampZSpeed);		
		}
		else
		{
			MoveToCoordRunActionPara(Z,destCoord.Z,NULL,0,10*_clampZSpeed,_clampZSpeed);
		}
	}
	
	bool bl=GZJC_FGFY();//故障判断
	SetOutPortState(ScrewDriverPort,false);//关电批
	if(Parameter.LockScrewType==QGDown)//气缸下压锁螺丝模式
		SetOutPortState(LockQGPort,false); //关闭Z轴气缸
	return bl;
}

