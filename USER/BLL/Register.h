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

//�����������
char* CalcDeviceNumber(void);
//���豸�Ͻ���ע��
bool OnRegister(char* regNum,OneStrParameterHandler showMSG);
//���������Ƿ�ƥ��
bool CheckDeviceNum(void);
//��������ʱ,���ע����,���ͨ����鷵��true
bool CheckRegister(void);

#ifdef __cplusplus
}
#endif

#endif

