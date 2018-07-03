#ifndef __DATETIME_H
#define __DATETIME_H

#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include "stm32f4xx.h"
#include "DateTime.h"


//实时时钟初始化
u8 IniRTC(void);
//时间校准。要设置的新时间，按年到秒排列，6个元素
bool TimeAdjust(char* setDateTime);
//获取当前日期及时间,返回结构体形式
DataTimeStruct* GetCurrDataTime(void);
//获取当前日期的字符串，格式为：2016-07-23
char* GetCurrDateStr(void);
//获取当前时间的字符串，格式为：11:20:26
char* GetCurrTimeStr(void);
//获取当前日期及时间字符串
char* GetCurrDataTimeStr(void);

#ifdef __cplusplus
}
#endif

#endif
