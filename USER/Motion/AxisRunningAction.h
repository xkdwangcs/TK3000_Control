#ifndef __AXISRUNNINGACTION_H
#define __AXISRUNNINGACTION_H 

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "AxisControl.h"
#include "stdbool.h"

//������й�����Ҫ�������麯�ĺ���ָ��
typedef void (*MotorRunningActionHandler)(void);

typedef enum
{
	SingleAction,//�������飬ֻ��ָ����һ��������ִ��һ��
	MultiAction,//Ҫ���ִ�е����飬��ָ���������϶��ִ��
}ActionTypeEnum;//��������

typedef struct
{
	float ActionCoord;						//Ҫ����������ֵ���ڴ�����ʱ���붯����ҪС������Ŀ������
	u32 ActionPulse;							//��ActionCoord����ó������������ⲿ���ø�ֵ��ֻ��Ϊ�˵��÷���
	bool isCallAction;						//Action�Ƿ��ѱ�����
}SingleActionParaStruct;					//��������ṹ

typedef struct
{
	float StartCoord;						//Ҫ��������ʼ����ֵ��С��0ʱ��ʾ���˶���ʼִ��
	float EndCoord;							//Ҫ�����Ľ�������ֵ��С��0ʱ��ʾִ�е���Ŀ������Ϊֹ
	s32 StartPulse;							//��StartCoord��Ӧ�ľ�������ֵ,�ڲ���
	s32 EndPulse;							//��EndCoord��Ӧ�ľ�������ֵ,�ڲ���
	//u32 PulseSpace;							//�������������ں���Χ�ڲ�ռ�õ��������Դ
}MultiActionParaStruct;						//�������ṹ

typedef struct
{
	ActionTypeEnum ActionType;				//��������
	MotorRunningActionHandler ActionFunc;	//�����������
	SingleActionParaStruct SingleActionPara;//�����������
	MultiActionParaStruct MultiActionPara;	//����������	
}AxisRunningActionStruct;					//�����й�����Ҫ���������ṹ

//����һ����������
//axisRunningAction:Ҫ��ӵ��������б��Ա��ָ�룬 axisID:���,actionCoord������������ֵ��actionFunc:�����ص�����
void CreateSingAction(AxisRunningActionStruct* axisRunningAction,AxisIDEnum axisID,float actionCoord,MotorRunningActionHandler actionFunc);
//����һ���������
//axisRunningAction:Ҫ��ӵ��������б��Ա��ָ�룬 axisID:���,
//startCoord:��ʼ���꣬<0ʱΪ������ʼ���꿪ʼ��endCoord:��������,<0ʱͬ��, actionFunc:�����ص�����
void CreateMultiAction(AxisRunningActionStruct* axisRunningAction,AxisIDEnum axisID,float startCoord,float endCoord,MotorRunningActionHandler actionFunc);

#ifdef __cplusplus
}
#endif
#endif
