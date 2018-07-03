#ifndef __TCURVE_H
#define __TCURVE_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "gpio.h"
#include "MotorControl.h"
#include "CurveBase.h"


//根据脉冲数创建一个曲线参数表
void CreateTCurve(MotorIDEnum motorID, u32 pulseCount,float runSpee,float accSpeed);
//获取指定点的速度预装值,插补加减速的第一种方式
u16 GetCurrPeriodValue_TCurve_Out(MotorIDEnum motorID,u32 currPulseIndex);
//PWM定时器中断调用
void PWM_T(MotorIDEnum motorID);


#ifdef __cplusplus
}
#endif

#endif

