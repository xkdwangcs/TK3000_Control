#include "AxisControl.h"
#include "Parameter.h"
#include "PortConfigFile.h"
#include "math.h"

//轴控制参数列表
AxisControlStruct AxisList[AxisNum];
//当前坐标
MultiAxisCoordStruct _currCoord;
MotorIDEnum _xMotor=M1;
MotorIDEnum _y1Motor=M2;
MotorIDEnum _y2Motor=M4;
MotorIDEnum _zMotor=M3;

MotorIDEnum GetMotorIDFormPortFile(AxisIDEnum axisID)
{
	for(u8 i=0;i<AxisNum;i++)
	{
		if(PortMotorCFG.MotorCFG[i].AxixID==axisID)
			return PortMotorCFG.MotorCFG[i].MotorID;
	}	
}

//轴初始化,对本项目所用的全部轴进行初始化
void AxisInit(void)
{
	MotorInit();
	for(u8 i=0;i<AxisNum;i++)
	{
		AxisList[i].AxisID=(AxisIDEnum)i;
		//AxisList[i].MotorID=PortMotorCFG.MotorCFG[i].MotorID;//从配置文件中读出
	}
	AxisList[X].MotorID=GetMotorIDFormPortFile(X);//从配置文件中读出
	AxisList[Y1].MotorID=GetMotorIDFormPortFile(Y1);
	AxisList[Y2].MotorID=GetMotorIDFormPortFile(Y2);
	AxisList[Z].MotorID=GetMotorIDFormPortFile(Z);
	
	AxisList[X].AxisType=PlaneAxis;
	AxisList[Y1].AxisType=PlaneAxis;
	AxisList[Y2].AxisType=PlaneAxis;
	AxisList[Z].AxisType=VerticalAxis;
	
	//可供外部直接调用	
	_xMotor=AxisList[X].MotorID;
	_y1Motor=AxisList[Y1].MotorID;
	_y2Motor=AxisList[Y2].MotorID;
	_zMotor=AxisList[Z].MotorID;
}

//取到实时坐标
MultiAxisCoordStruct* GetRealTimeCoord(void)
{
	_currCoord.X1 = MotorList[_xMotor].RealPulse * Parameter.PulseK[_xMotor];
	_currCoord.Y1 = MotorList[_y1Motor].RealPulse * Parameter.PulseK[_y1Motor];
	_currCoord.Y2 = MotorList[_y2Motor].RealPulse * Parameter.PulseK[_y2Motor];
	_currCoord.Z1 = MotorList[_zMotor].RealPulse * Parameter.PulseK[_zMotor];
	return &_currCoord;
}

//由一个轴号得到轴字符串
char* GetAxisStrFromID(AxisIDEnum axisID)
{
	switch(axisID)
	{
		case X:return "X";
		case Y1:return "Y1";
		case Y2:return "Y2";
		case Z:return "Z";
	}
}

//由轴字符串得到轴号
AxisIDEnum GetAxisIDFormStr(char* axisStr)
{
	if(StrCMP_n(axisStr,"X",1))
	{
		return X;
	}
	if(StrCMP_n(axisStr,"Y1",2))
	{
		return Y1;
	}
	if(StrCMP_n(axisStr,"Y2",2))
	{
		return Y2;
	}
	if(StrCMP_n(axisStr,"Z",1))
	{
		return Z;
	}
	u8 leng=strlen(axisStr);
	if(leng==1 && StrCMP_n(axisStr,"Y",leng))
	{
		return Y1;
	}
}

//获取指定轴的运行状态，true:正在运行，false:停止
bool GetAxisRunState(AxisIDEnum axisID)
{
	return MotorList[AxisList[axisID].MotorID].IsRunning;
}

//获取指定轴的当前实时脉冲值，相对于零点的值
s32 GetAxisRealPulse(AxisIDEnum axisID)
{
	return MotorList[AxisList[axisID].MotorID].RealPulse;
}

//轴点动运行
void AxisPTRun(AxisIDEnum axisID,DirectEnum direct)
{
	MotorPTRun(AxisList[axisID].MotorID,direct);
}

//轴点动运行
void AxisPTStop(AxisIDEnum axisID)
{
	MotorPTStop(AxisList[axisID].MotorID);
}

