
#ifndef _LOG_H_
#define _LOG_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "RTC_F4xx.h"
#include "PublicFunction.h"
#include "FileOperate.h"

//获取日志文件列表,逆序排列:最近的文件排在前面
u8 GetLogFileList(void);	
//读取多行文本，行尾用"\r\n"结尾，返回一次读取到的行数。rowStart:当前/下一行开始的地址
u8 ReadLogFileMultiRow(char* logFileName,u32* rowStart);
//写一行日志,由用户程序调用
void WriteLog(char* oneLineLog);
//手动把缓存中的数据写入文件。指日志条数小于指定大小时手动写入
void WriteLogBuffToFile(void);
//删除所有的日志文件
void DeleteAllLogFile(void);

//测试创建、写入日志文件
void TestLogFile(char* dataStr,char* msgStr);	
    
#ifdef __cplusplus
}
#endif

#endif 
