#include "WorkControl.h"
#include "WorkControlSuck.h"
#include "AppTasks.h"
#include "AxisLinkage.h"
#include "MainForm.h"

//取镙丝过程函数
bool GetScrew(void);
//锁镙丝过程
bool ClampScrew(CoordStruct destCoord);

//打开真空电磁阀
void OpenVacuumValve()
{
	SetOutPortState(VacuumValvePort,true);
}

//关闭真空电磁阀
void CloseVacuumValve()
{
	SetOutPortState(VacuumValvePort,false);
}

//测试坐标相关的动作
void TestCoordMoveSuck(void)
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
			XZLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.GetScrewPara.Coord.Z,NULL,0);	//取料点
			XYZLinkageToCoord(leftCoordList[i].X, leftCoordList[i].Y, leftCoordList[i].Z,NULL,0);//目标点
		}	
		XYZLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.GetScrewPara.Coord.Y, Parameter.WorkHeight,NULL,0); //放件位置：X轴在取料点,Y轴在零位,Z轴在工作高度

		if(Parameter.ProdType==SinglePlatform)
		  continue;
		for (int i = 0; i < rightCoordCount;i++ )//右平台
		{		
			XZLinkageToCoord(Parameter.GetScrewPara.Coord.X,Parameter.GetScrewPara.Coord.Z,NULL,0);	//取料点
			XY2ZLinkageToCoord(rightCoordList[i].X, rightCoordList[i].Y, rightCoordList[i].Z,NULL,0);//目标点
		}			
		XY2ZLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.GetScrewPara.Coord.Y, Parameter.WorkHeight,NULL,0); //放件位置：X轴在取料点,Y轴在零位,Z轴在工作高度
	}
}

bool _isScrewInhaleOK=true;//正式打螺丝前有吸取是否正常
//螺丝吸取检测任务，从吸取完到打螺丝前都要执行
void ScrewInhaleCheck(void)
{
	if(Parameter.FaultMode==IGNORE || !(Parameter.IsScrewCheck && Parameter.IsGetScrew))//如果不取料或不真空检测
	{
		//_isScrewInhaleOK=true;//不必要在此赋值
		return;
	}
	if(GetInPortState(ScrewInhalePort) && _isScrewInhaleOK)	
	{
		_isScrewInhaleOK=false;
		char* msg="吸取异常,无真空信号!";
		//WriteLog(msg);
		ShowWorkMsg(msg);
		StopAxisLinkageRun(_linkages,3);
	}
}

//取镙丝过程函数
bool GetScrewSuck(void)
{
	//读镙丝准备信号
	if (GetInPortState(ScrewRedyPort) && Parameter.FaultMode==WAIT && Parameter.IsGetScrew)
	{
		char* msg= "螺丝盒准备异常!";
    //WriteLog(msg);
		ShowWorkMsg(msg);
		OpenAlarm();
		return false;
	}
	//移动电机到取料位置
	if(Parameter.IsGetScrew)
	{
		float openVacuumCoord=Parameter.GetScrewPara.Coord.Z-Parameter.ZKTQLength;//开真空的坐标
		AxisRunningActionStruct actions[1];
		CreateSingAction(&actions[0],Z,openVacuumCoord,OpenVacuumValve);
		XZLinkageToCoord(Parameter.GetScrewPara.Coord.X,Parameter.GetScrewPara.Coord.Z,actions,1);
	}
	else
	{
		XZLinkageToCoord(Parameter.GetScrewPara.Coord.X,Parameter.GetScrewPara.Coord.Z,NULL,0);
	}
	Delay_s_OS(Parameter.GetScrewPara.getDelayTime);
	return true;
}

//弹簧、气缸共用函数
bool TH_QGDownCommSuck(CoordStruct destCoord,float openScrewDriverCoord,float closeVacuumCoord,float startJSCoord)
{
	for(u8 i=0;i<3;i++)
	{		
		AxisRunningActionStruct xAction[1];
		CreateMultiAction(&xAction[0],X,-1,-1,ScrewInhaleCheck);
		
		AxisRunningActionStruct zUpAction[1];
		CreateMultiAction(&zUpAction[0],Z,-1,-1,ScrewInhaleCheck);
		AxisRunningActionStruct zDownAction[4];
		CreateSingAction(&zDownAction[0],Z,openScrewDriverCoord,OpenScrewDriver);	//开电批事情
		CreateSingAction(&zDownAction[1],Z,startJSCoord,StartJS);	//开始计时事情
		CreateSingAction(&zDownAction[2],Z,closeVacuumCoord,CloseVacuumValve);		//关真空事情		
		CreateMultiAction(&zDownAction[2],Z,openScrewDriverCoord,destCoord.Z,CheckTorquePort);//开电批就开始检测扭力
		//CreateMultiAction(&zDownAction[3],Z,-1,-1,CheckTorquePort);	
		VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zDownAction,4,zUpAction,1};
			
		CreateLinkageOneAxis(&_linkages[0],X,destCoord.X,xAction,1,NULL);
		CreateLinkageOneAxis(&_linkages[1],_currYAxis,destCoord.Y,NULL,0,NULL);
		CreateLinkageOneAxis(&_linkages[2],Z,destCoord.Z,NULL,0,&zAxisLink);
		_isScrewInhaleOK=true;	//真空信号标志
		_isTorqueOK=false;		//扭力信号标志
		MultiAxisLinkage(_linkages,3);
		if(_isScrewInhaleOK)
		{
			break;
		}
		else
		{
			SetOutPortState(VacuumValvePort,false);//关真空电磁阀
			ZMoveToCoord(Parameter.WorkHeight);
			GetScrew();
		}
	}
	if(!_isScrewInhaleOK)//如果多次吸取后还不正常，则报警
	{
		SetOutPortState(VacuumValvePort,false);//关真空电磁阀
		SetOutPortState(ScrewDriverPort,false);//关电批
		OpenAlarm();
		return false;
	}
	return true;
}

