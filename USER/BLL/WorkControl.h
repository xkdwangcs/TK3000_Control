#ifndef __WORKCONTROL_H
#define __WORKCONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "CoordFile.h"
#include "Parameter.h"
#include "AxisLinkage.h"
#include "AxisControl.h"
#include "gpio.h"   
#include "MotionProc.h"
#include "Timekeeper.h"
	 
	 
typedef enum
{
	DeviceIniting,		//�豸��ʼ����
	TaskInitFail,		//�����ʼ��ʧ��
	RegWait,			//�ȴ�ע��
	DevReady,			//�豸׼������	
	DevWorking,			//�豸���ڹ�����	
	DevScram,			//��ͣ�У���ѯ����������
	USBInsert,			//USB�Ѳ��룬��ѯ����������	
}DeviceStatusEnum;	 
	 
typedef struct
{
	u16 CurrScrewIndex;	//��ǰ�ӹ�����˿��ţ���1��ʼ��
	u32 TotalTime;		//һ����������ʱ��
	u32 AvgTime;		//ƽ��ʱ��	
}ShowDataStruct;		//Ҫ��ʾ�������ϵ�����,����Ϊ�ṹ�壬�����Ժ���չ

typedef enum
{
	 Key,			//����
	 FJQD,			//�ż�����
}StartXinhaoSource;	//�����ź���Դ

typedef enum 
{
	KeyStop,	//������ͣ
	ScreenStop,	//��Ļ��ͣ
}StopSource;	//��ͣ�жϵ��ź���Դ

typedef enum 
{
	TaskNoRun,  			//����δ����״̬
	TaskRunning,			//��������״̬
	YReadyed,				//Y���ѽ���׼������״̬
}WorkTaskStateEnum;			//����˿�����״̬������һ����������������״̬��ʶ��Ҫ���������ܸı��ⲿ��ȫ���������

typedef struct 
{
	LeftRightPlatformEnum CurrPlatform;	//��ǰҪ������ƽ̨
	StartXinhaoSource StartSource;		//������Դ
	WorkTaskStateEnum WorkTaskState;	//�����״̬
}WorkTaskStartParaStruct;				//����˿������������Ϣ�ṹ

//�豸��ǰ״̬
extern DeviceStatusEnum _devCurrStatus;
//���������У�Ҫ��ʾ��������������
extern ShowDataStruct _showDatas;
//��ʼ����˿ʱ��
extern u32 _startWorkTime;
//˫ƽ̨ʱ����һƽ̨׼��
extern LeftRightPlatformEnum _yReadyPlat;

//�򿪵���
void OpenScrewDriver(void);
//��ʾ����˿ʱ��
void ShowWorkTime(void);
//��ʾ��Ϣ
void ShowWorkMsg_Plat(LeftRightPlatformEnum platform,char* msg);
//��ʾ������Ϣ
void ShowDebugMsg(char* msg);
//�жϸ���
void JudgeSuperficial(void);
//һ��ƽ̨���ڹ�������һƽ̨����׼��״̬ʱ��Y���ƶ�����һ������
void YReadyTask(void);
//һ��ƽ̨(�����)��ʼ����˿����ں���
void WorkRun(void *arg);
//�Զ���λ��N�θ�λ��
void AutoReset(void);
//������˿�󣬵ȴ�ȡ����Ҫ�����˹�����⡢ǰ�ź�ȡʱ������ 
void WaitGetWorkpiece(LeftRightPlatformEnum platform);
//����������صĶ���
void TestCoordMove(void);
//λ�ø�λ
void PositionReset(void);
//ϵͳ��λ
void SystemReset(void);
//�򿪱������������õĲ��������Ʊ����Ƽ�������
void OpenAlarm(void);
//�رձ���
void CloseAlarm(void);
//ϵͳ��λ���ᱣ�渴λǰ�����״̬
void SystemReset(void);


/***************����ʵ���ļ�����******************/
extern LinkageMotorStruct _linkages[3];
extern bool _isTorqueOK;//Ť���ź��Ƿ����յ�
extern AxisIDEnum _currYAxis;//��ǰ�����Ӧ��Y��

//��ʼ��ʱ
void StartJS(void);
//��������˿ʱ��
void CalcLockTime(void);
//Ť���źż��
void CheckTorquePort(void);
//���ϼ�⣬���ߡ�����
bool GZJC_FGFY(void);

#ifdef __cplusplus
}
#endif

#endif
