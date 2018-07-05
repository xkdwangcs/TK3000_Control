#include "WorkControl.h"
#include "AppTasks.h"
#include "AxisLinkage.h"
#include "main.h"
#include "WorkControlBlow.h"
#include "WorkControlSuck.h"
#include "WorkControlOut.h"
#include "WorkControlCustomMove.h"
#include "MainForm.h"
#include "WorkDataFile.h"

//设备当前状态
DeviceStatusEnum _devCurrStatus;

//工作过程中，要显示到界面的相关数据
ShowDataStruct _showDatas;
u16 _machiningCount = 0; //加工个数，N次复位用
LeftRightPlatformEnum _currPlatformWork = LeftPlatform; 	//当前工作任务的平台
AxisIDEnum _currYAxis=Y1; 									//当前任务对应的Y轴
WorkTaskStartParaStruct* _taskStartPara; 				//由外部传入任务的相关任务启动参数

bool _isStartJS=false;//一次锁付是否已经开始计时，用于扭力事情先于开始计时事情的问题
u32 _startWorkTime;//一个平台开始打螺丝时间
u32 _oneScrewStartTime;//一个螺丝开始打的时间
u32 _oneScrewCompleTime;//一个螺丝完成的时长，ms
LinkageMotorStruct _linkages[3];

//正常打螺丝前的相关状态的检查。
//返回true为通过，否则表示不能运行工作任务
bool CheckStateBeforeWork(void);
//取镙丝过程函数
bool GetScrew(void);
//锁镙丝过程
bool ClampScrew(CoordStruct destCoord);

char _msgBuff[30];
//显示信息，带平台信息
void ShowWorkMsg_Plat(LeftRightPlatformEnum platform,char* msg)
{
	memset(_msgBuff, 0, sizeof(_msgBuff));
	if(Parameter.ProdType==DoublePlatform)
	{
		if(platform==LeftPlatform )
			sprintf(_msgBuff,"%s%s","左平台",msg);
		else
			sprintf(_msgBuff,"%s%s","右平台",msg);
		ShowWorkMsg(_msgBuff);
	}
	else
	{
		ShowWorkMsg(msg);
	}
}

//打开电批
void OpenScrewDriver()
{
	SetOutPortState(ScrewDriverPort,true);
}

//显示打螺丝时间
void ShowWorkTime(void)
{
	_showDatas.TotalTime=GetTimekeeperValue()-_startWorkTime;
	_showDatas.AvgTime=_showDatas.TotalTime/_showDatas.CurrScrewIndex;
	ShowWorkData(); 
}

//测试坐标相关的动作
void TestCoordMove(void)
{
	ShowWorkMsg("动作测试！");
	if(Parameter.GetScrewType==SuckScrew)//吸螺丝 
		TestCoordMoveSuck();
	else if(Parameter.GetScrewType==BlowScrew)//吹螺丝
		TestCoordMoveBlow();
	else if(Parameter.GetScrewType==OutScrew)//出螺丝
		TestCoordMoveOut();
}

//多轴同时复位
void MultiAxisPositionReset()
{
	ShowDebugMsg("轴复位");
	AxisIDEnum zAxis[1]={Z};
	MultiAxisMoveToPositionZeroAndMove(zAxis,1);
	
	AxisIDEnum axiss[3]={X,Y1,Y2};
	MultiAxisMoveToPositionZeroAndMove(axiss,3);
}

//自动复位（N次复位）
void AutoReset(void)
{
	_machiningCount++;
	//判断N次复位
	if(_machiningCount >= Parameter.AutoResetNum)
	{
		_machiningCount=0;
		ShowWorkMsg("N次自动复位...");
		MultiAxisPositionReset();
	}	
}

void SystemReset(void)
{    
    for(u8 i=0;i<OutPortNum;i++)
	{
		bool bl=GetOutPortState_Index(i);
		*(__IO uint8_t *) (OutputStateAddrBase + i)=bl;
	}
    *(__IO uint8_t *) (PermissionAddr)=_currPermission;//保存权限
	__disable_fault_irq(); 
	NVIC_SystemReset();
}

//关闭报警
void CloseAlarm(void)
{
	ShowDebugMsg("关闭报警");
	EnableAlarmTwinkle(false);//打螺丝过程中不能急停关闭此任务，会出错。暂时屏蔽
	SetOutPortState(AlarmSundPort,false);
	SetOutPortState(AlarmLampPort,false);
}

