#ifndef _DATETIME_H_
#define _DATETIME_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include <stdint.h>
#include "PublicFunction.h"
//#include "time.h"
	
typedef struct 
{
	u8 Sec;
	u8 Min;
	u8 Hour;
	u8 Day;
	u8 Mon;
	u16 Year;
	u8 WDay;//星期
}DataTimeStruct;//日期时间结构体    
	
typedef enum
{
	Equal=0, //相等
	Greater, //大于
	Less,    //小于
}CMPValueEnum;//两数比较值的枚举    

//比较两个日期，可以包括时间（也可以没有），要求两个日期的格式是一致的
CMPValueEnum DateTimeCMP(char* dateTime1,char* dateTime2);
//把形式如"20160802"的格式转换为2016-08-02
char* DateFormat(char* dateStr);
//从字符串分解出日期,不分解时间,格式:2016-08-11
void ConvertToDate(char* dateStr,DataTimeStruct* dateTime);
//从字符串分解出日期及时间，格式:2016-08-11 21:13:15
void ConvertToDateTime(char* dateTimeStr,DataTimeStruct* dateTime);
//求两个日期的相差天数，date1-date2,有正负
int CalcDateDiff_Struct(DataTimeStruct date1,DataTimeStruct date2);
//求两个日期的相差天数，date1-date2,有正负
int CalcDateDiff_String(char* date1,char* date2);
//指定一个日期，加上天数。返回新的日期
char* DateAddDay(char* oldDate,int days);
    
#ifdef __cplusplus
}
#endif

#endif 
