#ifndef __WORKCONTROL_H
#define __WORKCONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "CoordFile.h"
#include "Parameter.h"
#include "AxisLinkage.h"
#include "AxisControl.h"
#include "gpio.h"   
#include "MotionProc.h"
#include "Timekeeper.h"
	 
	 
typedef enum
{
	DeviceIniting,		//设备初始化中
	TaskInitFail,		//任务初始化失败
	RegWait,			//等待注册
	DevReady,			//设备准备就绪	
	DevWorking,			//设备正在工作中	
	DevScram,			//急停中，查询与主动上送
	USBInsert,			//USB已插入，查询与主动上送	
}DeviceStatusEnum;	 
	 
typedef struct
{
	u16 CurrScrewIndex;	//当前加工的螺丝序号，从1开始的
	u32 TotalTime;		//一个工件的总时间
	u32 AvgTime;		//平均时间	
}ShowDataStruct;		//要显示到界面上的数据,定义为结构体，方便以后扩展

typedef enum
{
	 Key,			//按键
	 FJQD,			//放件启动
}StartXinhaoSource;	//启动信号来源

typedef enum 
{
	KeyStop,	//按键急停
	ScreenStop,	//屏幕急停
}StopSource;	//急停中断的信号来源

typedef enum 
{
	TaskNoRun,  			//任务未运行状态
	TaskRunning,			//任务运行状态
	YReadyed,				//Y轴已进入准备就绪状态
}WorkTaskStateEnum;			//锁螺丝任务的状态，用于一个函数两个任务中状态的识别。要在任务中能改变外部的全局任务参数

typedef struct 
{
	LeftRightPlatformEnum CurrPlatform;	//当前要工作的平台
	StartXinhaoSource StartSource;		//启动的源
	WorkTaskStateEnum WorkTaskState;	//任务的状态
}WorkTaskStartParaStruct;				//锁螺丝任务启动的信息结构

//设备当前状态
extern DeviceStatusEnum _devCurrStatus;
//工作过程中，要显示到界面的相关数据
extern ShowDataStruct _showDatas;
//开始打螺丝时间
extern u32 _startWorkTime;
//双平台时，另一平台准备
extern LeftRightPlatformEnum _yReadyPlat;

//打开电批
void OpenScrewDriver(void);
//显示打螺丝时间
void ShowWorkTime(void);
//显示信息
void ShowWorkMsg_Plat(LeftRightPlatformEnum platform,char* msg);
//显示调试信息
void ShowDebugMsg(char* msg);
//判断浮高
void JudgeSuperficial(void);
//一个平台正在工作，另一平台进入准备状态时的Y轴移动到第一个坐标
void YReadyTask(void);
//一个平台(左或右)开始锁镙丝的入口函数
void WorkRun(void *arg);
//自动复位（N次复位）
void AutoReset(void);
//打完螺丝后，等待取件。要启用了工件检测、前放后取时起作用 
void WaitGetWorkpiece(LeftRightPlatformEnum platform);
//测试坐标相关的动作
void TestCoordMove(void);
//位置复位
void PositionReset(void);
//系统复位
void SystemReset(void);
//打开报警。根据设置的参数，控制报警灯及报警声
void OpenAlarm(void);
//关闭报警
void CloseAlarm(void);
//系统复位，会保存复位前的相关状态
void SystemReset(void);


/***************具体实现文件调用******************/
extern LinkageMotorStruct _linkages[3];
extern bool _isTorqueOK;//扭力信号是否已收到
extern AxisIDEnum _currYAxis;//当前任务对应的Y轴

//开始计时
void StartJS(void);
//计算锁螺丝时长
void CalcLockTime(void);
//扭力信号检测
void CheckTorquePort(void);
//故障检测，浮高、滑牙
bool GZJC_FGFY(void);

#ifdef __cplusplus
}
#endif

#endif