//多轴同时运行，优化了运行效率。2018.04.02
void MoveToCoord_Multi(AxisIDEnum* axisList,float* destCoordList,u8 count,bool isSynch)
{
    MotorIDEnum motors[count];
    s32 pulseList[count];
    for(u8 i=0; i<count; i++)
    {
        MotorIDEnum mid=AxisList[axisList[i]].MotorID;
        s32 steps = (s32)(floor(destCoordList[i]/Parameter.PulseK[mid])-MotorList[mid].RealPulse);
        motors[i]=mid;
        pulseList[i]=steps;
    }
    MotorRun(motors,pulseList,count,isSynch);
}

//指定电机移动到目的坐标，没有插入动作，作为移动到坐标功能的底层函数
//isSynch:是否同步，指是否等待电机移动完后再返回，否则为异步
void MoveToCoord(AxisIDEnum axisID,float destCoord,bool isSynch)
{
    AxisIDEnum axisList[1]= {axisID};
    float coordList[1]= {destCoord};
    MoveToCoord_Multi(axisList,coordList,1,isSynch);
}

//停止指定的轴
void StopAxis(AxisIDEnum axisID)
{
	MotorStop(AxisList[axisID].MotorID);
}

//停止平台所有的轴运动
void StopAllAxis(void)
{
	for(u8 i=0;i<AxisNum;i++)
	{
		MotorStop(AxisList[i].MotorID);
	}
}

//移动三个轴直到限位信号外部中断，最长为行程长度
//复位速度要设置？
//多轴回零，带同步信号
void MultiAxisMoveToPositionZero(AxisIDEnum* axisIDs,u8 count)
{
    ResetFlag=true;
    float resetSpeed=Parameter.ResetSpeed;
	u8 countReal=0;
	MotorIDEnum motors[count];
	s32 pulseNum[count];
	float runSpeed[count];
	float accSpeed[count];
	
    for(int i=0; i<count; i++)
    {
        //若在限位位置，等待往正方向走20mm
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
        if(!GetInPortState(MotorPortList[motorID].LimitPort))//如果已经在原点或负原点
        {
			motors[countReal]=motorID;
			pulseNum[countReal]=20/Parameter.PulseK[motorID];
			runSpeed[countReal]=resetSpeed;
			accSpeed[countReal]=5*resetSpeed;
			countReal++;
        }
    }
	MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,countReal,true);
	
    //回零，行程长度多走50mm
    for(int i=0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
		motors[i]=motorID;
		pulseNum[i]=-(Parameter.AxisLength[motorID]+50)/Parameter.PulseK[motorID];
		runSpeed[i]=resetSpeed;
		accSpeed[i]=5*resetSpeed;
    }
    MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,true);
	
    //往正向走回零误差
    for(int i=0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
		motors[i]=motorID;
		pulseNum[i]=-MotorList[motorID].RealPulse*5;
		runSpeed[i]=resetSpeed;
		accSpeed[i]=5*resetSpeed;
    }
	//MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,isSynch);
	MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,true);

    //第二次找光感零点
    for(int i=0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
		motors[i]=motorID;
		pulseNum[i]=-MotorList[motorID].RealPulse*2;
		runSpeed[i]=resetSpeed/4;
		accSpeed[i]=resetSpeed;
    }
	MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,true);
	//原点限位标志清零
    for(int i=0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
        MotorList[motorID].LimitOrigin = false;
    }
	
    //往正向走回零误差
    for(int i=0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
		motors[i]=motorID;
		pulseNum[i]=-MotorList[motorID].RealPulse;
		runSpeed[i]=resetSpeed/4;
		accSpeed[i]=resetSpeed;
    }
    MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,true);
    ResetFlag=false;
}

//多轴回零，带同步信号，并移动到坐标位置
void MultiAxisMoveToPositionZeroAndMove(AxisIDEnum* axisIDs, u8 count)
{
    MultiAxisMoveToPositionZero(axisIDs,count);
    float resetSpeed=Parameter.ResetSpeed;
	MotorIDEnum motors[count];
	s32 pulseNum[count];
	float runSpeed[count];
	float accSpeed[count];
    for(int i = 0; i<count; i++)
    {
        MotorIDEnum motorID=AxisList[axisIDs[i]].MotorID;
        float coord=Parameter.WorkHeight;
        if(AxisList[axisIDs[i]].AxisType!=VerticalAxis)//如果不是垂直轴
        {
            coord=GetScrewOneAxisCoord(axisIDs[i]);
        }
		motors[i]=motorID;
		pulseNum[i]=coord/Parameter.PulseK[motorID];
		runSpeed[i]=resetSpeed;
		accSpeed[i]=5*resetSpeed;
    }
    MotorRun_OutSpeed_Multi(motors,pulseNum,runSpeed,accSpeed,count,true);
}

