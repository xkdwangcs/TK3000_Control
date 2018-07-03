#ifndef __COORDINATE_H
#define __COORDINATE_H 

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "AxisControl.h"
#include "AxisRunningAction.h"

typedef struct
{
	float SafeHeight;						//如果是垂直轴，必须要指定安全高度
	AxisRunningActionStruct* DownActions;	//下降时要插入的动作列表
	u8 DownActionCount;						//下降时要插入的动作个数	
	AxisRunningActionStruct* UpActions;		//上升时要插入的动作列表
	u8 UpActionCount;						//上升时要插入的动作个数
}VerticalAxisLinkStruct;//垂直轴相关参数

typedef struct
{
	AxisIDEnum AxisID;//轴号
	float DestCoord;//目的坐标值
	AxisRunningActionStruct* RunningActions;//要插入的动作列表，要求按动作坐标从小到大(往正方向时)或从大到小(入负方向走)排列
	u8 ActionCount;//过程中要插入的动作个数
	s32 OldPlusNum; //未动作前的脉冲数,内部调用，外部不用赋值
	u32 PlusNum_OtherRun;//当前轴达到多少个脉冲后其它轴可以运行。用于安全移动
	VerticalAxisLinkStruct VertAxisLink; //垂直轴相关参数，==NULL时表示为非垂直轴
}LinkageMotorStruct;//电机联动参数结构体

//三轴联动函数指针，xyz/xy2z。
typedef void (*ThreeLinkageHandler)(float xCoord,float y2Coord,float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount);

//单轴电机运转并插入一个或多个动作，同步的
//MotorRunningActions:要插入的动作列表，要求按动作坐标从小到大(往正方向时)或从大到小(入负方向走)排列
//actionCount:动作个数
void MoveToCoordRunAction(AxisIDEnum motorID,float destCoord,AxisRunningActionStruct* motorRunningActions,u8 actionCount);
//单轴电机运转并插入一个或多个动作，同步的。带速度参数
//MotorRunningActions:要插入的动作列表，要求按动作坐标从小到大(往正方向时)或从大到小(入负方向走)排列
//actionCount:动作个数
void MoveToCoordRunActionPara(AxisIDEnum motorID,float destCoord,AxisRunningActionStruct* motorRunningActions,u8 actionCount,float acc ,float speed);

//创建联动结构数组中的一根轴的参数
void CreateLinkageOneAxis(LinkageMotorStruct* oneLinkage,AxisIDEnum axisID,float destCoord,
	AxisRunningActionStruct* actionList,u8 actionCount,VerticalAxisLinkStruct* vertAxisLink);
//多轴联动，此函数考虑了一个完整的轨迹（在安全高度左右移动其它轴）
//现在只支持Z向下运动插入动作，如有需要加方向标志后可扩展到每轴都能插入动作
void MultiAxisLinkage(LinkageMotorStruct* linkageMotors,u8 count);
//停止当前联动。停止指定的所有电机，并退出
void StopAxisLinkageRun(LinkageMotorStruct* linkageMotors,u8 count);
//多轴联动，不带动作
void MultiAxisLinkageNoAction(AxisIDEnum* motors,float* coords, u8 count);
//Z轴移动到指定的坐标点,同步的。此函数可以去掉
void ZMoveToCoord(float zCoord);
//X、Z两轴联动，带运动轨迹（处理了工作高度）。
void XZLinkageToCoord(float xCoord, float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount);
//X、Y两轴联动(左平台)，一般X/Y过程中不会有动作的插入
void XYLinkageToCoord(float xCoord, float yCoord);
//X、Y2两轴联动(右平台)
void XY2LinkageToCoord(float xCoord, float yCoord);
//X、Y、Z三轴联动(左平台)，带运动轨迹（处理了工作高度）。
void XYZLinkageToCoord(float xCoord, float yCoord,float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount);
//X、Y2、Z三轴联动(右平台)，带运动轨迹（处理了工作高度）。
void XY2ZLinkageToCoord(float xCoord,float y2Coord,float zCoord,AxisRunningActionStruct* zActions,u8 zActionCount);

#ifdef __cplusplus
}
#endif
#endif
