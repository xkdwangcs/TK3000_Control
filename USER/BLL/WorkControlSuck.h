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
	
//测试坐标相关的动作
void TestCoordMoveSuck(void);
//取镙丝过程函数
bool GetScrewSuck(void);
//锁镙丝过程
bool ClampScrewSuck(CoordStruct destCoord);
	 
#ifdef __cplusplus
}
#endif

#endif
