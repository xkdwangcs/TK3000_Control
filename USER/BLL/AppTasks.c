﻿#include "AppTasks.h"
#include "MainForm.h"
//#include "IOTest.h"
#include "main.h"
#include "usbd_usr.h"
#include "USBFunc.h"
//#include "MoveFileExImportForm.h"
//#include "MainFormCustomMove.h"
//#include "LogFile.h"

portTickType _xLastWakeTime;
xTaskHandle _showRealDataHandle;    //显示实时坐标的任务句柄
xTaskHandle _workRunLeftHandle;  	//左平台打螺丝句柄
xTaskHandle _workRunRightHandle;  	//右平台打螺丝句柄
xTaskHandle _positionResetHandle;   //位置复位句柄
xTaskHandle _alarmHandle;           //报警灯闪烁句柄
xTaskHandle _IOStateShowHandle;		//IO口状态显示
xTaskHandle _dateTimeShowHandle;	//界面上时间的显示
xTaskHandle _usbHostHandle;			//USBHost
xTaskHandle _fatfsOperateHandle;	//FATFS相关操作的任务句柄,包括USB的操作
xTaskHandle _setUSBDirverHandle;	//开始USB存储设备的任务句柄
xTaskHandle _inportEventHandle;		//输入口事件任务句柄

static BaseType_t xHigherPriorityTaskWoken;  
static BaseType_t xHigherPriorityTaskWokenRight;  
//重打信号量
SemaphoreHandle_t _xSemaphoreRedo;
//打一下信号量
SemaphoreHandle_t _xSemaphoreNext;
//是否已进入故障等待状态
bool _isWaitSemaphore=false;
//左平台锁螺丝任务参数，内部用。任务函数通过参数arg获取
WorkTaskStartParaStruct _leftStartPara={LeftPlatform,Key,TaskNoRun};
//右平台锁螺丝任务参数，内部用。任务函数通过参数arg获取
WorkTaskStartParaStruct _rightStartPara={RightPlatform,Key,TaskNoRun};
//一个执行FATFS操作相关的函数句柄
EventHandlerNoPara _oneFatfsOperateFunc=NULL;

//输入口事件检测并执行
void InportEventCheckRun(void);
void InportEventTask(void *arg);

void Delay_ms_OS(u16 ms)
{
	//vTaskDelay(ms/portTICK_RATE_MS);
	_xLastWakeTime=xTaskGetTickCount();
	vTaskDelayUntil(&_xLastWakeTime,ms/portTICK_RATE_MS);
}

//延时，秒
void Delay_s_OS(float time_s)
{
	u32 time_int = (u32) (time_s * 1000.0);
	Delay_ms_OS(time_int);	
}

//显示实时数据
void ShowRealData(void *arg)
{
	while(true)
	{
		InportEventCheckRun();
		MultiAxisCoordStruct* mas=GetRealTimeCoord();
		if(Parameter.ProdType==CustomMovePlat)
		{
			//ShowCurrCoord_cm(mas);
		}
		else
		{
			ShowCurrCoord(mas);
		}
		Delay_ms_OS(200);   
	}
}

void USBHost(void *arg)
{
	while(true)
	{
        USBH_Process(&USB_OTG_Core, &USB_Host);
        Delay_ms_OS(10);
	}
}

void AlarmTwinkle(void *arg)
{
	while(true)
	{
		SetOutPortState(AlarmLampPort,true);
		Delay_ms_OS(500);
		SetOutPortState(AlarmLampPort,false);
		Delay_ms_OS(500);        
	}
}

void PositionResetTask(void *arg)
{
	for(;;)
	{
		/* 等待通知，进入阻塞 */  
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY);  
		//xTaskNotifyWait();
		PositionResetMove();
	}
}

bool _isOpenUSBDirver=false;
//设备作为虚拟磁盘的打开或关闭的任务，内部
void SetOpenMassTask(void *arg)
{
	for(;;)
	{
		/* 等待通知，进入阻塞 */  
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY);  
		if(_isOpenUSBDirver)
		{
			_currUSBMode=DirverMode;
			usbd_OpenMassStorage();
		}
		else
		{
			_currUSBMode=HostMode;
			usbd_CloseMassStorage();
		}
	}
}

//开启或关闭设备作为虚拟磁盘，外部调用
void SetUSBMassStorage(bool isOpen)
{
	_isOpenUSBDirver=isOpen;
	xTaskNotifyGive(_setUSBDirverHandle);
}

//运行Y轴准备任务
void RunYReadyTask(LeftRightPlatformEnum plat)
{
	_yReadyPlat=plat;
	_oneFatfsOperateFunc=YReadyTask;
	StartFATFSTask();
}

