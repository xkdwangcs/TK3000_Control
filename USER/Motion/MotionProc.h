#ifndef __MOTIONFILE_H
#define __MOTIONFILE_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "gpio.h"
#include "WorkControl.h"
#include "AxisLinkage.h"
#include "MotionProcFile.h"	 

//分析一个字符串，转换成一个动作结构体。字符串如果有错误返回空
OneMoveStruct* AnalyStrToMoveStruct(char* moveStr,OneStrParameterHandler showErrMsgHandel);
//根据一个动作结构生成一条解释性字符串
char* GetOneMoveStr(OneMoveStruct oms);
//执行一组动作
void RunOneGroupWork(OneMoveStruct* moves,u16 moveCount,LeftRightPlatformEnum platform,OneStrParameterHandler showMSG);
//左平台打螺丝前的动作流程
void WorkBeforMove_Left(void);
//左平台工件完成后的动作流程
void WorkCompletedMove_Left(void);
//右平台打螺丝前的动作流程
void WorkBeforMove_Right(void);
//右平台工件完成后的动作流程
void WorkCompletedMove_Right(void);
//复位动作流程
void PositionResetMove(void);
	 
#ifdef __cplusplus
}
#endif

#endif