//打开报警。根据设置的参数，控制报警灯及报警声
void OpenAlarm()
{
	ShowDebugMsg("打开报警");
	SetOutPortState(ScrewDriverPort,false);//关电批
	if(Parameter.LockScrewType==QGDown)
		SetOutPortState(LockQGPort,false); //关闭Z轴气缸
	ZMoveToCoord(Parameter.WorkHeight);//移动到工作高度再报警
	SetOutPortState(AlarmLampPort,true); //开启报警灯
	if(Parameter.AlarmLampMode==Twinkle)
	{
		EnableAlarmTwinkle(true);
	}
	//LCDBeep(500);	
	SetOutPortState(AlarmSundPort,true);//开启报警声
	Delay_s_OS(Parameter.AlarmBuzzerTime);
	SetOutPortState(AlarmSundPort,false);//关闭报警声	
}

//正常打螺丝前的相关状态的检查。
//返回true为通过，否则表示不能运行工作任务
bool CheckStateBeforeWork(void)
{
	//如果不是主窗体则不进行任何动作,大屏不一定用
	ShowDebugMsg("正常流程前的相关检查");
//	if(_currFormIndex!=1 && _currFormIndex!=71 && _currFormIndex!=23)
//		return false;
	if(Parameter.GetScrewPara.Coord.X<1)
	{
        char* msg="取料坐标异常：X<1";
        //WriteLog(msg);
		ShowWorkMsg(msg);
		return false;
	}
	u16 coordCount=0;
	GetCoordList(_taskStartPara->CurrPlatform, &coordCount);
	if(coordCount<=0)
	{
        char* msg= "未设定坐标";
        //WriteLog(msg);
		ShowWorkMsg_Plat(_taskStartPara->CurrPlatform, msg);
		return false;
	}
	if(Parameter.IsWorkpieceCheck)//如果启用工件检测
	{
		IOPortStruct workpiecePort=LeftWorkpiecePort;
		if(_taskStartPara->CurrPlatform==RightPlatform)
			workpiecePort=RightWorkpiecePort;
		if(GetInPortState(workpiecePort)==0)
		{
			Delay_ms_OS(100);
			if(GetInPortState(workpiecePort)==0)
				return true;
		}
		char* msg= "未检测到工件！";
        //WriteLog(msg);
		ShowWorkMsg(msg);
		return false;
	}
	return true;
}

bool _isTorqueOK=false;//扭力信号是否已收到
//扭力信号检测
void CheckTorquePort(void)
{
	if(Parameter.FaultMode==IGNORE || !Parameter.IsGetScrew)//如果不取料
	{
		return;
	}
	if(!GetInPortState(TorquePort) && !_isTorqueOK)//因为电机减速要多走几步，防止重复检测
	{
		ShowDebugMsg("收到扭力信号");
		if(_isStartJS)//如果已经开始计时
			CalcLockTime();
		else
			_oneScrewCompleTime=0;//表明扭力信号刚开始就有了
		_isTorqueOK=true;
		SetOutPortState(ScrewDriverPort,false);//关电批
		StopAxisLinkageRun(_linkages,3);
	}
}

//取镙丝过程函数
bool GetScrew(void)
{
	ShowDebugMsg("准备取料！");
	if(Parameter.GetScrewType==SuckScrew)//吸螺丝
		return GetScrewSuck();
	else if(Parameter.GetScrewType==BlowScrew)//吹螺丝
		return GetScrewBlow(); 	
	else if(Parameter.GetScrewType==OutScrew)//出螺丝
		return GetScrewOut();
}

//开始计时
 void StartJS()
{
	ShowDebugMsg("开始计时");
	_isStartJS=true;
	_oneScrewStartTime=GetTimekeeperValue();//开始计时
}

//计算锁螺丝时长
void CalcLockTime(void)
{
	_oneScrewCompleTime=GetTimekeeperValue()-_oneScrewStartTime;
}

//锁镙丝过程
bool ClampScrew(CoordStruct destCoord)
{	
	ShowDebugMsg("开始锁付");
	if(Parameter.GetScrewType==SuckScrew)
		return ClampScrewSuck(destCoord);
	else if(Parameter.GetScrewType==BlowScrew)//吹螺丝
		return ClampScrewBlow(destCoord);
	else if(Parameter.GetScrewType==OutScrew)//出螺丝
		return ClampScrewOut(destCoord);	
}

