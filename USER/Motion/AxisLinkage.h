#ifndef __COORDINATE_H
#define __COORDINATE_H 

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "AxisControl.h"
#include "AxisRunningAction.h"

typedef struct
{
	float SafeHeight;						//����Ǵ�ֱ�ᣬ����Ҫָ����ȫ�߶�
	AxisRunningActionStruct* DownActions;	//�½�ʱҪ����Ķ����б�
	u8 DownActionCount;						//�½�ʱҪ����Ķ�������	
	AxisRunningActionStruct* UpActions;		//����ʱҪ����Ķ����б�
	u8 UpActionCount;						//����ʱҪ����Ķ�������
}VerticalAxisLinkStruct;//��ֱ����ز���

typedef struct
{
	AxisIDEnum AxisID;//���
	float DestCoord;//Ŀ������ֵ
	AxisRunningActionStruct* RunningActions;//Ҫ����Ķ����б�Ҫ�󰴶��������С����(��������ʱ)��Ӵ�С(�븺������)����
	u8 ActionCount;//������Ҫ����Ķ�������
	s32 OldPlusNum; //δ����ǰ��������,�ڲ����ã��ⲿ���ø�ֵ
	u32 PlusNum_OtherRun;//��ǰ��ﵽ���ٸ������������������С����ڰ�ȫ�ƶ�
	VerticalAxisLinkStruct VertAxisLink; //��ֱ����ز�����==NULLʱ��ʾΪ�Ǵ�ֱ��
}LinkageMotorStruct;//������������ṹ��

//������������ָ�룬xyz/xy2z��
typedef void (*ThreeLinkageHandler)(float xCoord,float y2Coord,float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount);

//��������ת������һ������������ͬ����
//MotorRunningActions:Ҫ����Ķ����б�Ҫ�󰴶��������С����(��������ʱ)��Ӵ�С(�븺������)����
//actionCount:��������
void MoveToCoordRunAction(AxisIDEnum motorID,float destCoord,AxisRunningActionStruct* motorRunningActions,u8 actionCount);
//��������ת������һ������������ͬ���ġ����ٶȲ���
//MotorRunningActions:Ҫ����Ķ����б�Ҫ�󰴶��������С����(��������ʱ)��Ӵ�С(�븺������)����
//actionCount:��������
void MoveToCoordRunActionPara(AxisIDEnum motorID,float destCoord,AxisRunningActionStruct* motorRunningActions,u8 actionCount,float acc ,float speed);

//���������ṹ�����е�һ����Ĳ���
void CreateLinkageOneAxis(LinkageMotorStruct* oneLinkage,AxisIDEnum axisID,float destCoord,
	AxisRunningActionStruct* actionList,u8 actionCount,VerticalAxisLinkStruct* vertAxisLink);
//�����������˺���������һ�������Ĺ켣���ڰ�ȫ�߶������ƶ������ᣩ
//����ֻ֧��Z�����˶����붯����������Ҫ�ӷ����־�����չ��ÿ�ᶼ�ܲ��붯��
void MultiAxisLinkage(LinkageMotorStruct* linkageMotors,u8 count);
//ֹͣ��ǰ������ָֹͣ�������е�������˳�
void StopAxisLinkageRun(LinkageMotorStruct* linkageMotors,u8 count);
//������������������
void MultiAxisLinkageNoAction(AxisIDEnum* motors,float* coords, u8 count);
//Z���ƶ���ָ���������,ͬ���ġ��˺�������ȥ��
void ZMoveToCoord(float zCoord);
//X��Z�������������˶��켣�������˹����߶ȣ���
void XZLinkageToCoord(float xCoord, float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount);
//X��Y��������(��ƽ̨)��һ��X/Y�����в����ж����Ĳ���
void XYLinkageToCoord(float xCoord, float yCoord);
//X��Y2��������(��ƽ̨)
void XY2LinkageToCoord(float xCoord, float yCoord);
//X��Y��Z��������(��ƽ̨)�����˶��켣�������˹����߶ȣ���
void XYZLinkageToCoord(float xCoord, float yCoord,float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount);
//X��Y2��Z��������(��ƽ̨)�����˶��켣�������˹����߶ȣ���
void XY2ZLinkageToCoord(float xCoord,float y2Coord,float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount);

#ifdef __cplusplus
}
#endif
#endif
