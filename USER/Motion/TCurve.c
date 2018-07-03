#include "TCurve.h"
#include "Timer.h"
#include "Parameter.h"
#include "Timekeeper.h"

float _clk_preK;
u16 _conPeriod[MotorPortNum]; //匀速预装值
float AccTList[MotorPortNum];
float DecTList[MotorPortNum];
float _currMaxSpeed[MotorPortNum];//由加速度经过一段时间后到达的最大运行速度，不一定等于参数设置的速度
CurveParaStruct CurveList[MotorPortNum];//曲线参数列表

//根据脉冲数创建一个曲线参数表
void CreateTCurve(MotorIDEnum motorID, u32 pulseCount,float runSpeed,float accSpeed)
{	
	MotorControlStruct* mcs=&MotorList[motorID];
	mcs->ConSpeed=runSpeed;
	mcs->AccSpeed=accSpeed;
	mcs->DecSpeed=accSpeed;
	float pulseK=Parameter.PulseK[motorID];
	CurveParaStruct* cps=&CurveList[motorID];
	u32 timeCLK=MotorPortList[motorID].TimCLK;//定时器总线频率
	u16 timePrescaler=MotorPortList[motorID].Prescaler;
	_clk_preK=timeCLK/timePrescaler;
	
	if (runSpeed <= _vs * 3 || pulseCount < 10)//如果速度很小或脉冲数很少，只用匀速即可
	{
		cps->CurveType=Con;
		cps->ConStartIndex=0;
		cps->ConEndIndex=pulseCount-1;
	}
	else
	{
		u32 pulseIndex=0;
		float T=0;
		float Vt=0;
		
		while(true)
		{
			Vt=_vs+accSpeed*T;
			float pulseSpeed=Vt/pulseK;
			T=T+1.0f/pulseSpeed;
			if(Vt>=runSpeed || pulseIndex>=pulseCount)
			{
				u32 accCount=pulseIndex+1;//加速段脉冲数				
				if(pulseCount>2*accCount)//可以有匀速段
				{		
					cps->CurveType=Acc_Con_Dec;					
					cps->AccStartIndex=0;
					cps->AccEndIndex=accCount-1;
					
					u32 conCount=pulseCount-2* accCount;//匀速段脉冲数
					cps->ConStartIndex=cps->AccEndIndex+1;
					cps->ConEndIndex=accCount+conCount-1;
					
					cps->DecStartIndex=cps->ConEndIndex+1;
					cps->DecEndIndex=pulseCount-1;
				}			
				else//没有匀速段
				{
					cps->CurveType=Acc_Dec;					
					cps->AccStartIndex=0;
					cps->AccEndIndex=pulseCount/2;
					
					cps->DecStartIndex=cps->AccEndIndex+1;
					cps->DecEndIndex=pulseCount-1;
				}
				break;
			}	
			pulseIndex++;			
		}
	}		
	float pSpeed=runSpeed/pulseK;
	_conPeriod[motorID]=(u16)(_clk_preK/pSpeed);	
	mcs->TimPeriod=_conPeriod[motorID];
	if(cps->CurveType!=Con)//如果不全为匀速
	{
		float pulseSpeed=_vs/pulseK;
		mcs->TimPeriod=(u16)(_clk_preK/pulseSpeed);
	}
	TIMPeriodSet(motorID,mcs->TimPeriod);	
	mcs->RunPulseCounter=0;
	AccTList[motorID]=0;
	DecTList[motorID]=0;
}

u16 CalcAccPeriodValue(MotorIDEnum motorID)
{	
	_currMaxSpeed[motorID]=_vs+MotorList[motorID].AccSpeed*AccTList[motorID];
	if (_currMaxSpeed[motorID] >=MotorList[motorID].ConSpeed)
		_currMaxSpeed[motorID] = MotorList[motorID].ConSpeed;
	float pulseSpeed=_currMaxSpeed[motorID]/Parameter.PulseK[motorID];
	u16 periodValue=(u16)(_clk_preK/pulseSpeed);
	AccTList[motorID]=AccTList[motorID]+1.0f/pulseSpeed;
	return periodValue;
}

u16 CalcDecPeriodValue(MotorIDEnum motorID)
{
	float Vt=_currMaxSpeed[motorID]-MotorList[motorID].DecSpeed*DecTList[motorID];
	if (Vt <= _vs)
	{
		Vt = _vs;
		if(MotorList[motorID].StopTag)
		{
			StopMotor_in(motorID);
		}
	}
	float pulseSpeed=Vt/Parameter.PulseK[motorID];
	u16 periodValue=(u16)(_clk_preK/pulseSpeed);
	DecTList[motorID]=DecTList[motorID]+1.0f/pulseSpeed;
	return periodValue;
}