LeftRightPlatformEnum _yReadyPlat=LeftPlatform;
void YReadyTask(void)
{
	LeftRightPlatformEnum plat=_yReadyPlat;//*(LeftRightPlatformEnum*)arg;
	AxisIDEnum yAxis=Y1;
	u16 coordCount=0;
	CoordStruct* coordList = GetCoordList(plat, &coordCount);
	if(coordCount<1)
		return;
	if(plat==RightPlatform)
	{
		yAxis=Y2;
	}
	ShowWorkMsg_Plat(plat, "进入准备状态");
	MoveToCoord(yAxis,coordList[0].Y,false);//移动到左平台Y轴的第一个坐标		
}

bool _isWorkHG=true;//本次工件是否合格
bool _isWaitGetWorkpiece=false;//是否已等待取件
//一个平台完成后的统计工作。
void WorkCompletedStatistics(void)
{
	if(!_isWorkHG)
	{
		WorkData.TotalStatistics.unQualifiedNumber++;
		WorkData.TotalStatistics.qualifiedNumber--;
		WriteWorkData();
	}
	ShowWorkTime();
	StopTimekeeper();		
	_taskStartPara->WorkTaskState=TaskNoRun;		
	ContinueNextPlatWork(_taskStartPara->CurrPlatform);	
}

//打完螺丝后，等待取件。要启用了工件检测、前放后取时起作用 
void WaitGetWorkpiece(LeftRightPlatformEnum platform)
{
	if(Parameter.IsWorkpieceCheck && Parameter.QFMode==QFHQ)//如果启用工件检测、前放后取
	{
		_isWaitGetWorkpiece=true;
		WorkCompletedStatistics();
		ShowWorkMsg_Plat(_taskStartPara->CurrPlatform, "等待取件");
		ContinueNextPlatWork(_taskStartPara->CurrPlatform);
		IOPortStruct workpiecePort;//工件检测光感输入口
		if(platform==LeftPlatform)
		{
			XYLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.AxisLength[Y1]); //X轴在取料点，Y轴在最大行程处
			workpiecePort=LeftWorkpiecePort;
		}
		else
		{
			XY2LinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.AxisLength[Y2]); //X轴在取料点，Y轴在最大行程处
			workpiecePort=RightWorkpiecePort;
		}
        
		while(true)//等待取工件
		{
			if(GetInPortState(workpiecePort))//收到取件完成信号
			{
				Delay_ms_OS(300);
				if(GetInPortState(workpiecePort))
					break;
			}
		}
		Delay_s_OS(Parameter.GainWorkpieceDelayTiem);//取件延时
	}	
}

//故障检测，浮高、滑牙
bool GZJC_FGFY(void)
{	
	if(Parameter.IsGetScrew)//如果取料，最后判断是否来扭力
	{
		ShowDebugMsg("故障检测判断");
		u32 fyMaxTime=999999;
		if(!_isTorqueOK)//表明Z轴已到目的坐标，在动作函数中未检测到的扭力
		{
			float nlDelay=Parameter.NLDelay*1000;
			u16 delayCount=(u16)(nlDelay/10);			
			for(u16 i=0;i<delayCount;i++)
			{
				Delay_ms_OS(10);
				if(!GetInPortState(TorquePort))
				{
					_isTorqueOK=true;
					SetOutPortState(ScrewDriverPort,false);//早点关电批，防止扫头
					break;
				}
			}
			if(GetInPortState(TorquePort) && !_isTorqueOK)//如果还无扭力信号
			{
				_oneScrewCompleTime=fyMaxTime;//设定一个大值，便于后面的判断。实际值不可能大于此值
			}
			else
			{
				CalcLockTime();
			}		
		}
		if(Parameter.FaultMode==WAIT)//如果故障等待
		{
			char msg[20]={0};
			bool isFG=false;
			if(_oneScrewCompleTime<Parameter.ScrewTimeDown)//小于设定的时长下限，浮高
			{
				isFG=true;
				sprintf(msg,"浮高，T=%d(ms)",_oneScrewCompleTime);		
				ShowWorkMsg(msg);
				if(Parameter.IsFGBJ) //浮高是否报警
				{
					//WriteLog(msg);
					OpenAlarm();
					return false;
				}
				return true;
			}

			if(!isFG && _oneScrewCompleTime>Parameter.ScrewTimeUP)//大于设定的时长上限，滑牙
			{
				if(_oneScrewCompleTime==fyMaxTime)
				{
					strcpy(msg,"滑牙，无扭力信号!");
				}
				else
				{
					sprintf(msg,"滑牙，T=%d(ms)",_oneScrewCompleTime);	
				}						
				ShowWorkMsg(msg);
				if(Parameter.IsFYBJ) //滑牙是否报警
				{
					//WriteLog(msg);
					OpenAlarm();
					return false;
				}
				return true;
			}
			else if(!isFG) //正常范围，OK
			{
				sprintf(msg,"锁紧正常，T=%d(ms)",_oneScrewCompleTime);			
				ShowWorkMsg(msg);
				return true;
			}
		}			
	}
	return true;
}

