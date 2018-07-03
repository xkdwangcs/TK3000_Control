#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "gpio.h"
#include "Timer.h"
#include "exti.h"
#include "nvic.h"
#include "PublicFunction.h"
#include "COM_3.h"
#include "CMD.h"
#include "WorkControl.h"
#include "SST25V.h"
#include "Parameter.h"
#include "CoordFile.h"
#include "AxisLinkage.h"
#include "AxisControl.h"
#include "RTC_F4xx.h"
#include "MainForm.h"
//#include "RegisterForm.h"
#include "WIFI.h"
#include "usbh_usr.h" 
#include "usb_bsp.h"
#include "usbd_usr.h"
#include "PortConfigFile.h"
#include "Register.h"
#include "Timekeeper.h"
#include "MemoryAlloc.h"
#include "WorkDataFile.h"
#include "AppTasks.h"

//设备初始化
void DeviceInit(void);

#ifdef __cplusplus
}
#endif

#endif

