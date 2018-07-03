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

//����һ���ַ�����ת����һ�������ṹ�塣�ַ�������д��󷵻ؿ�
OneMoveStruct* AnalyStrToMoveStruct(char* moveStr,OneStrParameterHandler showErrMsgHandel);
//����һ�������ṹ����һ���������ַ���
char* GetOneMoveStr(OneMoveStruct oms);
//ִ��һ�鶯��
void RunOneGroupWork(OneMoveStruct* moves,u16 moveCount,LeftRightPlatformEnum platform,OneStrParameterHandler showMSG);
//��ƽ̨����˿ǰ�Ķ�������
void WorkBeforMove_Left(void);
//��ƽ̨������ɺ�Ķ�������
void WorkCompletedMove_Left(void);
//��ƽ̨����˿ǰ�Ķ�������
void WorkBeforMove_Right(void);
//��ƽ̨������ɺ�Ķ�������
void WorkCompletedMove_Right(void);
//��λ��������
void PositionResetMove(void);
	 
#ifdef __cplusplus
}
#endif

#endif

