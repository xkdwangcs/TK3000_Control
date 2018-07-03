/******************** (C) COPYRIGHT 2009 www.armjishu.com ************************
* File Name          : date.c
* Author             : www.armjishu.com Team
* Version            : V1.0
* Date               : 12/1/2009
* Description        : 日期相关函数
*******************************************************************************/
#include "RTC_F4xx.h"
#include "stm32f4xx.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Public/Delay.h"

NVIC_InitTypeDef   NVIC_InitStructure;
RTC_TimeTypeDef RTC_Time;
RTC_DateTypeDef RTC_Date;
DataTimeStruct _currDateTime;

//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec)
{
	RTC_Time.RTC_Hours=hour;
	RTC_Time.RTC_Minutes=min;
	RTC_Time.RTC_Seconds=sec;
	if(hour<=12)
		RTC_Time.RTC_H12=RTC_H12_AM;
	else
		RTC_Time.RTC_H12=RTC_H12_PM;	
	return RTC_SetTime(RTC_Format_BIN,&RTC_Time);
	
}
//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date)
{	
	RTC_DateTypeDef RTC_Time;
	RTC_Time.RTC_Date=date;
	RTC_Time.RTC_Month=month;
	RTC_Time.RTC_WeekDay=3;//星期数也要设置，否则年份会出错。但好像可以随意设置不会影响年份
	RTC_Time.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_Time);
}

//实时时钟初始化
u8 IniRTC(void)
{	
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问 
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//是否第一次配置?
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE 开启    
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
		{
			retry++;
			Delay_ms(10);
		}
		if(retry==0)
			return 1;		//LSE 开启失败. 
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟 

		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数(0~7FFF)
		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式
		RTC_Init(&RTC_InitStructure);
 
		RTC_Set_Time(6,0,0);	//设置时间
		RTC_Set_Date(16,6,6);	//设置日期
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
	}  
	return 0;
}

//获取当前日期及时间,返回结构体形式
DataTimeStruct* GetCurrDataTime(void)
{
	RTC_GetTime(RTC_Format_BIN,&RTC_Time);
	RTC_GetDate(RTC_Format_BIN, &RTC_Date);
	_currDateTime.Year=RTC_Date.RTC_Year;
	_currDateTime.Mon=RTC_Date.RTC_Month;
	_currDateTime.Day=RTC_Date.RTC_Date;
	_currDateTime.WDay=RTC_Date.RTC_WeekDay;
	_currDateTime.Hour=RTC_Time.RTC_Hours;
	_currDateTime.Min=RTC_Time.RTC_Minutes;
	_currDateTime.Sec=RTC_Time.RTC_Seconds;
	return &_currDateTime;
}

char _currDataTimeStr[40];
//获取当前日期及时间字符串
char* GetCurrDataTimeStr()
{
	GetCurrDataTime();
    memset(_currDataTimeStr,0,sizeof(_currDataTimeStr));
	sprintf(_currDataTimeStr,"20%02d-%02d-%02d %0.2d:%0.2d:%0.2d", RTC_Date.RTC_Year, RTC_Date.RTC_Month, 
		RTC_Date.RTC_Date, RTC_Time.RTC_Hours, RTC_Time.RTC_Minutes, RTC_Time.RTC_Seconds);
	return _currDataTimeStr;
}

//获取当前日期的字符串，格式为：2016-07-23
char* GetCurrDateStr(void)
{
    GetCurrDataTime();
    memset(_currDataTimeStr,0,sizeof(_currDataTimeStr));
	sprintf(_currDataTimeStr,"20%02d-%02d-%02d", RTC_Date.RTC_Year, RTC_Date.RTC_Month,RTC_Date.RTC_Date);
	return _currDataTimeStr;
}

//获取当前时间的字符串，格式为：11:20:26
char* GetCurrTimeStr(void)
{
    GetCurrDataTime();
    memset(_currDataTimeStr,0,sizeof(_currDataTimeStr));
	sprintf(_currDataTimeStr,"%0.2d:%0.2d:%0.2d", RTC_Time.RTC_Hours, RTC_Time.RTC_Minutes, RTC_Time.RTC_Seconds);
	return _currDataTimeStr;
}

//时间校准。要设置的新时间，按年到秒排列，6个元素
bool TimeAdjust(char* setDateTime)
{
	DataTimeStruct dts;
	ConvertToDateTime(setDateTime,&dts);
	dts.Year-=2000;
	if(dts.Year<16 || dts.Year>50)
		return false;
	if(dts.Mon<1 || dts.Mon>12)
		return false;
	if(dts.Day<1 || dts.Mon>31)
		return false;
	if(dts.Hour<0 || dts.Hour>24)
		return false;
	if(dts.Min<0 || dts.Min>60)
		return false;
	if(dts.Sec<0 || dts.Sec>60)
		return false;
	RTC_Set_Time(dts.Hour,dts.Min,dts.Sec);	//设置时间
	RTC_Set_Date(dts.Year,dts.Mon,dts.Day);		//设置日期
	return true;
}
