#ifndef _USBOPERATE_H
#define _USBOPERATE_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "Parameter.h"
#include "MainForm.h"
//#include "SysInfoUpdate.h"
	
typedef enum
{
	NoUSBOP,          //无USB操作
	ExportCoordFile,  //从设备复制坐标文件到U盘
	ImportCoordFile,  //从U盘复制坐标文件到设备
  ExportLogFile,    //复制日志文件到U盘
	ExportParaFile,   //导出参数文件 
	ImportParaFile,   //导入参数文件 
	ImportMPPara,     //导入铭牌参数
	ExportPortFile,   //导出端口文件
	ImportPortFile,   //导入端口文件
}USBOperateEnum;    //USB的相关操作枚举，用于任务

//显示USB操作窗体
void ShowUSBFuncForm(void);
//USB相关操作的任务窗体
void USBOpereateTask(void);
	
#ifdef __cplusplus
}
#endif

#endif