//锁螺丝的任务函数
void LockScrewTask(void *arg)
{
	SetCurrStatus(DevWorking,"设备正在工作中");
	_taskStartPara =(WorkTaskStartParaStruct*)arg;
	_taskStartPara->WorkTaskState=TaskRunning;
	if(!CheckStateBeforeWork())
	{
		_taskStartPara->WorkTaskState=TaskNoRun;
		return;//continue;
	}
	if(_taskStartPara->CurrPlatform==LeftPlatform)
	{
		_currPlatformWork=LeftPlatform;
		_currYAxis=Y1;
	}
	else
	{
		_currPlatformWork=RightPlatform;
		_currYAxis=Y2;
	}
	u16 coordCount=0;
	CoordStruct* coordList = GetCoordList(_currPlatformWork, &coordCount);
	bool currWorkState=true;//工作状态，true时为正常，false时为故障
	StartTimekeeper();
	_startWorkTime=GetTimekeeperValue();//开始打螺丝时的时间
	if(_taskStartPara->StartSource==FJQD)
		Delay_s_OS(Parameter.StartWorkDelay);		
	ZMoveToCoord(Parameter.WorkHeight);
	if(_currPlatformWork==LeftPlatform)
		WorkBeforMove_Left();
	else
		WorkBeforMove_Right();
	_isWorkHG=true;//本工件是否合格
	//不要打完就要累加个数
	WorkData.DayStatistics.totalNumber++; 
	WorkData.TotalStatistics.totalNumber++;
	WorkData.TotalStatistics.qualifiedNumber++;
	WriteWorkData();
	ShowWorkTime();
	for (u16 coordIndex = 0; coordIndex < coordCount; )//为什么局部变量不行？coordIndex++还是等于0
	{		
		CoordStruct currCoord=coordList[coordIndex];
		_showDatas.CurrScrewIndex =coordIndex+1;
		ShowWorkMsg_Plat(_currPlatformWork,"正常加工中...");
		ShowWorkData();
		currWorkState = GetScrew();//取镙丝过程   

BreakLoop: 
		if(!currWorkState)//如果有故障，等待用户响应
		{
			_isWaitSemaphore=true;
			while(true)
			{
				//if(xSemaphoreTake(xSemaphoreRedo, portMAX_DELAY) == pdTRUE)
				//当为0时可以用于 对信号量进行轮询
				if(xSemaphoreTake(_xSemaphoreRedo, 0) == pdTRUE)
				{
					CloseAlarm();
					ShowWorkMsg_Plat(_currPlatformWork,"重锁螺丝！");
					break;
				}
				if(xSemaphoreTake(_xSemaphoreNext, 0) == pdTRUE)
				{
					coordIndex++;
					CloseAlarm();
					ShowWorkMsg_Plat(_currPlatformWork,"锁下一个螺丝！");
					_isWorkHG=false;
					break;
				}
				Delay_ms_OS(200);
				ShowWorkTime();
			}
			continue;
		}
		_isStartJS=false;//开始计时标示置否
		_oneScrewCompleTime=2;//螺丝时长置位，特殊数字
		currWorkState =ClampScrew(currCoord);//锁镙丝过程 
		if(!currWorkState) 
			goto BreakLoop;
		coordIndex++;
		ShowWorkTime();
	}	

	//完成了所有的坐标点
	if(_currPlatformWork==LeftPlatform)
		WorkCompletedMove_Left();
	else
		WorkCompletedMove_Right();
	if(!_isWaitGetWorkpiece)
	{
		WorkCompletedStatistics();
	}
	SetCurrStatus(DevReady,"设备准备就绪");
	ShowWorkMsg_Plat(_currPlatformWork,"加工完毕！");	
}
	
//一个平台(左或右)开始锁镙丝的入口函数
void WorkRun(void *arg)
{
	for(;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  
		if(Parameter.ProdType!=CustomMovePlat)
		{
			LockScrewTask(arg);
		}
		else
		{
			RunCustomMove(arg);
		}
	}
}
