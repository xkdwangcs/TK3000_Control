#ifndef __SCURVE_H
#define __SCURVE_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "gpio.h"
#include "MotorControl.h"
#include "CurveBase.h"

#define _accPerPoints  1000

//加速段预装值数据结构,为了节省内存
typedef struct
{
    u16 PerCount;
    u16 PerValue;
} AccPerValueStruct;

typedef struct
{
    CurveTypeEnum CurveType;
	//AccPerValueStruct AccPerList[1000];
    u16 AccPerCount;		//加速段结构体列表元素个数
    u16 ConPeriod;			//匀速预装值
    u32 AccPulseCount; 		//加速段脉冲数
    u32 ConPulseCount; 		//匀速段脉冲数
    u32 DecPulseCount; 		//减速段脉冲数
    u32 Acc_Con_Count; 		//加速段+匀速段脉冲数    
    u32 AccPulseIndex;
    u32 CurrAccTableRow;	//加速段已经查询到了第几行
    u32 CurrRowMaxIndex; 	//加速段当前查询到行的最大脉冲索引
	u32 CurrDecTableRow;	//减速段已经查询到了第几行
    u32 CurrRowMinIndex; 	//减速段当前查询到行的最小脉冲索引
	float LastS;
	float LastV;
	float VPerK;
} STableParaStruct;

extern AccPerValueStruct AccPerList[MotorPortNum][_accPerPoints]; //分配内存用

//根据脉冲数创建一个曲线参数表，压缩表
void CreateSTable(MotorIDEnum motorID, u32 pulseCount,float runSpee,float accSpeed);
//PWM定时器中断调用
void PWM_ST(MotorIDEnum motorID);


#ifdef __cplusplus
}
#endif

#endif

