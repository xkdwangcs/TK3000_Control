#ifndef _WORKDATAFILE_H_
#define _WORKDATAFILE_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "FileOperate.h"
#include "Public/PublicFunction.h"
    
                
    //Ҫͳ�Ƶ������Ŀ
	typedef struct
	{
		u32 totalNumber;				//�ܼӹ�����
		u32 qualifiedNumber;			//�ϸ�����
		u32 unQualifiedNumber;			//���ϸ�����
	} StatisticsStruct;

    typedef struct
	{
       	StatisticsStruct DayStatistics;		//��ͳ������
		StatisticsStruct TotalStatistics;	//��ͳ������
		u8 DevieIniFailNum; 				//�豸��ʼ��ʧ�ܴ���
		u8 DeviceIniBadTag; 				//�ļ�ϵͳ�𻵵ı�־
		char LastOnPowerDate[20];           //���һ�ο���ʱ�䣬�����ж��豸ʱ���Ƿ���ȷ���Ƿ�ע�����;
		char ParaVer[7];					//�����汾�������Զ���������������������ʱ��д  
    }WorkDataStruct;//�������ݽṹ��
    
//����ʵ��
extern WorkDataStruct WorkData;
//�Ӵ洢���ж�ȡ��ع�������
void IniWorkData(void);
//�ѹ�������д��洢��
void WriteWorkData(void);
	
/**************************/	

    
#ifdef __cplusplus
}
#endif

#endif 
