#ifndef __TIMECLOCK_H
#define __TIMECLOCK_H

#ifdef __cplusplus
extern "C" {
#endif 


#include "stm32f4xx.h"
#include "stdbool.h"
#include "Public/Handlers.h"
	
/*****
一个计时器里面包含多个定时器
******/		

#define SoftTimerCount 10 //软定时器的最大数目
	
typedef enum
{
	SingTimer,	//单次定时器
	LoopTimer,	//循环的多次定时器
}SoftTimerType; //软件定时器类型	
	
typedef struct
{
	volatile SoftTimerType TimerType; 			//定时器类型
	volatile bool IsUse; 						//是否使用定时器
	volatile bool IsEnable;						//定时器是否使能
	volatile u32 Count;  						//当前定时器中的计时
	volatile u32 PreValue;						//计时器的目标值(ms),到达这个值将引发事件(调用回调)
	volatile OneIntParameterHandler CallFunc;	//回调函数，返回的参数为定时器的索引，从0开始的
	
}SoftTimerStruct;//软定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 

typedef struct
{
	volatile u32 TimerValue; 				//当前计时器的值(ms)
	volatile SoftTimerStruct SoftTimers[SoftTimerCount];	//定时器列表，定时器的索引由调用者自己维护
}TimekeeperStruct;//计时器结构体

//初始化计时器
void IniTimekeeper(void);
//获取计时器当前的计时数，单位ms
u32 GetTimekeeperValue(void);
//开始计时器
void StartTimekeeper(void);
//复位计时器
void ResetTimekeeper(void);
//停止计时器
void StopTimekeeper(void);

//创建一个软定时器。如果创建成功则返回0开始的索引，否则返回0xFF
//timerType:定时器类型，perValue:定时器预装值，当计时器累计到此值，将引发回调。callFunc：回调函数
u8 CreateSoftTimer(SoftTimerType timerType,u32 perValue,OneIntParameterHandler callFunc);
//删除一个定时器，释放定时器
void DeleteSoftTimer(u8 timerIndex);
//开始一个定时器
void StartSoftTimer(u8 timerIndex);
//复位一个定时器
void ResetSoftTimer(u8 timerIndex);
//停止一个定时器
void StopSoftTimer(u8 timerIndex);


#ifdef __cplusplus
}
#endif

#endif
