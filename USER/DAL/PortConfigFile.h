#ifndef __IOPORTFILE_H
#define __IOPORTFILE_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "gpio.h"
#include "AxisControl.h"
	 
typedef struct
{
	IOPortStruct* Outport; 	//�̶�����˿�ʵ��ָ�룬��gpio�ļ�������
	char PortStr[32];	 	//�˿����ơ�ע��
	u8 PortIndex;//�˿���������0��ʼ	
}OutPortListCFGStruct;//��������ò���	 

typedef struct
{	
	AxisIDEnum AxixID; 		//���
	MotorIDEnum MotorID;	//����˿ڱ��
}MotorCFGStruct;//����˿����ò���	 

typedef struct
{
	OutPortListCFGStruct PortCFG[FixPortNum];	//����˿�����
	MotorCFGStruct MotorCFG[MotorPortNum];		//����˿�����
	char ParaVer[7];				  //�����汾�������Զ���������������������ʱ��д        
}PortMotorCFGStruct;

extern char* _portFileName;//����˿ڼ�����˿������ļ���
extern PortMotorCFGStruct PortMotorCFG;

//��ʼ���˿������ļ�
void IniPortCFGFile(void);
//����˿������ļ�
void SavePortConfig(void);
//�ָ�����Ĭ�ϵĶ˿������ļ�
void WritDefaultPortCFG(void);
	 
#ifdef __cplusplus
}
#endif

#endif
