#ifndef _CMD_H
#define _CMD_H

#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include <stm32f4xx.h>
#include "PublicFunction.h"
#include "string.h"
#include "AxisLinkage.h"
#include "Protocol.h"
#include "WorkControl.h"
	 
	 
//RDD相关命令的处理
void RDDProcess(void);
//WRT相关命令的处理
void WRTProcess(void);
//OUT相关命令的处理
void OUTProcess(void);   
	 
//设置程序当前状态。statusID：状态号，statusDescribe：状态描述文本
//要注意的是，由于汉字显示要用UTF-8,所以传入的文字信息一定要是UTF-8.否则显示不了
void SetCurrStatus(DeviceStatusEnum devStatus,char* statusDescribe);	 
	 
//控制器通过SRQ请求显示某一窗体
//void ShowForm_SRQ(char* formName);
//控制器通过SRQ请求显示USB功能窗体
void ShowUSBFuncForm(void);
//显示系统复位窗体,stopSource:指停止源是屏幕上急停还是机台上按钮急停
void ShowSysResetForm(StopSource stopSource);
	 
#ifdef __cplusplus
}
#endif

#endif
