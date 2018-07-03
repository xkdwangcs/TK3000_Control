#ifndef __WORKCONTROLBLOW_H
#define __WORKCONTROLBLOW_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "CoordFile.h"
#include "Parameter.h"
#include "AxisLinkage.h"
#include "MotorControl.h"
#include "gpio.h"   
#include "MotionProc.h"
#include "Timekeeper.h"	 
	 
//����������صĶ���
void TestCoordMoveBlow(void);
//ȡ��˿���̺���
bool GetScrewBlow(void);
//����˿����
bool ClampScrewBlow(CoordStruct destCoord);
	 
#ifdef __cplusplus
}
#endif

#endif
