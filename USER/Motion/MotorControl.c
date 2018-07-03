#include "MotorControl.h"
#include "Parameter.h"
#include "Timer.h"
#include "TCurve.h"
#include "LineRunin.h"

//电机控制参数列表
MotorControlStruct MotorList[MotorPortNum];
//点动运行参数列表
PTRunParaStruct PTRunPara[MotorPortNum];
//点动运行速度列表。减速度目前无用，与加速度一样
//第一个索引表示：慢、中、快；第二个索引表示：匀速度、加速度、减速度
//const float _ptSpeed[3][3]={{10,50,50},{50,250,250},{100,1000,1000}};
const float _ptSpeed[3][3]={{50,800,800},{400,2000,2000},{500,5000,5000}};
//复位标志，解决点动过原点与系统复位有矛盾的问题
bool ResetFlag= false;

//由一个电机ID得到电机字符串
char* GetMotorStrFromID(MotorIDEnum motorID)
{
	switch(motorID)
	{
		case M1:return "M1";
		case M2:return "M2";
		case M3:return "M3";
		case M4:return "M4";
		case M5:return "M5";
		case M6:return "M6";
	}
}

//由一个电机字符得到电机编号枚举,如果不是电机字符返回9999
MotorIDEnum GetMotorIDFromStr(char* motorStr)
{
	if(StrCMP_n(motorStr,"M1",2))
		return M1;
	if(StrCMP_n(motorStr,"M2",2))
		return M2;
	if(StrCMP_n(motorStr,"M3",2))
		return M3;
	if(StrCMP_n(motorStr,"M4",2))
		return M4;
	if(StrCMP_n(motorStr,"M5",2))
		return M5;
	if(StrCMP_n(motorStr,"M6",2))
		return M6;
	return 99;
}

//设置电机旋转方向
void SetMotorDirect(MotorIDEnum motorID, DirectEnum direct)
{
	MotorList[motorID].Direct = direct;
	if(direct == ZFX)
	{
		GPIO_SetBits(MotorPortList[motorID].DirectPort, MotorPortList[motorID].DirectPin);//设置DIR
	}		
	else
	{
		GPIO_ResetBits(MotorPortList[motorID].DirectPort, MotorPortList[motorID].DirectPin);
	}	
}

//电机初始化,对所有电机进行初始化
void MotorInit(void)
{
	float workSpeedPer=Parameter.WorkSpeedPercent/100.0f;
	for (u8 i = 0; i < MotorPortNum; i++)
	{
		MotorList[i].MotorID = (MotorIDEnum)i;		
		MotorList[i].ConSpeed = (u32)(Parameter.RunSpeed[i]*workSpeedPer);
		MotorList[i].AccSpeed = (u32)(Parameter.AccSpeed[i]*workSpeedPer);
		MotorList[i].DecSpeed = (u32)(Parameter.DecSpeed[i]*workSpeedPer);
		SetMotorDirect(i,ZFX);		
		MotorList[i].MaxSteps = (s32)(Parameter.AxisLength[i]/Parameter.PulseK[i]);	
		MotorList[i].StopTag	= false;	
	}
}

//点动参数设置，一般轴是长度，旋转轴时是角度
void SetPTRunPara(PTSpeedTypeEnum ptSpeedType)
{
	float ptSpeedPer=Parameter.PTSpeed/100.0f;
	for(int i=0;i<MotorPortNum;i++)
	{
		PTRunPara[i].PTSpeedType = ptSpeedType;
		switch (ptSpeedType)
		{
			case SLOW:
			{
				PTRunPara[i].ConSpeed = _ptSpeed[ptSpeedType][0]*ptSpeedPer;
				PTRunPara[i].AccSpeed = _ptSpeed[ptSpeedType][1]*ptSpeedPer;
				PTRunPara[i].DecSpeed = _ptSpeed[ptSpeedType][2]*ptSpeedPer;
				PTRunPara[i].RunPulse = (u32)(0.1f/Parameter.PulseK[AxisList[i].MotorID]);
				break;
			}
			case MEDIUM:
			{
				PTRunPara[i].ConSpeed = _ptSpeed[ptSpeedType][0]*ptSpeedPer;
				PTRunPara[i].AccSpeed = _ptSpeed[ptSpeedType][1]*ptSpeedPer;
				PTRunPara[i].DecSpeed = _ptSpeed[ptSpeedType][2]*ptSpeedPer;
				PTRunPara[i].RunPulse = (u32)(4.0f/Parameter.PulseK[AxisList[i].MotorID]);//8
				break;
			}
			case FAST:
			{
				PTRunPara[i].ConSpeed = _ptSpeed[ptSpeedType][0]*ptSpeedPer;
				PTRunPara[i].AccSpeed = _ptSpeed[ptSpeedType][1]*ptSpeedPer;
				PTRunPara[i].DecSpeed = _ptSpeed[ptSpeedType][2]*ptSpeedPer;
				PTRunPara[i].RunPulse = (u32)(8.0f/Parameter.PulseK[AxisList[i].MotorID]);//20
				break;
			}
		}
	}
}

//原点光感限位中断处理函数
void OriginLimitFunc(MotorIDEnum motorID)
{
	if (!GetInPortState(MotorPortList[motorID].LimitPort) && MotorList[motorID].Direct == FFX)
	{		
		MotorList[motorID].LimitOrigin = true;
		if(ResetFlag)//手动调到过零也把坐标清零，考虑快、中、慢带来的影响
			MotorList[motorID].RealPulse = (s32)(-1*Parameter.OffsetZeroPoint[motorID]/Parameter.PulseK[motorID]);
	}
}

