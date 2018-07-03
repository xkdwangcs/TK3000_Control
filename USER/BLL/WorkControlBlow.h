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
	 
//测试坐标相关的动作
void TestCoordMoveBlow(void);
//取镙丝过程函数
bool GetScrewBlow(void);
//锁镙丝过程
bool ClampScrewBlow(CoordStruct destCoord);
	 
#ifdef __cplusplus
}
#endif

#endif
