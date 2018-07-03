#ifndef _APPTASKS_H
#define _APPTASKS_H

#include "stm32f4xx.h"
#include "WorkControl.h" 
#include "AxisLinkage.h" 
#include "stdbool.h"
// FreeRTOS head file, add here.
#include "FreeRTOS.h"
#include "task.h"//包含这个头文件之前一定要包含FreeRTOS.h
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"  

#ifdef __cplusplus
 extern "C" {
#endif 

extern xTaskHandle _workRunLeftHandle;      //左平台打螺丝句柄
extern xTaskHandle _workRunRightHandle;     //右平台打螺丝句柄
extern SemaphoreHandle_t _xSemaphoreRedo;    //重打信号量
extern SemaphoreHandle_t _xSemaphoreNext;    //打一下信号量   
extern bool _isWaitSemaphore;               //是否已进入故障等待状态
//一个执行FATFS操作相关的函数句柄
extern EventHandlerNoPara _oneFatfsOperateFunc;
	 
//操作系统的延时，ms
void Delay_ms_OS(u16 msValue);
//操作系统的延时，s
void Delay_s_OS(float sValue);
//任务初始化
void InitTask(void *arg);
//开始动作测试任务
void StartTestCoordMove(void);
//急停
void ScramStop(StopSource stopSource);  
//打开或关闭设备作为虚拟U盘
void SetUSBMassStorage(bool isOpen);
//开始位置复位任务
void StartPositionReset(void);   
//USBHost任务关闭或开启
void SetUSBHostTaskEnable(bool isEnable);
//开始FATFS操作的相关任务
void StartFATFSTask(void);
//报警灯闪烁使能
void EnableAlarmTwinkle(bool isEnable);
//实时坐标显示任务关闭或开启
void SetRealCoordTaskEnable(bool isEnable);
//IO状态显示任务关闭或开启
void SetIOStateTaskEnable(bool isEnable);
//时间显示任务关闭或开启
void SetDateTimeTaskEnable(bool isEnable);
//双平台时，正在工作的平台工作任务调用此函数，使另一个平台进入准备状态平台继续正常工作
//platformComplate:调用此函数的任务平台，例如：LeftPlat表示左平台已完成(包括前放后取时的等待取件)，右平台可以开始工作了
void ContinueNextPlatWork(LeftRightPlatformEnum platformComplate);
//开始打螺丝任务
void StartWorkTask(LeftRightPlatformEnum platform,StartXinhaoSource sxs);
//故障后重打本坐标
void RepeatWork(void);
//如果正在工作中、故障等待中，电机复位中返回true，否则返回false
bool IsWorkRunning(void);

#ifdef __cplusplus
}
#endif

#endif
