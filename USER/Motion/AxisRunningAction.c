#include "AxisRunningAction.h"
#include "Parameter.h"

//创建一个单次事情
//axisRunningAction:要添加到的事情列表成员的指针， axisID:轴号,actionCoord：动作的坐标值，actionFunc:动作回调函数
void CreateSingAction(AxisRunningActionStruct* axisRunningAction,AxisIDEnum axisID,float actionCoord,MotorRunningActionHandler actionFunc)
{
	axisRunningAction->ActionType=SingleAction;
	axisRunningAction->ActionFunc=actionFunc;
	axisRunningAction->SingleActionPara.ActionCoord=actionCoord;
	axisRunningAction->SingleActionPara.ActionPulse=(s32)(actionCoord /Parameter.PulseK[AxisList[axisID].MotorID]);
	axisRunningAction->SingleActionPara.isCallAction=false;
}

//创建一个多次事情
//axisRunningAction:要添加到的事情列表成员的指针， axisID:轴号,
//startCoord:起始坐标，<0时为从轴起始坐标开始，endCoord:结束坐标，<0时同上, actionFunc:动作回调函数
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

////根据事件结构录入的坐标等参数计算出内部要用的参数
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