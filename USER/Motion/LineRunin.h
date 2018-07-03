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

//�Ƿ��Ѿ����������壨��ʱ���Ѿ���������
extern bool _isTimed[MotorPortNum];
	 
//����ֱ�߲岹���У�Ŀǰֻ֧����������ֱ�߲岹	 
void LineRuninRun_Motor(MotorIDEnum* motorList,s32* pulseList, u8 count,float runSpeed);

void PWMProc(MotorIDEnum motorID);	 
	 
#ifdef __cplusplus
}
#endif

#endif