void DeviceAndTaskInit(void)
{
	DeviceInit();
	xTaskCreate(PositionResetTask, "PosReset", configMINIMAL_STACK_SIZE, NULL, 3, &_positionResetHandle); 
	xTaskCreate(AlarmTwinkle, "AlarmTwin", configMINIMAL_STACK_SIZE, NULL, 3, &_alarmHandle);
	vTaskSuspend(_alarmHandle);
	xTaskCreate(WorkRun, "WorkRunLeft", 4096, &_leftStartPara, 3, &_workRunLeftHandle ); 
	xTaskCreate(WorkRun, "WorkRunRight", 4096, &_rightStartPara, 3, &_workRunRightHandle ); 
  //xTaskCreate(ShowIOState, "IOState", configMINIMAL_STACK_SIZE, NULL, 3, &_IOStateShowHandle);
	//vTaskSuspend(_IOStateShowHandle);
  //xTaskCreate(ShowDataTime, "DateTimeShow", configMINIMAL_STACK_SIZE, NULL, 3, &_dateTimeShowHandle);
  xTaskCreate(ShowRealData, "ShowRealData", configMINIMAL_STACK_SIZE, NULL, 3, &_showRealDataHandle);
	xTaskCreate(InportEventTask, "InportEvnet", configMINIMAL_STACK_SIZE, NULL, 3, &_inportEventHandle);	
}

//执行与FATFS相关所有操作任务的函数
void FATFSOperate_ALL(void *arg)
{
	DeviceAndTaskInit();//设备初始化及任务初始化只运行一次
    for(;;)
	{
		//等待通知，进入阻塞 
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
		if(_oneFatfsOperateFunc!=NULL)
		{
			_oneFatfsOperateFunc();
			_oneFatfsOperateFunc=NULL;
			continue;
		}
		
		//USB的操作任务，要确保其它任务不会执行到下面
		SetUSBHostTaskEnable(false);//执行USB相关任务时要先关闭Host任务，否则读写USB会出错
		USBOpereateTask();
    //MoveOpereateTask();
		//LogFileOpereateTask();//非USB操作不能放在此处，会打开USBHost连接任务，占用资源
		SetUSBHostTaskEnable(true);//在此要打开Host任务，否则不能多次连USB
    }        
}

//开始FATFS操作的相关任务
void StartFATFSTask(void)
{
	if(eTaskGetState(_fatfsOperateHandle)==eRunning)
		return;
    xTaskNotifyGive(_fatfsOperateHandle);  
}

//任务初始化
void InitTask(void *arg)
{
	//是否已进入故障等待状态
	_isWaitSemaphore=false;		
	//重打信号量
	_xSemaphoreRedo= xSemaphoreCreateBinary();  //二值信号量;
	//打一下信号量
	_xSemaphoreNext= xSemaphoreCreateBinary();  //二值信号量;

	xTaskCreate(FATFSOperate_ALL, "USBOP", 6000, NULL, 3, &_fatfsOperateHandle);
	xTaskCreate(USBHost, "USBHost", configMINIMAL_STACK_SIZE, NULL, 3, &_usbHostHandle);
    vTaskSuspend(_usbHostHandle);    
	xTaskCreate(SetOpenMassTask, "OpenMass", configMINIMAL_STACK_SIZE, NULL, 3, &_setUSBDirverHandle);    
	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb); //初始化USBHost
    //WriteLog("设备初始化成功！");
    vTaskDelete(NULL);
    for(;;)
    {
    
    }
}

bool _isMotionTest=false;//是否正在动作测试
void StartTestCoordMove()
{
	if(IsWorkRunning())
		return;
	ShowMainForm(0);
	//xTaskNotifyGive(_testCoordMoveHandle);
	_oneFatfsOperateFunc=TestCoordMove;
	StartFATFSTask();
	_isMotionTest=true;
}

//停止所有的任务
void StopTasks(void)
{	
	vTaskSuspend(_fatfsOperateHandle);  
    vTaskSuspend(_showRealDataHandle); 
	vTaskSuspend(_workRunLeftHandle);
	vTaskSuspend(_workRunRightHandle); 
	vTaskSuspend(_positionResetHandle); 
	vTaskSuspend(_alarmHandle);
	vTaskSuspend(_IOStateShowHandle);  	
    vTaskSuspend(_dateTimeShowHandle); 		
    //vTaskSuspend(_usbHostHandle);	
    //vTaskSuspend(_setUSBDirverHandle);
	//vTaskSuspendAll();//不能挂起调度器否则电机有时停不下来
}

