#ifndef __CURVEBASE_H
#define __CURVEBASE_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "gpio.h"
#include "MotorControl.h"

typedef enum
{
	//5段S加减速，压缩查表
	SCurve_Five_Table,
	//5段S加减速,实时计算
	SCurve_Five_Real,
	//5段S加减速，不压缩查表
	SCurve_Five_Table_NOZIP,
	//T形加减速
	TCurve,
}AccDecCurveTypeEnum;

typedef enum
{
    Acc_Con_Dec, 	//有加、匀、减三段
    Acc_Dec,	 	//只有加、减速,当点数较少时会出现。把脉冲点半分，速度可能会减慢一点，但可以避免过冲
    //Acc,			//只有加速段，这时考虑加速度设置不当的问题，为了解少过冲的现象，不出现此种情况
    Con,			//只有匀速段，运行速度很小、脉冲数很少时出现
} CurveTypeEnum; 	//曲线类型

typedef struct
{
    CurveTypeEnum CurveType;
    u32 AccStartIndex;
    u32 AccEndIndex;
    u32 ConStartIndex;
    u32 ConEndIndex;
    u32 DecStartIndex;
    u32 DecEndIndex;
} CurveParaStruct;

//初始速度、终止速度
extern float _vs;

void IniPerToCOM(MotorIDEnum motorID);
//添加预装值到缓存，个数满足条件后就发送到串口
void AddSendPer(MotorIDEnum motorID,u16 per);
//发送剩余的预装值到串口
void SendSYPer();

//根据脉冲数创建一个曲线参数表
void CreateCurve(MotorIDEnum motorID, u32 pulseCount,float runSpeed,float accSpeed);

//PWM定时器中断调用
void PWM(MotorIDEnum motorID);

//电机停止，内部调用
void StopMotor_in(MotorIDEnum motorID);

#ifdef __cplusplus
}
#endif

#endif