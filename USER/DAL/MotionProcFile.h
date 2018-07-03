#ifndef __MOVINGPROCFILE_H
#define __MOVINGPROCFILE_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "gpio.h"
#include "AxisLinkage.h"

#define MoveMaxCount 40 //һ����������������

typedef enum
{
	OutPortMotion=0,	//����ڶ���
	AxisMove,			//����ƶ���ָ������
	AxisReset,			//�����λ���ص�ԭ����ٻص���ʼ��
	NReset,				//N���Զ���λ
	WaitTakeWork,		//ǰ�ź�ȡʱ,�ȴ�ȡ��
	DelayMotion,		//������ʱ
	AxisOrigin,			//�������ʼ��
	AxisParaCoord,		//���ƶ��������趨������
	WaitInport,			//�ȴ�����˿��ź�
	InportEvent,		//�����¼�
}MotionTypeEnum;//��������
	 
typedef struct
{
	u8 PortIndex;		//�˿���������0��ʼ
	bool IsHV;			//�����״̬��true:ʹ�ܶ˿�,һ��ָ�ߵ�ƽ��false:�رն˿�,һ��ָ�͵�ƽ
	float MoveDelay;	//������ʱ(s),����ʹ����ʱ���ر���ʱ	
}OutputMovePara;//����ڶ�������

typedef struct
{
	AxisIDEnum AxisList[AxisNum];	//����б�
	float DestCoord[AxisNum];		//Ŀ������
	u8 AxisCount;					//�����ĵ����
}MotorMovePara;//�����������

typedef struct
{
	AxisIDEnum AxisList[AxisNum];	//����б�
	u8 AxisCount;					//�������ĸ�λ����
}MotorResetPara;//��λ��������

typedef struct
{
	AxisIDEnum AxisList[AxisNum];	//����б�
	u8 AxisCount;					//������������
}MotorToStartPara;//���ƶ�����ʼ��

typedef struct
{
	AxisIDEnum AxisList[AxisNum];	//����б�
	u8 AxisCount;					//������������
}AxisToParaCoordStruct;//���ƶ��������е����õ�

typedef struct
{
	float DelayValue;//Ҫ��ʱ��ֵ(s)
}DelayParaStruct;//��ʱ�����ṹ��

typedef struct
{
	u8 PortIndex;		//�������������0��ʼ
	bool InputState;	//�����״̬���߻��ʱʹ�������
	float WaitDelay;	//�ȴ������״̬�����ʱ��
}WaitInportParaStruct;//�ȴ�����˿��źŽṹ��

typedef struct
{
	u8 PortIndex;		//�������������0��ʼ
	bool InputState;	//�����״̬���߻��ʱʹ�������
	char Msg[40];		//��ʾ����Ϣ
}InportEventStruct;		//�����¼��ṹ��
	 
typedef struct
{
	//�������͡�0������ڶ�����1������˶�,2:�����λ��3��N���Զ���λ��4:�ȴ�ȡ����5����ʱ��6���������ʼ�㣬7:���ƶ��������趨������
	MotionTypeEnum MotionType;
	OutputMovePara OutputPara;				//�����������
	MotorMovePara MotorPara;				//�����������
	MotorResetPara ResetPara;				//�����λ����
	DelayParaStruct DelayPara;  			//��ʱ����ʵ��
	MotorToStartPara MotorToStart;			//�������ʼ�����
	AxisToParaCoordStruct AxisToParaCoord;	//���ƶ��������е����õ�
	WaitInportParaStruct WaitInportPara;	//�ȴ�����˿��ź�
	InportEventStruct InportEvent;			//�����¼�		
}OneMoveStruct;//һ�������Ľṹ�� 

typedef struct
{
	char MoveName[50];				 		 			//�������ơ�����
	//�����汾�������Զ���������������������ʱ��д������������ÿ����Ŀ�п��ܲ�ͬ���汾�ŵ�λ�ò�ȷ�������Է���ǰ��  
	char ParaVer[7];									
	u16 WorkBeforMoveCount_Left;			 			//��ƽ̨��������˿֮ǰ�Ķ�������
	OneMoveStruct WorkBeforMoves_Left[MoveMaxCount];	//��ƽ̨��������˿֮ǰ�Ķ�����
	u16 WorkComplMoveCount_Left;			 			//��ƽ̨����˿��ɺ�Ķ�������
	OneMoveStruct WorkComplMoves_Left[MoveMaxCount];	//��ƽ̨����˿��ɺ�Ķ�����
	u16 WorkBeforMoveCount_Right;			 			//��ƽ̨��������˿֮ǰ�Ķ�������
	OneMoveStruct WorkBeforMoves_Right[MoveMaxCount];	//��ƽ̨��������˿֮ǰ�Ķ�����
	u16 WorkComplMoveCount_Right;			 			//��ƽ̨����˿��ɺ�Ķ�������
	OneMoveStruct WorkComplMoves_Right[MoveMaxCount];	//��ƽ̨����˿��ɺ�Ķ�����
	u16 ResetMoveCount;			 	 					//��λʱ�Ķ�����������λ���̲��÷�����ƽ̨
	OneMoveStruct ResetMoveMoves[MoveMaxCount];			//��λʱ�Ķ�����	 
	u16 InportEventCount;								//������¼�����
	OneMoveStruct InportEvents[InPortNum];				//������¼���
}MovesStruct;//һ�鶯���Ľṹ��

extern char* _moveFileName;
//��ǰ�������ļ��µ�����
extern MovesStruct Moves;

//��ʼ�����������ļ�
void IniMoveProcFile(void);
//�ָ�����Ĭ�ϵĶ�������
void WritDefaultMoveProc(void);
//��ȡ�����ļ�
void ReadMoveFile(void);
//���������ļ�
void SaveMoveFile(void);
//��ʾ�����ļ���ʼ����Ϣ����д��־
void ShowMotionIniMsg(void);
	 
#ifdef __cplusplus
}
#endif

#endif