//急停
void ScramStop(StopSource stopSource)
{
//	if(stopSource==ScreenStop)
//	{
//		WriteLog("在LCD上按下了急停！");
//	}
//	else if(stopSource==KeyStop)
//	{
//		WriteLog("按下了急停按钮！");
//	}
	
	StopTasks();
	StopAllAxis();
	//StopTasks();
	SetOutPortState(ScrewDriverPort,false);	//电批信号关闭
	SetOutPortState(VacuumValvePort,false);	//真空信号关闭
	CloseAlarm();
  DeviceStatusSRQ(DevScram,"设备已急停");
	//WriteLogBuffToFile();//写入日志缓存到文件
}

void btnInportEvent_OKClick(u16 keyCode)
{
	CloseAlarm();	
	SystemReset();
}

void btnInportEventCancelClick(u16 keyCode)
{
	*(__IO uint8_t *)(InportEventCancelTag)=1;
	CloseAlarm();	
	SystemReset();
}

InportEventStruct* _ies;
void InportEventTask(void *arg)
{
	for(;;)
	{
		/* 等待通知，进入阻塞 */  
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
		StopTasks();		
		StopAllAxis();
		SetOutPortState(ScrewDriverPort,false);	//电批信号关闭
		SetOutPortState(VacuumValvePort,false);	//真空信号关闭
		OpenAlarm();
		ShowMessgeForm(_ies->Msg,btnInportEvent_OKClick,btnInportEventCancelClick);
	}
}

//输入口事件检测并执行
void InportEventCheckRun(void)
{
	if(Moves.InportEventCount==0)
		return;
	if(*(__IO uint8_t *)(InportEventCancelTag)==1)
		return;
	for(u16 i=0;i<Moves.InportEventCount;i++)
	{
		InportEventStruct* ies = &Moves.InportEvents[i].InportEvent;
		if(GetInPortState_Index(ies->PortIndex)==ies->InputState)
		{
			_ies=ies;
			xTaskNotifyGive(_inportEventHandle);
		}
	}
}

//如果正在工作中、故障等待中，电机复位中返回true，否则返回false
bool IsWorkRunning(void)
{
	if(_isMotionTest)
		return true;
	if(eTaskGetState(_positionResetHandle)==eRunning)
		return true;
	if(eTaskGetState(_positionResetHandle)==eRunning)
		return true;
	if(eTaskGetState(_workRunLeftHandle)==eRunning) 
		return true;
	if(eTaskGetState(_workRunRightHandle)==eRunning)
		return true;
	if(eTaskGetState(_workRunLeftHandle)==eBlocked) //故障等待时
		return true;
	if(eTaskGetState(_workRunRightHandle)==eBlocked)
		return true;
	return false;
}

//开始位置复位任务
void StartPositionReset(void)
{
	if(IsWorkRunning())
		return;
	//vTaskNotifyGiveFromISR( _positionResetHandle ,&xHigherPriorityTaskWoken);  //这个函数更快
	xTaskNotifyGive(_positionResetHandle);
}

//双平台时，正在工作的平台工作任务调用此函数，使另一个平台进入准备状态平台继续正常工作
//platformComplate:调用此函数的任务平台，例如：LeftPlat表示左平台已完成(包括前放后取时的等待取件)，右平台可以开始工作了
void ContinueNextPlatWork(LeftRightPlatformEnum platformComplate)
{
	if(Parameter.ProdType==SinglePlatform)//如果是单平台则不存在这样的操作
		return;
	if(platformComplate==RightPlatform)
	{
		if(_leftStartPara.WorkTaskState!=YReadyed)//如果不是准备状态，表示用户未按下另一启动按钮
			return;
		ShowWorkMsg_Plat(LeftPlatform, "开始工作");
		_leftStartPara.WorkTaskState=TaskRunning;
		StartWorkTask(LeftPlatform,_leftStartPara.StartSource);
	}
	else
	{
		if(_rightStartPara.WorkTaskState!=YReadyed)
			return;
		ShowWorkMsg_Plat(RightPlatform, "开始工作");
		_rightStartPara.WorkTaskState=TaskRunning;
		StartWorkTask(RightPlatform,_rightStartPara.StartSource);
	}
}

