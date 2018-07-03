#ifndef __MOVINGPROCFILE_H
#define __MOVINGPROCFILE_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "gpio.h"
#include "AxisLinkage.h"

#define MoveMaxCount 40 //一个动作组的最大动作数

typedef enum
{
	OutPortMotion=0,	//输出口动作
	AxisMove,			//电机移动到指定坐标
	AxisReset,			//电机复位，回到原点后再回到起始点
	NReset,				//N次自动复位
	WaitTakeWork,		//前放后取时,等待取件
	DelayMotion,		//动作延时
	AxisOrigin,			//电机到起始点
	AxisParaCoord,		//轴移动到参数设定的坐标
	WaitInport,			//等待输入端口信号
	InportEvent,		//输入事件
}MotionTypeEnum;//动作类型
	 
typedef struct
{
	u8 PortIndex;		//端口索引，从0开始
	bool IsHV;			//输出口状态，true:使能端口,一般指高电平；false:关闭端口,一般指低电平
	float MoveDelay;	//动作延时(s),包括使能延时及关闭延时	
}OutputMovePara;//输出口动作参数

typedef struct
{
	AxisIDEnum AxisList[AxisNum];	//轴号列表
	float DestCoord[AxisNum];		//目的坐标
	u8 AxisCount;					//动作的电机数
}MotorMovePara;//电机动作参数

typedef struct
{
	AxisIDEnum AxisList[AxisNum];	//轴号列表
	u8 AxisCount;					//本动作的复位轴数
}MotorResetPara;//复位动作参数

typedef struct
{
	AxisIDEnum AxisList[AxisNum];	//轴号列表
	u8 AxisCount;					//本动作的轴数
}MotorToStartPara;//轴移动到起始点

typedef struct
{
	AxisIDEnum AxisList[AxisNum];	//轴号列表
	u8 AxisCount;					//本动作的轴数
}AxisToParaCoordStruct;//轴移动到参数中的设置点

typedef struct
{
	float DelayValue;//要延时的值(s)
}DelayParaStruct;//延时参数结构体

typedef struct
{
	u8 PortIndex;		//输入口索引，从0开始
	bool InputState;	//输入口状态，高或低时使输出动作
	float WaitDelay;	//等待输入口状态的最大时长
}WaitInportParaStruct;//等待输入端口信号结构体

typedef struct
{
	u8 PortIndex;		//输入口索引，从0开始
	bool InputState;	//输入口状态，高或低时使输出动作
	char Msg[40];		//提示的信息
}InportEventStruct;		//输入事件结构体
	 
typedef struct
{
	//动作类型。0：输出口动作，1：电机运动,2:电机复位，3：N次自动复位，4:等待取件，5：延时，6：电机到起始点，7:轴移动到参数设定的坐标
	MotionTypeEnum MotionType;
	OutputMovePara OutputPara;				//输出动作参数
	MotorMovePara MotorPara;				//电机动作参数
	MotorResetPara ResetPara;				//电机复位参数
	DelayParaStruct DelayPara;  			//延时参数实例
	MotorToStartPara MotorToStart;			//电机到起始点参数
	AxisToParaCoordStruct AxisToParaCoord;	//轴移动到参数中的设置点
	WaitInportParaStruct WaitInportPara;	//等待输入端口信号
	InportEventStruct InportEvent;			//输入事件		
}OneMoveStruct;//一个动作的结构体 

typedef struct
{
	char MoveName[50];				 		 			//动作名称、描述
	//参数版本，用于自动升级参数扇区或当扇区损坏时重写。由于轴数在每个项目中可能不同，版本号的位置不确定，所以放在前面  
	char ParaVer[7];									
	u16 WorkBeforMoveCount_Left;			 			//左平台正常打螺丝之前的动作个数
	OneMoveStruct WorkBeforMoves_Left[MoveMaxCount];	//左平台正常打螺丝之前的动作组
	u16 WorkComplMoveCount_Left;			 			//左平台打螺丝完成后的动作个数
	OneMoveStruct WorkComplMoves_Left[MoveMaxCount];	//左平台打螺丝完成后的动作组
	u16 WorkBeforMoveCount_Right;			 			//右平台正常打螺丝之前的动作个数
	OneMoveStruct WorkBeforMoves_Right[MoveMaxCount];	//右平台正常打螺丝之前的动作组
	u16 WorkComplMoveCount_Right;			 			//右平台打螺丝完成后的动作个数
	OneMoveStruct WorkComplMoves_Right[MoveMaxCount];	//右平台打螺丝完成后的动作组
	u16 ResetMoveCount;			 	 					//复位时的动作个数，复位流程不用分左右平台
	OneMoveStruct ResetMoveMoves[MoveMaxCount];			//复位时的动作组	 
	u16 InportEventCount;								//输入口事件个数
	OneMoveStruct InportEvents[InPortNum];				//输入口事件组
}MovesStruct;//一组动作的结构体

extern char* _moveFileName;
//当前动流程文件下的内容
extern MovesStruct Moves;

//初始化动作配置文件
void IniMoveProcFile(void);
//恢复出厂默认的动作流程
void WritDefaultMoveProc(void);
//读取动作文件
void ReadMoveFile(void);
//保存流程文件
void SaveMoveFile(void);
//提示动作文件初始化信息，并写日志
void ShowMotionIniMsg(void);
	 
#ifdef __cplusplus
}
#endif

#endif
