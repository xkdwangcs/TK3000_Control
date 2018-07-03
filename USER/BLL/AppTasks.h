#ifndef _APPTASKS_H
#define _APPTASKS_H

#include "stm32f4xx.h"
#include "WorkControl.h" 
#include "AxisLinkage.h" 
#include "stdbool.h"
// FreeRTOS head file, add here.
#include "FreeRTOS.h"
#include "task.h"//�������ͷ�ļ�֮ǰһ��Ҫ����FreeRTOS.h
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"  

#ifdef __cplusplus
 extern "C" {
#endif 

extern xTaskHandle _workRunLeftHandle;      //��ƽ̨����˿���
extern xTaskHandle _workRunRightHandle;     //��ƽ̨����˿���
extern SemaphoreHandle_t _xSemaphoreRedo;    //�ش��ź���
extern SemaphoreHandle_t _xSemaphoreNext;    //��һ���ź���   
extern bool _isWaitSemaphore;               //�Ƿ��ѽ�����ϵȴ�״̬
//һ��ִ��FATFS������صĺ������
extern EventHandlerNoPara _oneFatfsOperateFunc;
	 
//����ϵͳ����ʱ��ms
void Delay_ms_OS(u16 msValue);
//����ϵͳ����ʱ��s
void Delay_s_OS(float sValue);
//�����ʼ��
void InitTask(void *arg);
//��ʼ������������
void StartTestCoordMove(void);
//��ͣ
void ScramStop(StopSource stopSource);  
//�򿪻�ر��豸��Ϊ����U��
void SetUSBMassStorage(bool isOpen);
//��ʼλ�ø�λ����
void StartPositionReset(void);   
//USBHost����رջ���
void SetUSBHostTaskEnable(bool isEnable);
//��ʼFATFS�������������
void StartFATFSTask(void);
//��������˸ʹ��
void EnableAlarmTwinkle(bool isEnable);
//ʵʱ������ʾ����رջ���
void SetRealCoordTaskEnable(bool isEnable);
//IO״̬��ʾ����رջ���
void SetIOStateTaskEnable(bool isEnable);
//ʱ����ʾ����رջ���
void SetDateTimeTaskEnable(bool isEnable);
//˫ƽ̨ʱ�����ڹ�����ƽ̨����������ô˺�����ʹ��һ��ƽ̨����׼��״̬ƽ̨������������
//platformComplate:���ô˺���������ƽ̨�����磺LeftPlat��ʾ��ƽ̨�����(����ǰ�ź�ȡʱ�ĵȴ�ȡ��)����ƽ̨���Կ�ʼ������
void ContinueNextPlatWork(LeftRightPlatformEnum platformComplate);
//��ʼ����˿����
void StartWorkTask(LeftRightPlatformEnum platform,StartXinhaoSource sxs);
//���Ϻ��ش�����
void RepeatWork(void);
//������ڹ����С����ϵȴ��У������λ�з���true�����򷵻�false
bool IsWorkRunning(void);

#ifdef __cplusplus
}
#endif

#endif