//开始打螺丝任务
void StartWorkTask(LeftRightPlatformEnum platform,StartXinhaoSource sxs)
{
	//如果不是主窗体则不进行任何动作
	//if(_currFormIndex!=1 && _currFormIndex!=71 && _currFormIndex!=91)
		//return;	
	if(_devCurrStatus!=DevReady && _devCurrStatus!=DevWorking)
		return;
	if(platform==RightPlatform && Parameter.ProdType==SinglePlatform)//如果是单平台不响应右平台启动
		return;
	if(eTaskGetState(_positionResetHandle)==eRunning)//如果正复位则不响应
		return;
	if(_isMotionTest)//如果正在动作测试
		return;
    char* msg= "未检测到工件！";
	if(platform==LeftPlatform)
	{
		//如果右平台正在工作(包括正常工作、故障等待)，则左平台进入准备状态(Y1移到第一个坐标)
		if(_rightStartPara.WorkTaskState==TaskRunning)
		{
			_leftStartPara.WorkTaskState=YReadyed;//左平台Y轴已准备就绪
			RunYReadyTask(LeftPlatform);
			return;
		}
		eTaskState taskState=eTaskGetState(_workRunLeftHandle);
		if(taskState==eSuspended) //当Y轴进入准备状态后，由另一平台任务进入此代码时任务状态变成eReady，why??
		{
			if(Parameter.IsWorkpieceCheck && GetInPortState(LeftWorkpiecePort))//如果启用了工件检测，但没检测到工件
			{
				//WriteLog(msg);
				ShowWorkMsg(msg);//这种提示都要有，都要写入日志文件
				return;
			}
			_leftStartPara.StartSource=sxs;
			xTaskNotifyGive(_workRunLeftHandle);
		}
		else if(_isWaitSemaphore)
		{
			_isWaitSemaphore=false;
			//在中断中发送信号量，以FromISR结尾的函数具有保护功能，如果在任务中发送信号量可使用xSemaphoreGive。
			xSemaphoreGiveFromISR(_xSemaphoreNext, &xHigherPriorityTaskWoken );  
		}
	}
	else
	{
		//如果右平台正在工作(包括正常工作、故障等待)，则左平台进入准备状态(Y1移到第一个坐标)
		if(_leftStartPara.WorkTaskState==TaskRunning)
		{
			_rightStartPara.WorkTaskState=YReadyed;//右平台Y轴已准备就绪
			RunYReadyTask(RightPlatform);
			return;
		}
		eTaskState taskState=eTaskGetState(_workRunRightHandle);
		if(taskState==eSuspended) //当Y轴进入准备状态后，由另一平台任务进入此代码时任务状态变成eReady，why??
		{
			if(Parameter.IsWorkpieceCheck && GetInPortState(RightWorkpiecePort))//如果启用了工件检测，但没检测到工件
			{
				//WriteLog(msg);
				ShowWorkMsg(msg);
				return;
			}
			_rightStartPara.StartSource=sxs;
			xTaskNotifyGive(_workRunRightHandle);
		}
		else if(_isWaitSemaphore)
		{
			_isWaitSemaphore=false;
			//在中断中发送信号量，以FromISR结尾的函数具有保护功能，如果在任务中发送信号量可使用xSemaphoreGive。
			xSemaphoreGiveFromISR(_xSemaphoreNext, &xHigherPriorityTaskWokenRight);  
		}
	}	
}

//故障后重打本坐标
void RepeatWork(void)
{
	if(_isWaitSemaphore)
	{
		ShowWorkMsg("重打信号");
		_isWaitSemaphore=false;
		//在中断中发送信号量，以FromISR结尾的函数具有保护功能，如果在任务中发送信号量可使用xSemaphoreGive。
		//xSemaphoreGiveFromISR(_xSemaphoreRedo, &xHigherPriorityTaskWoken ); 
		xSemaphoreGive(_xSemaphoreRedo);
	}
}

//USBHost任务关闭或开启
void SetUSBHostTaskEnable(bool isEnable)
{
    if(_usbHostHandle==NULL)
        return;
	if(isEnable)
		xTaskResumeFromISR(_usbHostHandle);
	else
		vTaskSuspend(_usbHostHandle);
}

//报警灯闪烁使能
void EnableAlarmTwinkle(bool isEnable)
{
	if(isEnable)
		vTaskResume(_alarmHandle);
	else
		vTaskSuspend(_alarmHandle);
}

//实时坐标显示任务关闭或开启
void SetRealCoordTaskEnable(bool isEnable)
{
    if(_showRealDataHandle==NULL)
        return;
	if(isEnable)
		vTaskResume(_showRealDataHandle);
	else
		vTaskSuspend(_showRealDataHandle);
}

//IO状态显示任务关闭或开启
void SetIOStateTaskEnable(bool isEnable)
{
    if(_IOStateShowHandle==NULL)
        return;
	if(isEnable)
		vTaskResume(_IOStateShowHandle);
	else
		vTaskSuspend(_IOStateShowHandle);
}

//时间显示任务关闭或开启
void SetDateTimeTaskEnable(bool isEnable)
{
    if(_dateTimeShowHandle==NULL)
        return;
    if(isEnable)
		vTaskResume(_dateTimeShowHandle);
	 else 
		vTaskSuspend(_dateTimeShowHandle);
}
