#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "MotorControl.h"

//#define T_FREQ 6000000	
	
//��ʱ����ʼ������	
void Timer_Config(void);
//PWM��ʱ������/ֹͣ���á�motorID������ţ�setState������/ֹͣ
void PWM_TIMControl(MotorIDEnum motorID, FunctionalState setState);

void TIMPeriodSet(MotorIDEnum id,u32 period);

#ifdef __cplusplus
}
#endif

#endif
