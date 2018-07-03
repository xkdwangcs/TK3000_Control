#ifndef __CURVEBASE_H
#define __CURVEBASE_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "gpio.h"
#include "MotorControl.h"

typedef enum
{
	//5��S�Ӽ��٣�ѹ�����
	SCurve_Five_Table,
	//5��S�Ӽ���,ʵʱ����
	SCurve_Five_Real,
	//5��S�Ӽ��٣���ѹ�����
	SCurve_Five_Table_NOZIP,
	//T�μӼ���
	TCurve,
}AccDecCurveTypeEnum;

typedef enum
{
    Acc_Con_Dec, 	//�мӡ��ȡ�������
    Acc_Dec,	 	//ֻ�мӡ�����,����������ʱ����֡���������֣��ٶȿ��ܻ����һ�㣬�����Ա������
    //Acc,			//ֻ�м��ٶΣ���ʱ���Ǽ��ٶ����ò��������⣬Ϊ�˽��ٹ�������󣬲����ִ������
    Con,			//ֻ�����ٶΣ������ٶȺ�С������������ʱ����
} CurveTypeEnum; 	//��������

typedef struct
{
    CurveTypeEnum CurveType;
    u32 AccStartIndex;
    u32 AccEndIndex;
    u32 ConStartIndex;
    u32 ConEndIndex;
    u32 DecStartIndex;
    u32 DecEndIndex;
} CurveParaStruct;

//��ʼ�ٶȡ���ֹ�ٶ�
extern float _vs;

void IniPerToCOM(MotorIDEnum motorID);
//���Ԥװֵ�����棬��������������ͷ��͵�����
void AddSendPer(MotorIDEnum motorID,u16 per);
//����ʣ���Ԥװֵ������
void SendSYPer();

//��������������һ�����߲�����
void CreateCurve(MotorIDEnum motorID, u32 pulseCount,float runSpeed,float accSpeed);

//PWM��ʱ���жϵ���
void PWM(MotorIDEnum motorID);

//���ֹͣ���ڲ�����
void StopMotor_in(MotorIDEnum motorID);

#ifdef __cplusplus
}
#endif

#endif