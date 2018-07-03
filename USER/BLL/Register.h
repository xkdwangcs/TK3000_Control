#ifndef __REGISTER_H
#define __REGISTER_H

#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include "stm32f4xx.h"
#include "stdbool.h"
#include "stdio.h"
#include "time.h"
#include "string.h"
#include "Handlers.h"
#include "DES.h"
#include "PublicFunction.h"
#include "DateTime.h"
#include "RTC_F4xx.h"
#include "DAL/SysParameter.h"
#include "Encrypt_XYQ.h"

//计算出机器码
char* CalcDeviceNumber(void);
//在设备上进行注册
bool OnRegister(char* regNum,OneStrParameterHandler showMSG);
//检查机器码是否匹配
bool CheckDeviceNum(void);
//启动程序时,检查注册码,如果通过检查返回true
bool CheckRegister(void);

#ifdef __cplusplus
}
#endif

#endif

