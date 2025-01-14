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

	 
typedef struct
{
    MultiAxisCoordStruct RealCoord; //实时坐标
    DataTimeStruct DataTime;        //控制器时间    
}LoopDataStruct;//循环读取的数据结构
	 
//RDD相关命令的处理
void RDDProcess(void);
//WRT相关命令的处理
void WRTProcess(void);
//OUT相关命令的处理
void OUTProcess(void);   
	 
//设置程序当前状态。statusID：状态号，statusDescribe：状态描述文本
//要注意的是，由于汉字显示要用UTF-8,所以传入的文字信息一定要是UTF-8.否则显示不了
void SetCurrStatus(DeviceStatusEnum devStatus,char* statusDescribe);	 
//主动上送设备状态
void DeviceStatusSRQ(DeviceStatusEnum devStatus,char* statusDescribe);
	 
#ifdef __cplusplus
}
#endif

#endif
