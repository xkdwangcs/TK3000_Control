#ifndef __MOTORCONTROL_H
#define __MOTORCONTROL_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "gpio.h"

typedef enum
{
	ZFX = 0, 	//����1,������
	FFX = !ZFX	//����2,������
} DirectEnum;//����ö��

typedef enum
{
	LEFT_LIMIT = 0, 
	RIGHT_LIMIT, 
	LEFT_RIGHT_LIMIT,
	NO_LIMIT,
} LimitModeEnum;//��λģʽ��Ӧ��Ҫ�ŵ�������

typedef enum
{
	NormalRun,		//����ģʽ
	PTRun,			//�㶯����ģʽ
}RunModeEnum;//�������ģʽ

typedef enum
{
	SLOW = 0, 	//��
	MEDIUM,   	//��
	FAST,		//��
} PTSpeedTypeEnum;//�㶯�ٶ�����ö��

//typedef struct
//{
//	float Acc, //�㶯��
//}PTSpeedStruct;

//����㶯���еĲ���
typedef struct
{
	PTSpeedTypeEnum PTSpeedType;//�㶯�ٶ�����
	float ConSpeed;				//�����ٶ�(�����ٶ�)
	float AccSpeed;				//���ٶ� 
	float DecSpeed;				//���ٶ� 
	u32 RunPulse;				//Ҫ�ߵ�������
} PTRunParaStruct;//�㶯�������ò����ṹ��

typedef enum
{
	SingleMotor,		//��������
	SameTimeGoto,		//ͬʱ����
	Runin,				//�岹����
}MotorRunTypeEnum;//�����������

typedef enum
{
	PrimaryMotor,	//�����
	SecondMotor,	//�����
}PriSecMotorEnum;//�����������ö��

//����ṹ��
typedef struct
{
	MotorIDEnum MotorID; 			//��ǰ���õĵ���˿ڱ��
	DirectEnum Direct ;  			//��ǰ���з���
	RunModeEnum RunMode;			//����ģʽ	
	bool IsRunning;					//����Ƿ�����
	bool PTRunTag;					//�㶯���б�־
	bool StopTag;					//ʹ���ֹͣ�ı�־			 
	bool LimitOrigin;				//ԭ����λ(����λ)�ı�־
	bool LimitMaxLeng;				//����г���λ�ı�־
	float ConSpeed;					//�����ٶ�(�����ٶ�)
	float AccSpeed;					//���ٶ� 
	float DecSpeed;					//���ٶ� 	
	u32 RunPulseNum;				//��ǰҪ���е�������,��Ҫ����
	u32 RunPulseCounter;			//��ǰ����������������
	s32 RealPulse;					//ʵʱλ�ö�Ӧ���������������λ���λ�õ�������		
	s32 MaxSteps;					//����г̲���	
	u32 TimPeriod;					//��ʱ��Ԥװֵ	
	MotorRunTypeEnum RunType;		//�����������
	PriSecMotorEnum PriSecMotor;	//���ӵ������
}MotorControlStruct;//������Ʋ����ṹ

//������Ʋ����б�
extern MotorControlStruct MotorList[MotorPortNum];
//��λ��־������㶯��ԭ����ϵͳ��λ��ì�ܵ�����
extern bool ResetFlag;

//�����ʼ��,�����е�����г�ʼ��
void MotorInit(void);
//ԭ������λ�жϴ�����
void OriginLimitFunc(MotorIDEnum motorID);
//����г���λ�жϲ�����,����չΪ���ģʽ
void MaxLengLimitFunc(MotorIDEnum motorID);
//PWM�жϵ���
void TIM_PWM(MotorIDEnum motorID);
//��һ�����ID�õ�����ַ���
char* GetMotorStrFromID(MotorIDEnum motorID);
//��һ������ַ��õ�������ö��
MotorIDEnum GetMotorIDFromStr(char* motorStr);
//һ���������ǰ��׼������
bool MotorRunIniState(MotorIDEnum motorID,s32 pulseNum);
//������к�����motorID:Ҫ���еĵ���ţ�pulseNum��Ҫ�ߵ������������ű�ʾ��ת����
//runSpeed�������ٶ�;accSpeed�����ٶ�
void MotorRun_OutSpeed(MotorIDEnum motorID,s32 pulseNum,float runSpeed,float accSpeed);
//�����������ٶ����ã�����ͬʱ���У����Ч��
void MotorRun_OutSpeed_Multi(MotorIDEnum* motors,s32* pulseNumList,float* runSpeed,float* accSpeed,u8 count,bool isSynch);
//ʹָ���ĵ���������С�motorID:Ҫ���еĵ���ţ�pulseNum��Ҫ�ߵ������������ű�ʾ��ת����
//�����ٶ�Ϊ�������õ��ٶȼ��ٷֱ�
//���Զ�����ͬʱ���У�Ϊ���Ż�ִ��Ч�ʣ�2018.04.02
void MotorRun(MotorIDEnum* motors,s32* pulseNumList,u8 count,bool isSynch);
//ָֹͣ�����
void MotorStop(MotorIDEnum motorID);

//�㶯�������ã�һ�����ǳ��ȣ���ת��ʱ�ǽǶ�
void SetPTRunPara(PTSpeedTypeEnum ptSpeedType);
//�����ʼ�㶯
void MotorPTRun(MotorIDEnum motorID,DirectEnum direct);
//���ֹͣ�㶯
void MotorPTStop(MotorIDEnum motorID);

#ifdef __cplusplus
}
#endif

#endif
