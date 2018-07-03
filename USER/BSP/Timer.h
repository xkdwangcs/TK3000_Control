#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "MotorControl.h"

//#define T_FREQ 6000000	
	
//定时器初始化配置	
void Timer_Config(void);
//PWM定时器启动/停止设置。motorID：电机号，setState：启动/停止
void PWM_TIMControl(MotorIDEnum motorID, FunctionalState setState);

void TIMPeriodSet(MotorIDEnum id,u32 period);

#ifdef __cplusplus
}
#endif

#endif
