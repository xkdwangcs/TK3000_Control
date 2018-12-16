#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"

#define InPortNum 24  //����˿����� 
#define OutPortNum 16 //����˿�����   
#define FixPortNum 6  	//�ض�����˿ڵ���Ŀ	 	
#define MotorPortNum 6    	//Ӳ�����ܽ�������������

	//IO�����ýṹ��
	typedef struct
	{
		GPIO_TypeDef* Port; //GPIO
		u32 Pin; //Pin
		//NVIC_InitTypeDef* Nvic; //�ж����ȼ�����
	}IOPortStruct;

	typedef struct 
	{
		GPIO_TypeDef *PulsePort;	//��������˿ں�PORT
		u16 PulsePin;				//��������˿�����
		u16 PulsePinSource;
		u8 GPIO_AF;
		GPIO_TypeDef *DirectPort;	//�����������˿ں�
		u16 DirectPin;
//		GPIO_TypeDef *LimitPort; 	//��λ�ź�����˿ںţ�Ŀǰֻ������������λģʽ
//		u16 LimitPin;				//��λ�ź���������
		IOPortStruct LimitPort;		//��λ�ź�����˿ڣ�Ŀǰֻ������������λģʽ
		u16 Prescaler;				//��Ƶϵ��
		u32 TimCLK;					//��ʱ��ʱ��Ƶ��
	}MotorPortStruct;
	
	typedef enum
	{
		M1 = 0, 
		M2,
		M3,
		M4,
		M5, 
		M6,
	} MotorIDEnum;//����˿ڱ��ö�٣��ԾͰ����ϵĵ�������
	
	//���е�����˿ڼ���
	extern IOPortStruct OutPortList[OutPortNum];
	//����˿��б�
	extern MotorPortStruct MotorPortList[MotorPortNum];
	//�������ƶ˿�
	extern IOPortStruct ScrewDriverPort;
	//��յ�ŷ����ƶ˿�
	extern IOPortStruct VacuumValvePort;
	//����������˿�
	extern IOPortStruct AlarmSundPort;
	//����������˿�
	extern IOPortStruct AlarmLampPort;
	//Z�����׿��ƶ˿�
	extern IOPortStruct LockQGPort;
	//���Ͽ��ƶ˿�
	//extern IOPortStruct PushMaterPort;
	//���Ͽ��ƶ˿�
	extern IOPortStruct BlowMaterPort;
    
	//��λ��ť
	//extern IOPortStruct ResetPort;
	//��������ť
	extern IOPortStruct LeftStartPort;
	//��������ť
	extern IOPortStruct RightStartPort;
	//ֹͣ��ť
	extern IOPortStruct StopPort;
	//�ش�ť
	extern IOPortStruct RedoPort;

	//������׼�����ź�����ڣ�Ŀǰֻ������˿�õ���˿��׼���ź�
	extern IOPortStruct ScrewRedyPort;
	//����ź������/����˿ʱΪ����OK�ź�
	extern IOPortStruct ScrewInhalePort;
	//Ť���ź������
	extern IOPortStruct TorquePort;
	//��ƽ̨��������ź������
	extern IOPortStruct LeftWorkpiecePort;
	//��ƽ̨��������ź������
	extern IOPortStruct RightWorkpiecePort;

	//���롢����ڳ�ʼ��
	void GPIO_Config(void);
	//��ѯ�����״̬
	//OutPortList:����˿ڣ����أ�true:�ߵ�ƽ,false:�͵�ƽ
	bool GetOutPortState(IOPortStruct outPort);
	//��ѯ�����״̬
	//inPort:����˿ڣ����أ�true:�ߵ�ƽ,false:�͵�ƽ
	bool GetInPortState(IOPortStruct inPort);
	//���������״̬���Ƿ�ʹ��(�ߵ�ƽ)
	//OutPortList:����˿ڣ���0��ʼ��isEnabled:�Ƿ�Ϊ�ߵ�ƽ
	void SetOutPortState(IOPortStruct OutPortList,bool isEnabled);
	//���������״̬���Ƿ�ʹ��
	//portIndex:�˿���ţ���0��ʼ��isEnabled:�Ƿ�Ϊ�ߵ�ƽ
	void SetOutPortState_Index(u8 portIndex,bool isEnabled);
	//��ѯ���������״̬
	//portIndex:�˿���ţ���0��ʼ
	bool GetOutPortState_Index(u8 portIndex);
	//��ѯ����˿�״̬
	bool GetInPortState_Index(u8 portIndex);
	//����ָʾ����˸
	void LightAlarmPCB(u8 count);
    
    typedef struct
    {
        bool InportStatus[InPortNum];  //24·�����״̬
        bool OutportStatus[OutPortNum]; //16·�����״̬
    }IOStatusStruct; //IO��״̬�ṹ
    
    //��ȡ���������������״̬
    IOStatusStruct GetIOStatus();

#ifdef __cplusplus
}
#endif

#endif
