#ifndef __TCURVE_H
#define __TCURVE_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "gpio.h"
#include "MotorControl.h"
#include "CurveBase.h"


//��������������һ�����߲�����
void CreateTCurve(MotorIDEnum motorID, u32 pulseCount,float runSpee,float accSpeed);
//��ȡָ������ٶ�Ԥװֵ,�岹�Ӽ��ٵĵ�һ�ַ�ʽ
u16 GetCurrPeriodValue_TCurve_Out(MotorIDEnum motorID,u32 currPulseIndex);
//PWM��ʱ���жϵ���
void PWM_T(MotorIDEnum motorID);


#ifdef __cplusplus
}
#endif

#endif

