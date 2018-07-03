#ifndef _USBOPERATE_H
#define _USBOPERATE_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "Parameter.h"
#include "MainForm.h"
//#include "SysInfoUpdate.h"
	
typedef enum
{
	NoUSBOP,          //��USB����
	ExportCoordFile,  //���豸���������ļ���U��
	ImportCoordFile,  //��U�̸��������ļ����豸
  ExportLogFile,    //������־�ļ���U��
	ExportParaFile,   //���������ļ� 
	ImportParaFile,   //��������ļ� 
	ImportMPPara,     //�������Ʋ���
	ExportPortFile,   //�����˿��ļ�
	ImportPortFile,   //����˿��ļ�
}USBOperateEnum;    //USB����ز���ö�٣���������

//��ʾUSB��������
void ShowUSBFuncForm(void);
//USB��ز�����������
void USBOpereateTask(void);
	
#ifdef __cplusplus
}
#endif

#endif