//获取指定点的速度预装值,插补加减速的第一种方式
u16 GetCurrPeriodValue_TCurve_Out(MotorIDEnum motorID,u32 currPulseIndex)
{
	CurveParaStruct* cps=&CurveList[motorID];
	u16 currPeriod=0;
	if((cps->CurveType==Con) ||
	   (cps->CurveType==Acc_Con_Dec && currPulseIndex>=cps->ConStartIndex && currPulseIndex<=cps->ConEndIndex))//有匀速段，且在匀速段
	{
		currPeriod=_conPeriod[motorID];
	}
	else if(currPulseIndex>=cps->AccStartIndex && currPulseIndex<=cps->AccEndIndex)//在加速段
	{
		currPeriod=CalcAccPeriodValue(motorID);
	}
	else//减速段
	{
		currPeriod=CalcDecPeriodValue(motorID);
	}
	return currPeriod;	
}

//点动处理
void PTRunProc(MotorIDEnum motorID)
{
	MotorControlStruct* mcs=&MotorList[motorID];
	CurveParaStruct* cps=&CurveList[motorID];
	if(!mcs->PTRunTag)//点动停止
	{
		mcs->RunMode=NormalRun;
		if(cps->CurveType==Con)//如果只有匀速，立即停
		{
			StopMotor_in(motorID);
		}
		else//进入减速停止
		{
			u32 decCount=cps->DecEndIndex-cps->DecEndIndex+1;
			cps->DecStartIndex=mcs->RunPulseCounter;
			cps->DecEndIndex=cps->DecStartIndex+decCount-1;
			mcs->RunPulseCounter=cps->DecStartIndex;
		}		
	}
	else //点动运行
	{
		if(cps->CurveType!=Con && mcs->RunPulseCounter<=cps->AccEndIndex)//如果不全是匀速，且在加速段
		{
			mcs->TimPeriod=CalcAccPeriodValue(motorID);
		}
		else
		{
			mcs->TimPeriod=_conPeriod[motorID];
		}
	}
}

//T形加减速PWM
void PWM_T(MotorIDEnum motorID)
{
	MotorControlStruct* mcs=&MotorList[motorID];
	CurveParaStruct* cps=&CurveList[motorID];
	if(mcs->Direct==ZFX)
	{	  	
		mcs->RealPulse++;		
		MaxLengLimitFunc(motorID);
	}
	else
	{
		mcs->RealPulse--;
	}
	if(mcs->LimitOrigin && mcs->Direct==ZFX)
		mcs->LimitOrigin=false;
	if(mcs->LimitMaxLeng && mcs->Direct==FFX)
		mcs->LimitMaxLeng=false;
	mcs->RunPulseCounter++;
	
	//走到左、右限位必需停止
	if((mcs->LimitOrigin && mcs->Direct==FFX)||(mcs->LimitMaxLeng && mcs->Direct==ZFX))
	{
		StopMotor_in(motorID);
	}	
	
	//点动运行处理
	if(mcs->RunMode==PTRun)
	{
		PTRunProc(motorID);
		return;
	}
	
	//是否已走完所有脉冲
//	if(mcs->RunPulseCounter>=mcs->RunPulseNum)
//	{
//		StopMotor_in(motorID);
//	}
	//是否已走完所有脉冲，插补时的脉冲数由插补类自行判断
	if(mcs->RunPulseCounter>=mcs->RunPulseNum && mcs->RunType!=Runin)
	{
		StopMotor_in(motorID);
	}
	//如果收到急停标志，急停处理
	if(mcs->StopTag)
	{
		//mcs->StopTag=false; //只进一次,如果加此句停止效果不理想，为什么??
		//如果已经在减速段，不处理，等待完成减速
		//如果不在减速段,脉冲索引定位到减速段
		if(mcs->RunPulseCounter<cps->DecStartIndex) 
		{
			mcs->RunPulseCounter=cps->DecStartIndex;
		}		
	}
		
	//插补加减速的第一种方式，在插补类中改变预装值
//	if(mcs->RunType==Runin)
//	{
//		OnRunin(motorID);
//		return;
//	}	
	if((cps->CurveType==Con) ||(cps->CurveType==Acc_Con_Dec && mcs->RunPulseCounter>=cps->ConStartIndex 
		&& mcs->RunPulseCounter<=cps->ConEndIndex))//有匀速段，且在匀速段
	{
		mcs->TimPeriod=_conPeriod[motorID];
	}
	else if(mcs->RunPulseCounter>=cps->AccStartIndex && mcs->RunPulseCounter<=cps->AccEndIndex)//在加速段
	{
		mcs->TimPeriod=CalcAccPeriodValue(motorID);
	}
	else//减速段
	{
		mcs->TimPeriod=CalcDecPeriodValue(motorID);
	}			
	//插补加减速的第二种方式，改变预装值后再调用插补
//	if(mcs->RunType==Runin)
//	{
//		OnRunin(motorID);
//	}
}
