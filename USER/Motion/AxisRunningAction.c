#include "AxisRunningAction.h"
#include "Parameter.h"

//����һ����������
//axisRunningAction:Ҫ��ӵ��������б��Ա��ָ�룬 axisID:���,actionCoord������������ֵ��actionFunc:�����ص�����
void CreateSingAction(AxisRunningActionStruct* axisRunningAction,AxisIDEnum axisID,float actionCoord,MotorRunningActionHandler actionFunc)
{
	axisRunningAction->ActionType=SingleAction;
	axisRunningAction->ActionFunc=actionFunc;
	axisRunningAction->SingleActionPara.ActionCoord=actionCoord;
	axisRunningAction->SingleActionPara.ActionPulse=(s32)(actionCoord /Parameter.PulseK[AxisList[axisID].MotorID]);
	axisRunningAction->SingleActionPara.isCallAction=false;
}

//����һ���������
//axisRunningAction:Ҫ��ӵ��������б��Ա��ָ�룬 axisID:���,
//startCoord:��ʼ���꣬<0ʱΪ������ʼ���꿪ʼ��endCoord:�������꣬<0ʱͬ��, actionFunc:�����ص�����
void CreateMultiAction(AxisRunningActionStruct* axisRunningAction,AxisIDEnum axisID,float startCoord,float endCoord,MotorRunningActionHandler actionFunc)
{
	axisRunningAction->ActionType=MultiAction;
	axisRunningAction->ActionFunc=actionFunc;
	axisRunningAction->MultiActionPara.StartCoord=startCoord;
	axisRunningAction->MultiActionPara.EndCoord=endCoord;
	if(startCoord<0)
		axisRunningAction->MultiActionPara.StartPulse=-1;
	else
		axisRunningAction->MultiActionPara.StartPulse=(s32)(startCoord /Parameter.PulseK[AxisList[axisID].MotorID]);
	if(endCoord<0)
		axisRunningAction->MultiActionPara.EndPulse=-1;
	else
		axisRunningAction->MultiActionPara.EndPulse=(s32)(endCoord /Parameter.PulseK[AxisList[axisID].MotorID]);
}

////�����¼��ṹ¼�������Ȳ���������ڲ�Ҫ�õĲ���
//void CalceActionPara(AxisIDEnum motorID,AxisRunningActionStruct* axisRunningActions,u8 actionCount)
//{
//	for(u8 i=0;i<actionCount;i++)
//	{
//		AxisRunningActionStruct* oneAction=&axisRunningActions[i];
//		if(oneAction->ActionType==SingleAction)
//		{
//			oneAction->SingleActionPara.ActionPulse=(s32)(oneAction->SingleActionPara.ActionCoord /Parameter.PulseK[motorID]);
//			oneAction->SingleActionPara.isCallAction=false;
//		}
//		else if(oneAction->ActionType==MultiAction)
//		{
//			if(oneAction->MultiActionPara.StartCoord<0)
//				oneAction->MultiActionPara.StartPulse=-1;
//			else
//				oneAction->MultiActionPara.StartPulse=(s32)(oneAction->MultiActionPara.StartCoord /Parameter.PulseK[motorID]);
//			
//			if(oneAction->MultiActionPara.EndCoord<0)
//				oneAction->MultiActionPara.EndPulse=-1;
//			else
//				oneAction->MultiActionPara.EndPulse=(s32)(oneAction->MultiActionPara.EndCoord /Parameter.PulseK[motorID]);
//		}
//	}
//}