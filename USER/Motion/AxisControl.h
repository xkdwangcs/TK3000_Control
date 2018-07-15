#ifndef __AXISCONTROL_H
#define __AXISCONTROL_H

#include "MotorControl.h"

#define AxisNum 	4  //����Ŀ�����õ����������

typedef enum
{
	X = 0, 
	Y1,
	Y2,
	Z,	
} AxisIDEnum;//��ö��

typedef struct
{
	float X;
	float Y;
	float Z;
} CoordStruct;//����ṹ

typedef struct
{
	float X1;
    float X2;
	float Y1;//��ƽ̨Y����
	float Y2;//��ƽ̨Y����
	float Z1;
    float Z2;
}MultiAxisCoordStruct;//�������꣬��Ҫ����ʵʱ����

typedef enum
{
	PlaneAxis,   //ƽ���ᣬ
	VerticalAxis,//��ֱ��
	RevolveAxis, //��ת��	
}AxisTypeEnum;//������ö��

typedef struct
{
	AxisIDEnum AxisID; 		//��ǰ���õĵ���˿ڱ��
	MotorIDEnum MotorID;	//�����Ӧ�ĵ�����
	AxisTypeEnum AxisType;	//������	
}AxisControlStruct;//����Ʋ����ṹ

//����Ʋ����б�
extern AxisControlStruct AxisList[AxisNum];
//��ǰ����
extern MultiAxisCoordStruct _currCoord;
extern MotorIDEnum _xMotor;
extern MotorIDEnum _y1Motor;
extern MotorIDEnum _y2Motor;
extern MotorIDEnum _zMotor;

//���ʼ��,�Ա���Ŀ���õ�ȫ������г�ʼ��
void AxisInit(void);
//ȡ��ʵʱ����
MultiAxisCoordStruct* GetRealTimeCoord(void);
//��һ����ŵõ����ַ���
char* GetAxisStrFromID(AxisIDEnum axisID);
//�����ַ����õ����
AxisIDEnum GetAxisIDFormStr(char* axisStr);
//��ȡָ���������״̬��true:�������У�false:ֹͣ
//�滻ԭ����Motor.IsRunning��Ա
bool GetAxisRunState(AxisIDEnum axisID);
//��ȡָ����ĵ�ǰ����ֵ�����������ֵ
s32 GetAxisRealPulse(AxisIDEnum axisID);
//����ͬʱ���У��Ż�������Ч�ʡ�2018.04.02
void MoveToCoord_Multi(AxisIDEnum* axisList,float* destCoordList,u8 count ,bool isSynch);
//ָ������ƶ���Ŀ�����꣬û�в��붯������Ϊ�ƶ������깦�ܵĵײ㺯��
//isSynch:�Ƿ�ͬ����ָ�Ƿ�ȴ�����ƶ�����ٷ��أ�����Ϊ�첽
void MoveToCoord(AxisIDEnum motorID,float destCoord,bool isSynch);
//ָֹͣ������
void StopAxis(AxisIDEnum axisID);
//ֹͣƽ̨���е����˶�
void StopAllAxis(void);

//������㣬��ͬ���źţ����ƶ�������λ��
void MultiAxisMoveToPositionZeroAndMove(AxisIDEnum* id,u8 count);

//��㶯����
void AxisPTRun(AxisIDEnum axisID,DirectEnum direct);
//��㶯����
void AxisPTStop(AxisIDEnum axisID);

#ifdef __cplusplus
}
#endif

#endif
