#ifndef __WORKCONTROLSUCK_H
#define __WORKCONTROLSUCK_H

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
void TestCoordMoveSuck(void);
//ȡ��˿���̺���
bool GetScrewSuck(void);
//����˿����
bool ClampScrewSuck(CoordStruct destCoord);
	 
#ifdef __cplusplus
}
#endif

#endif
