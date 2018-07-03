#ifndef __AXISRUNNINGACTION_H
#define __AXISRUNNINGACTION_H 

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "AxisControl.h"
#include "stdbool.h"

//电机运行过程中要插入事情函的函数指针
typedef void (*MotorRunningActionHandler)(void);

typedef enum
{
	SingleAction,//单次事情，只在指定的一个坐标上执行一次
	MultiAction,//要多次执行的事情，在指定脉冲间隔上多次执行
}ActionTypeEnum;//事情类型

typedef struct
{
	float ActionCoord;						//要动作的坐标值，在此坐标时插入动作，要小于运行目的坐标
	u32 ActionPulse;							//由ActionCoord计算得出的脉冲数，外部不用赋值，只是为了调用方便
	bool isCallAction;						//Action是否已被调用
}SingleActionParaStruct;					//单次事情结构

typedef struct
{
	float StartCoord;						//要动作的起始坐标值，小于0时表示从运动开始执行
	float EndCoord;							//要动作的结束坐标值，小于0时表示执行到轴目的坐标为止
	s32 StartPulse;							//与StartCoord对应的绝对脉冲值,内部用
	s32 EndPulse;							//与EndCoord对应的绝对脉冲值,内部用
	//u32 PulseSpace;							//动作脉冲间隔，在合理范围内不占用电机运行资源
}MultiActionParaStruct;						//多次事情结构

typedef struct
{
	ActionTypeEnum ActionType;				//事情类型
	MotorRunningActionHandler ActionFunc;	//动作函数句柄
	SingleActionParaStruct SingleActionPara;//单次事情参数
	MultiActionParaStruct MultiActionPara;	//多次事情参数	
}AxisRunningActionStruct;					//轴运行过程中要插入的事情结构

//创建一个单次事情
//axisRunningAction:要添加到的事情列表成员的指针， axisID:轴号,actionCoord：动作的坐标值，actionFunc:动作回调函数
void CreateSingAction(AxisRunningActionStruct* axisRunningAction,AxisIDEnum axisID,float actionCoord,MotorRunningActionHandler actionFunc);
//创建一个多次事情
//axisRunningAction:要添加到的事情列表成员的指针， axisID:轴号,
//startCoord:起始坐标，<0时为从轴起始坐标开始，endCoord:结束坐标,<0时同上, actionFunc:动作回调函数
void CreateMultiAction(AxisRunningActionStruct* axisRunningAction,AxisIDEnum axisID,float startCoord,float endCoord,MotorRunningActionHandler actionFunc);

#ifdef __cplusplus
}
#endif
#endif
