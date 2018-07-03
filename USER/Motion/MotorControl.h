#ifndef __MOTORCONTROL_H
#define __MOTORCONTROL_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "gpio.h"

typedef enum
{
	ZFX = 0, 	//方向1,正方向
	FFX = !ZFX	//方向2,反方向
} DirectEnum;//方向枚举

typedef enum
{
	LEFT_LIMIT = 0, 
	RIGHT_LIMIT, 
	LEFT_RIGHT_LIMIT,
	NO_LIMIT,
} LimitModeEnum;//限位模式，应该要放到轴类中

typedef enum
{
	NormalRun,		//正常模式
	PTRun,			//点动运行模式
}RunModeEnum;//电机运行模式

typedef enum
{
	SLOW = 0, 	//慢
	MEDIUM,   	//中
	FAST,		//快
} PTSpeedTypeEnum;//点动速度类型枚举

//typedef struct
//{
//	float Acc, //点动加
//}PTSpeedStruct;

//电机点动运行的参数
typedef struct
{
	PTSpeedTypeEnum PTSpeedType;//点动速度类型
	float ConSpeed;				//匀速速度(运行速度)
	float AccSpeed;				//加速度 
	float DecSpeed;				//减速度 
	u32 RunPulse;				//要走的脉冲数
} PTRunParaStruct;//点动运行所用参数结构体

typedef enum
{
	SingleMotor,		//单轴运行
	SameTimeGoto,		//同时到达
	Runin,				//插补运行
}MotorRunTypeEnum;//电机运行类型

typedef enum
{
	PrimaryMotor,	//主电机
	SecondMotor,	//副电机
}PriSecMotorEnum;//主副电机类型枚举

//电机结构体
typedef struct
{
	MotorIDEnum MotorID; 			//当前作用的电机端口编号
	DirectEnum Direct ;  			//当前运行方向
	RunModeEnum RunMode;			//运行模式	
	bool IsRunning;					//电机是否运行
	bool PTRunTag;					//点动运行标志
	bool StopTag;					//使电机停止的标志			 
	bool LimitOrigin;				//原点限位(左限位)的标志
	bool LimitMaxLeng;				//最大行程限位的标志
	float ConSpeed;					//匀速速度(运行速度)
	float AccSpeed;					//加速度 
	float DecSpeed;					//减速度 	
	u32 RunPulseNum;				//当前要运行的脉冲数,不要方向
	u32 RunPulseCounter;			//当前运行脉冲数计数器
	s32 RealPulse;					//实时位置对应的脉冲数，相对限位零点位置的脉冲数		
	s32 MaxSteps;					//最大行程步数	
	u32 TimPeriod;					//定时器预装值	
	MotorRunTypeEnum RunType;		//电机运行类型
	PriSecMotorEnum PriSecMotor;	//主从电机类型
}MotorControlStruct;//电机控制参数结构

//电机控制参数列表
extern MotorControlStruct MotorList[MotorPortNum];
//复位标志，解决点动过原点与系统复位有矛盾的问题
extern bool ResetFlag;

//电机初始化,对所有电机进行初始化
void MotorInit(void);
//原点光感限位中断处理函数
void OriginLimitFunc(MotorIDEnum motorID);
//最大行程限位判断并处理,可扩展为光感模式
void MaxLengLimitFunc(MotorIDEnum motorID);
//PWM中断调用
void TIM_PWM(MotorIDEnum motorID);
//由一个电机ID得到电机字符串
char* GetMotorStrFromID(MotorIDEnum motorID);
//由一个电机字符得到电机编号枚举
MotorIDEnum GetMotorIDFromStr(char* motorStr);
//一个电机运行前的准备工作
bool MotorRunIniState(MotorIDEnum motorID,s32 pulseNum);
//电机运行函数。motorID:要运行的电机号，pulseNum：要走的脉冲数，符号表示旋转方向
//runSpeed：匀速速度;accSpeed：加速度
void MotorRun_OutSpeed(MotorIDEnum motorID,s32 pulseNum,float runSpeed,float accSpeed);
//电机参数外的速度设置，多轴同时运行，提高效率
void MotorRun_OutSpeed_Multi(MotorIDEnum* motors,s32* pulseNumList,float* runSpeed,float* accSpeed,u8 count,bool isSynch);
//使指定的电机进入运行。motorID:要运行的电机号，pulseNum：要走的脉冲数，符号表示旋转方向
//运行速度为参数设置的速度及百分比
//可以多个电机同时运行，为了优化执行效率，2018.04.02
void MotorRun(MotorIDEnum* motors,s32* pulseNumList,u8 count,bool isSynch);
//停止指定电机
void MotorStop(MotorIDEnum motorID);

//点动参数设置，一般轴是长度，旋转轴时是角度
void SetPTRunPara(PTSpeedTypeEnum ptSpeedType);
//电机开始点动
void MotorPTRun(MotorIDEnum motorID,DirectEnum direct);
//电机停止点动
void MotorPTStop(MotorIDEnum motorID);

#ifdef __cplusplus
}
#endif

#endif
