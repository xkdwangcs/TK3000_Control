#ifndef __AXISCONTROL_H
#define __AXISCONTROL_H

#include "MotorControl.h"

#define AxisNum 	4  //本项目中所用到的最大轴数

typedef enum
{
	X = 0, 
	Y1,
	Y2,
	Z,	
} AxisIDEnum;//轴枚举

typedef struct
{
	float X;
	float Y;
	float Z;
} CoordStruct;//坐标结构

typedef struct
{
	float X1;
    float X2;
	float Y1;//左平台Y坐标
	float Y2;//右平台Y坐标
	float Z1;
    float Z2;
}MultiAxisCoordStruct;//多轴坐标，主要用于实时坐标

typedef enum
{
	PlaneAxis,   //平面轴，
	VerticalAxis,//垂直轴
	RevolveAxis, //旋转轴	
}AxisTypeEnum;//轴类型枚举

typedef struct
{
	AxisIDEnum AxisID; 		//当前作用的电机端口编号
	MotorIDEnum MotorID;	//与轴对应的电机编号
	AxisTypeEnum AxisType;	//轴类型	
}AxisControlStruct;//轴控制参数结构

//轴控制参数列表
extern AxisControlStruct AxisList[AxisNum];
//当前坐标
extern MultiAxisCoordStruct _currCoord;
extern MotorIDEnum _xMotor;
extern MotorIDEnum _y1Motor;
extern MotorIDEnum _y2Motor;
extern MotorIDEnum _zMotor;

//轴初始化,对本项目所用的全部轴进行初始化
void AxisInit(void);
//取到实时坐标
MultiAxisCoordStruct* GetRealTimeCoord(void);
//由一个轴号得到轴字符串
char* GetAxisStrFromID(AxisIDEnum axisID);
//由轴字符串得到轴号
AxisIDEnum GetAxisIDFormStr(char* axisStr);
//获取指定轴的运行状态，true:正在运行，false:停止
//替换原来的Motor.IsRunning成员
bool GetAxisRunState(AxisIDEnum axisID);
//获取指定轴的当前脉冲值，相对于零点的值
s32 GetAxisRealPulse(AxisIDEnum axisID);
//多轴同时运行，优化了运行效率。2018.04.02
void MoveToCoord_Multi(AxisIDEnum* axisList,float* destCoordList,u8 count ,bool isSynch);
//指定电机移动到目的坐标，没有插入动作，作为移动到坐标功能的底层函数
//isSynch:是否同步，指是否等待电机移动完后再返回，否则为异步
void MoveToCoord(AxisIDEnum motorID,float destCoord,bool isSynch);
//停止指定的轴
void StopAxis(AxisIDEnum axisID);
//停止平台所有的轴运动
void StopAllAxis(void);

//多轴回零，带同步信号，并移动到坐标位置
void MultiAxisMoveToPositionZeroAndMove(AxisIDEnum* id,u8 count);

//轴点动运行
void AxisPTRun(AxisIDEnum axisID,DirectEnum direct);
//轴点动运行
void AxisPTStop(AxisIDEnum axisID);

#ifdef __cplusplus
}
#endif

#endif
