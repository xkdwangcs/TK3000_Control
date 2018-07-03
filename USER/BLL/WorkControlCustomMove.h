#ifndef __WORKCONTROLCUSTOM_H
#define __WORKCONTROLCUSTOM_H

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
	 

//���пͻ��Զ���Ķ���
void RunCustomMove(void* arg);
	 
#ifdef __cplusplus
}
#endif

#endif