//锁镙丝过程
bool ClampScrewSuck(CoordStruct destCoord)
{	
	float _clampZSpeed = Parameter.ScrewLockSpeed;//锁螺丝速度，Z轴下压模式时用
	float openScrewDriverCoord = destCoord.Z - Parameter.DPTQLength- Parameter.ScrewPara.length;//开电批的位置
	float startJSCoord = destCoord.Z - Parameter.ScrewPara.length;//开始计时的位置,气缸下压模式时用
	float closeVacuumCoord = destCoord.Z - Parameter.ScrewPara.length /2 ;//关真空的位置
	if(Parameter.LockScrewType==THDown)//弹簧下压锁螺丝模式
	{
		TH_QGDownCommSuck(destCoord,openScrewDriverCoord,closeVacuumCoord,startJSCoord);
	}
	else if(Parameter.LockScrewType==QGDown)//气缸下压锁螺丝模式
	{
		TH_QGDownCommSuck(destCoord,openScrewDriverCoord,closeVacuumCoord,startJSCoord);
		if(Parameter.IsGetScrew)
		{
			SetOutPortState(LockQGPort,true);//打开Z轴气缸		
			float nlDelay=Parameter.LockQGDelay*1000;//Z轴气缸延时
			u32 delayCount=(u32)(nlDelay/10);			
			for(u32 i=0;i<delayCount;i++)
			{
				Delay_ms_OS(10);
				if(!GetInPortState(TorquePort))
				{					
					SetOutPortState(ScrewDriverPort,false);//关电批
					SetOutPortState(LockQGPort,false); //关闭Z轴气缸
					_isTorqueOK=true;
					CalcLockTime();
					break;
				}
			}
		}
	}
	else  if(Parameter.LockScrewType==MotorDown) //Z轴电机下压锁螺丝模式
	{
		for(u8 i=0;i<3;i++)
		{
			AxisRunningActionStruct xAction[1];
			CreateMultiAction(&xAction[0],X,-1,-1,ScrewInhaleCheck);
			
			AxisRunningActionStruct zUpAction[1];
			CreateMultiAction(&zUpAction[0],Z,-1,-1,ScrewInhaleCheck);
			AxisRunningActionStruct zDownAction[2];
			CreateSingAction(&zDownAction[0],Z,openScrewDriverCoord,OpenScrewDriver);	//开电批事情
			CreateSingAction(&zDownAction[1],Z,closeVacuumCoord,CloseVacuumValve);		//关真空事情
			VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zDownAction,2,zUpAction,1};
				
			CreateLinkageOneAxis(&_linkages[0],X,destCoord.X,xAction,1,NULL);
			CreateLinkageOneAxis(&_linkages[1],_currYAxis,destCoord.Y,NULL,0,NULL);
			CreateLinkageOneAxis(&_linkages[2],Z,destCoord.Z- Parameter.ScrewPara.length,NULL,0,&zAxisLink);
			_isScrewInhaleOK=true;
			MultiAxisLinkage(_linkages,3);
			if(_isScrewInhaleOK)
			{
				break;
			}
			else
			{
				SetOutPortState(VacuumValvePort,false);//关真空电磁阀
				ZMoveToCoord(Parameter.WorkHeight);
				GetScrew();
			}
		}
		if(!_isScrewInhaleOK)//如果多次吸取后还不正常，则报警
		{
			SetOutPortState(VacuumValvePort,false);//关真空电磁阀
			SetOutPortState(ScrewDriverPort,false);//关电批
			OpenAlarm();
			return false;
		}
		SetOutPortState(ScrewDriverPort,true);//开电批。防止电批提前为0时，电批没有打开
		if(Parameter.IsGetScrew)
		{
			_isTorqueOK=false;
			StartJS();
			AxisRunningActionStruct actions1[1];
			CreateMultiAction(&actions1[0],Z,-1,-1,CheckTorquePort);		
			MoveToCoordRunActionPara(Z,destCoord.Z,actions1,1,10*_clampZSpeed,_clampZSpeed);		
		}
		else
		{
			MoveToCoordRunActionPara(Z,destCoord.Z,NULL,0,10*_clampZSpeed,_clampZSpeed);
		}
	}	
	SetOutPortState(VacuumValvePort,false);//关真空电磁阀
	
	bool bl=GZJC_FGFY();//故障判断
	SetOutPortState(ScrewDriverPort,false);//关电批
	if(Parameter.LockScrewType==QGDown)//气缸下压锁螺丝模式
		SetOutPortState(LockQGPort,false); //关闭Z轴气缸
	return bl;
}
	
