#ifndef __IOPORTFILE_H
#define __IOPORTFILE_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "gpio.h"
#include "AxisControl.h"
	 
typedef struct
{
	IOPortStruct* Outport; 	//固定输出端口实例指针，从gpio文件中引出
	char PortStr[32];	 	//端口名称、注释
	u8 PortIndex;//端口索引，从0开始	
}OutPortListCFGStruct;//输出口配置参数	 

typedef struct
{	
	AxisIDEnum AxixID; 		//轴号
	MotorIDEnum MotorID;	//电机端口编号
}MotorCFGStruct;//电机端口配置参数	 

typedef struct
{
	OutPortListCFGStruct PortCFG[FixPortNum];	//输出端口配置
	MotorCFGStruct MotorCFG[MotorPortNum];		//电机端口配置
	char ParaVer[7];				  //参数版本，用于自动升级参数扇区或当扇区损坏时重写        
}PortMotorCFGStruct;

extern char* _portFileName;//输出端口及电机端口配置文件名
extern PortMotorCFGStruct PortMotorCFG;

//初始化端口配置文件
void IniPortCFGFile(void);
//保存端口配置文件
void SavePortConfig(void);
//恢复出厂默认的端口配置文件
void WritDefaultPortCFG(void);
	 
#ifdef __cplusplus
}
#endif

#endif
