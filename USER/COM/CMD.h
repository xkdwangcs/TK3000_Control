#ifndef _CMD_H
#define _CMD_H

#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include <stm32f4xx.h>
#include "PublicFunction.h"
#include "string.h"
#include "AxisLinkage.h"
#include "Protocol.h"
#include "WorkControl.h"
	 
typedef struct
{
    MultiAxisCoordStruct RealCoord; //ʵʱ����
    DataTimeStruct DataTime;        //������ʱ��    
}LoopDataStruct;//ѭ����ȡ�����ݽṹ
	 
//RDD�������Ĵ���
void RDDProcess(void);
//WRT�������Ĵ���
void WRTProcess(void);
//OUT�������Ĵ���
void OUTProcess(void);   
	 
//���ó���ǰ״̬��statusID��״̬�ţ�statusDescribe��״̬�����ı�
//Ҫע����ǣ����ں�����ʾҪ��UTF-8,���Դ����������Ϣһ��Ҫ��UTF-8.������ʾ����
void SetCurrStatus(DeviceStatusEnum devStatus,char* statusDescribe);	 
//���������豸״̬
void DeviceStatusSRQ(DeviceStatusEnum devStatus,char* statusDescribe);
	 
#ifdef __cplusplus
}
#endif

#endif
