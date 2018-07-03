#ifndef _MINFORMS_H
#define _MINFORMS_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "WorkControl.h"	 
	 
	 
//显示欢迎窗体
void ShowDateTimeSetForm(u16 keyCode);
//显示登陆窗体。showOkForm：登陆成功时显示的窗体句柄，showBackForm：返回按钮时的窗体句柄
void ShowLoginForm(PermissionTypeEnum destPer,PCLICK showOkForm,PCLICK showBackForm);
//无按钮的信息显示窗体
void ShowMessgeForm_NoButton(char* msg);
//有按钮的信息显示窗体
void ShowMessgeForm(char* msg,PCLICK okClickFunc,PCLICK backClickFunc);
//显示主窗体
void ShowMainForm();
 //显示工作过程中的信息
void ShowWorkMsg(char* msg);
//显示调试信息
void ShowDebugMsg(char* msg);
//显示加工过程中的相关数据
void ShowWorkData(void);
//显示实时坐标
void ShowCurrCoord(MultiAxisCoordStruct* realCoord);	 
	 
#ifdef __cplusplus
}
#endif

#endif
