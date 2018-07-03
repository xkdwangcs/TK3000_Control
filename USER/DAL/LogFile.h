
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

//��ȡ��־�ļ��б�,��������:������ļ�����ǰ��
u8 GetLogFileList(void);	
//��ȡ�����ı�����β��"\r\n"��β������һ�ζ�ȡ����������rowStart:��ǰ/��һ�п�ʼ�ĵ�ַ
u8 ReadLogFileMultiRow(char* logFileName,u32* rowStart);
//дһ����־,���û��������
void WriteLog(char* oneLineLog);
//�ֶ��ѻ����е�����д���ļ���ָ��־����С��ָ����Сʱ�ֶ�д��
void WriteLogBuffToFile(void);
//ɾ�����е���־�ļ�
void DeleteAllLogFile(void);

//���Դ�����д����־�ļ�
void TestLogFile(char* dataStr,char* msgStr);	
    
#ifdef __cplusplus
}
#endif

#endif 
