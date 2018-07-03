#ifndef __SCURVE_H
#define __SCURVE_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "gpio.h"
#include "MotorControl.h"
#include "CurveBase.h"

#define _accPerPoints  1000

//���ٶ�Ԥװֵ���ݽṹ,Ϊ�˽�ʡ�ڴ�
typedef struct
{
    u16 PerCount;
    u16 PerValue;
} AccPerValueStruct;

typedef struct
{
    CurveTypeEnum CurveType;
	//AccPerValueStruct AccPerList[1000];
    u16 AccPerCount;		//���ٶνṹ���б�Ԫ�ظ���
    u16 ConPeriod;			//����Ԥװֵ
    u32 AccPulseCount; 		//���ٶ�������
    u32 ConPulseCount; 		//���ٶ�������
    u32 DecPulseCount; 		//���ٶ�������
    u32 Acc_Con_Count; 		//���ٶ�+���ٶ�������    
    u32 AccPulseIndex;
    u32 CurrAccTableRow;	//���ٶ��Ѿ���ѯ���˵ڼ���
    u32 CurrRowMaxIndex; 	//���ٶε�ǰ��ѯ���е������������
	u32 CurrDecTableRow;	//���ٶ��Ѿ���ѯ���˵ڼ���
    u32 CurrRowMinIndex; 	//���ٶε�ǰ��ѯ���е���С��������
	float LastS;
	float LastV;
	float VPerK;
} STableParaStruct;

extern AccPerValueStruct AccPerList[MotorPortNum][_accPerPoints]; //�����ڴ���

//��������������һ�����߲�����ѹ����
void CreateSTable(MotorIDEnum motorID, u32 pulseCount,float runSpee,float accSpeed);
//PWM��ʱ���жϵ���
void PWM_ST(MotorIDEnum motorID);


#ifdef __cplusplus
}
#endif

#endif

