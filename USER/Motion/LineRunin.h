#ifndef __LINERUNIN_H
#define __LINERUNIN_H

#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include "stm32f4xx.h"
#include "absacc.h"
#include "string.h"
#include "math.h"
#include "MotorControl.h"

//是否已经发送了脉冲（定时器已经被触发）
extern bool _isTimed[MotorPortNum];
	 
//多轴直线插补运行，目前只支持任意两轴直线插补	 
void LineRuninRun_Motor(MotorIDEnum* motorList,s32* pulseList, u8 count,float runSpeed);

void PWMProc(MotorIDEnum motorID);	 
	 
#ifdef __cplusplus
}
#endif

#endif
