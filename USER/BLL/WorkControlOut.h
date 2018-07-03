#ifndef __WORKCONTROLOUT_H
#define __WORKCONTROLOUT_H

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
void TestCoordMoveOut(void);
//ȡ��˿���̺���
bool GetScrewOut(void);
//����˿����
bool ClampScrewOut(CoordStruct destCoord);
	 
#ifdef __cplusplus
}
#endif

#endif
