#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"

#define InPortNum 24  //输入端口数量 
#define OutPortNum 16 //输出端口数量   
#define FixPortNum 6  	//特定输出端口的数目	 	
#define MotorPortNum 6    	//硬件所能接入的最大电机数量

	//IO口配置结构体
	typedef struct
	{
		GPIO_TypeDef* Port; //GPIO
		u32 Pin; //Pin
		//NVIC_InitTypeDef* Nvic; //中断优先级配置
	}IOPortStruct;

	typedef struct 
	{
		GPIO_TypeDef *PulsePort;	//脉冲输出端口号PORT
		u16 PulsePin;				//脉冲输出端口引脚
		u16 PulsePinSource;
		u8 GPIO_AF;
		GPIO_TypeDef *DirectPort;	//方向控制输出端口号
		u16 DirectPin;
//		GPIO_TypeDef *LimitPort; 	//限位信号输入端口号，目前只发现用了左限位模式
//		u16 LimitPin;				//限位信号输入引脚
		IOPortStruct LimitPort;		//限位信号输入端口，目前只发现用了左限位模式
		u16 Prescaler;				//分频系数
		u32 TimCLK;					//定时器时钟频率
	}MotorPortStruct;
	
	typedef enum
	{
		M1 = 0, 
		M2,
		M3,
		M4,
		M5, 
		M6,
	} MotorIDEnum;//电机端口编号枚举，对就板子上的电机输出口
	
	//所有的输出端口集合
	extern IOPortStruct OutPortList[OutPortNum];
	//电机端口列表
	extern MotorPortStruct MotorPortList[MotorPortNum];
	//电批控制端口
	extern IOPortStruct ScrewDriverPort;
	//真空电磁阀控制端口
	extern IOPortStruct VacuumValvePort;
	//报警声输出端口
	extern IOPortStruct AlarmSundPort;
	//报警灯输出端口
	extern IOPortStruct AlarmLampPort;
	//Z轴气缸控制端口
	extern IOPortStruct LockQGPort;
	//推料控制端口
	//extern IOPortStruct PushMaterPort;
	//吹料控制端口
	extern IOPortStruct BlowMaterPort;
    
	//复位按钮
	//extern IOPortStruct ResetPort;
	//左启动按钮
	extern IOPortStruct LeftStartPort;
	//右启动按钮
	extern IOPortStruct RightStartPort;
	//停止按钮
	extern IOPortStruct StopPort;
	//重打按钮
	extern IOPortStruct RedoPort;

	//供料器准备好信号输入口，目前只有吸螺丝用的螺丝盒准备信号
	extern IOPortStruct ScrewRedyPort;
	//真空信号输入口/吹螺丝时为吹料OK信号
	extern IOPortStruct ScrewInhalePort;
	//扭力信号输入口
	extern IOPortStruct TorquePort;
	//左平台工件检测信号输入口
	extern IOPortStruct LeftWorkpiecePort;
	//右平台工件检测信号输入口
	extern IOPortStruct RightWorkpiecePort;

	//输入、输出口初始化
	void GPIO_Config(void);
	//查询输出口状态
	//OutPortList:输出端口，返回：true:高电平,false:低电平
	bool GetOutPortState(IOPortStruct outPort);
	//查询输入口状态
	//inPort:输入端口，返回：true:高电平,false:低电平
	bool GetInPortState(IOPortStruct inPort);
	//设置输出口状态，是否使能(高电平)
	//OutPortList:输出端口，从0开始；isEnabled:是否为高电平
	void SetOutPortState(IOPortStruct OutPortList,bool isEnabled);
	//设置输出口状态，是否使能
	//portIndex:端口序号，从0开始；isEnabled:是否为高电平
	void SetOutPortState_Index(u8 portIndex,bool isEnabled);
	//查询其它输出口状态
	//portIndex:端口序号，从0开始
	bool GetOutPortState_Index(u8 portIndex);
	//查询输入端口状态
	bool GetInPortState_Index(u8 portIndex);
	//板载指示灯闪烁
	void LightAlarmPCB(u8 count);
    
    typedef struct
    {
        bool InportStatus[InPortNum];  //24路输入口状态
        bool OutportStatus[OutPortNum]; //16路输出口状态
    }IOStatusStruct; //IO口状态结构
    
    //读取控制器输入输出口状态
    IOStatusStruct GetIOStatus();

#ifdef __cplusplus
}
#endif

#endif