//最大行程限位判断并处理,可扩展为光感模式
void MaxLengLimitFunc(MotorIDEnum motorID)
{
	if(MotorList[motorID].RealPulse>MotorList[motorID].MaxSteps)
		MotorList[motorID].LimitMaxLeng = true; 
}

//检查指定的电机能否动作
bool CheckMotorIsCanRun(MotorIDEnum motorID,DirectEnum direct)
{
	MotorControlStruct* mcs=&MotorList[motorID];
	if(mcs->IsRunning || (mcs->LimitOrigin && direct==FFX)|| (mcs->LimitMaxLeng && direct==ZFX))
		return false;
	return true;
}

//一个电机运行前的准备工作
bool MotorRunIniState(MotorIDEnum motorID,s32 pulseNum)
{
	if(pulseNum==0)
		return false;
	DirectEnum direct=ZFX;
	if(pulseNum<0)
		direct=FFX;
	if(!CheckMotorIsCanRun(motorID,direct))//如检测电机不能运行
		return false;
	SetMotorDirect(motorID,direct);	
	MotorList[motorID].RunPulseNum=abs(pulseNum);
	MotorList[motorID].RunPulseCounter=0;
	MotorList[motorID].IsRunning=true;	
	return true;
}

//电机运行函数。motorID:要运行的电机号，pulseNum：要走的脉冲数，符号表示旋转方向
//runSpeed：匀速速度;accSpeed：加速度
void MotorRun_OutSpeed(MotorIDEnum motorID,s32 pulseNum,float runSpeed,float accSpeed)
{	
	if(!MotorRunIniState(motorID,pulseNum))
		return;
	MotorList[motorID].RunType=SingleMotor;
	CreateCurve(motorID,MotorList[motorID].RunPulseNum,runSpeed,accSpeed);
	PWM_TIMControl(motorID,ENABLE);	
}

//电机参数外的速度设置，多轴同时运行，提高效率
void MotorRun_OutSpeed_Multi(MotorIDEnum* motors,s32* pulseNumList,float* runSpeed,float* accSpeed,u8 count,bool isSynch)
{
	if(count==0)
		return;
	bool isCanRun[count];
	for(u8 i=0;i<count;i++)
	{
		MotorIDEnum motorID=motors[i];
		if(MotorRunIniState(motorID,pulseNumList[i]))
		{
			MotorList[motorID].RunType=SingleMotor;
			isCanRun[i]=true;
			CreateCurve(motorID,abs(pulseNumList[i]),runSpeed[i],accSpeed[i]);
		}
		else
		{
			isCanRun[i]=false;
		}
	}
	for(u8 i=0;i<count;i++)
	{
		if(isCanRun[i])
			PWM_TIMControl(motors[i],ENABLE);
	}
	if(isSynch)
	{
		for(int i=0; i<count; i++)
		{
            while(MotorList[motors[i]].IsRunning);
		}
	}
}

//使指定的电机进入运行。motorID:要运行的电机号，pulseNum：要走的脉冲数，符号表示旋转方向
//运行速度为参数设置的速度及百分比
//可以多个电机同时运行，为了优化执行效率，2018.04.02
void MotorRun(MotorIDEnum* motors,s32* pulseNumList,u8 count,bool isSynch)
{
	if(count==0)
		return;
	float workSpeedPer=Parameter.WorkSpeedPercent/100.0f;
	bool isCanRun[count];
	for(u8 i=0;i<count;i++)
	{
		MotorIDEnum motorID=motors[i];
		if(MotorRunIniState(motorID,pulseNumList[i]))
		{
			MotorList[motorID].RunType=SingleMotor;
			isCanRun[i]=true;
			CreateCurve(motorID,abs(pulseNumList[i]),Parameter.RunSpeed[motorID]*workSpeedPer,Parameter.AccSpeed[motorID]*workSpeedPer);
		}
		else
		{
			isCanRun[i]=false;
		}
	}
	for(u8 i=0;i<count;i++)
	{
		if(isCanRun[i])
		{
			PWM_TIMControl(motors[i],ENABLE);
		}
	}
	if(isSynch)
	{
		for(int i=0; i<count; i++)
		{
            while(MotorList[motors[i]].IsRunning);
		}
	}
}

//停止指定电机,要设计减速停止
void MotorStop(MotorIDEnum motorID)
{
	//需要加上此判断，否则当电机停止时再置此标志，下次移动将因为停止标志而出错
	if(MotorList[motorID].IsRunning)
		MotorList[motorID].StopTag=true;
}

//电机开始点动
void MotorPTRun(MotorIDEnum motorID,DirectEnum direct)
{
	if(MotorList[motorID].IsRunning)
		return;
	MotorList[motorID].RunMode=PTRun;
	MotorList[motorID].PTRunTag=true;
	s8 k=1;
	if(direct==FFX)
		k=-1;
	MotorRun_OutSpeed(motorID,k*PTRunPara[motorID].RunPulse,PTRunPara[motorID].ConSpeed,PTRunPara[motorID].AccSpeed);
}

//电机停止点动
void MotorPTStop(MotorIDEnum motorID)
{
	MotorList[motorID].PTRunTag=false;
}

