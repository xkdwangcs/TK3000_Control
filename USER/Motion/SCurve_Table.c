#include "Timer.h"
#include "Parameter.h"
#include "MemoryAlloc.h"
#include "math.h"
#include "PublicFunction.h"
#include "SCurve_Table.h"
//#include "RuninBase.h"

//把参数列表及预装值列表分开，增加速度
AccPerValueStruct AccPerList[MotorPortNum][_accPerPoints]__at(MotionCurveBuff);		//加速段预装值列表
STableParaStruct SPara[MotorPortNum];		//加速段参数列表

//const u16 _perBuffLeng=100; //双缓存长度
//u8 _currWriteBuff=1;		//当前写缓存的索引
//u8 _currReadBuff=1;			//当前读缓存的索引
//u16 PerBuff[2][MotorPortNum][_perBuffLeng];
////u16 PerBuff2[MotorPortNum][_perBuffLeng];
//u16 writeBuffIndex=0;


void AddAccPer(MotorIDEnum motorID, float v)
{
    STableParaStruct* sps=&SPara[motorID];
	AccPerValueStruct* accPerList = AccPerList[motorID];
    u16 perValue = (u16)(sps->VPerK / v);
    u16 count=sps->AccPerCount;
	
    if (count > 0 && accPerList[count-1].PerValue == perValue)
    {
        accPerList[count-1].PerCount++;
    }
    else
    {
        accPerList[count].PerValue = perValue;
        accPerList[count].PerCount = 1;
        sps->AccPerCount++;
    }
}

void GetPulseV(MotorIDEnum motorID, float currS, float currV)
{
    STableParaStruct* sps=&SPara[motorID];
    while (true)
    {
        float currL = sps->AccPulseIndex * Parameter.PulseK[motorID];
        if (currL >= sps->LastS && currL < currS)
        {
            AddAccPer(motorID, sps->LastV);
            sps->AccPulseIndex++;
        }
        else if (currL == currS)
        {
            AddAccPer(motorID,currV);
            sps->AccPulseIndex++;
        }
        else if (currL > currS)
            break;
    }
    sps->LastS = currS;
    sps->LastV = currV;
}

//创建加速段列表
void CreateSTable(MotorIDEnum motorID, u32 pulseCount,float runSpeed,float accSpeed)
{
    float pulseK=Parameter.PulseK[motorID];
    float L = pulseCount*pulseK;
    float dt = 0.001; 		//时间离散
    float J = accSpeed * 5; //加加速度
    float vm=runSpeed;
    MotorControlStruct* mcs=&MotorList[motorID];
    AccPerValueStruct* accPerList = AccPerList[motorID];
    mcs->ConSpeed=runSpeed;
    mcs->AccSpeed=accSpeed;
    mcs->DecSpeed=accSpeed;

    u32 timeCLK=SystemCoreClock/2;//定时器总线频率
    u16 timePrescaler=MotorPortList[motorID].Prescaler;
    float v_perK=timeCLK*pulseK/timePrescaler;
    STableParaStruct* sps=&SPara[motorID];
    sps->VPerK=v_perK;
    /*if (pulseCount < 100)//如果脉冲数很少，只用匀速即可
    {
        sps->CurveType=Con;
        sps->ConPulseCount=pulseCount;
        sps->ConPeriod=(u16)(v_perK / _vs);
        return;
    }*/

    float Tm = sqrt((vm - _vs) / J);
    float Sa = 2 * _vs * Tm + J * Tm * Tm * Tm;
    //float T3 = 0;
    if (L > 2 * Sa)//有匀速段
    {
        //T3 = (L - 2 * Sa) / vm;
        sps->CurveType=Acc_Con_Dec;
    }
    else
    {
        float k1 = L * sqrt(J) / 4;
        float k2 = sqrt(L * L * J / 16 + 8 * _vs * _vs * _vs / 27);
        float tk = Sign(k1 - k2) * pow(fabs(k1 - k2), 1.0 / 3); //要注意，C语言中的abs函数是int,fabs才是浮点数
        float k3 = pow(k1 + k2, 1.0f / 3) + tk;
        Tm = k3 / sqrt(J);
        vm = k3 * k3 + _vs;
        //Sa = 2 * _vs * Tm + J * Tm * Tm * Tm;
        Sa = L / 2;
        sps->CurveType=Acc_Dec;
    }

    u32 accCount = (u32)(Sa / pulseK);
    sps->CurveType=Acc_Con_Dec;
    sps->AccPulseCount=accCount;
    sps->DecPulseCount=accCount;
    sps->ConPulseCount=pulseCount- accCount-sps->DecPulseCount;//匀速段脉冲数
    sps->Acc_Con_Count=accCount+sps->ConPulseCount;
    sps->ConPeriod=(u16)(v_perK / vm);
    sps->AccPulseIndex=0;
    sps->AccPerCount = 0;
    memset(accPerList,0,sizeof(AccPerList[0]));
    sps->LastV = _vs;
    sps->LastS = 0;

    float t = 0;
    float vi = 0;
    float si = 0;
    float tm2 = Tm * Tm;
    float tm3 = Tm * Tm * Tm;
    float tm_2 = 2 * Tm;
    float vsjtm_2 =_vs + J * tm2;
    float jtm3 = J * tm3;
    while (true)
    {
        if (t <= Tm)
        {
            float jtt=J * t * t;
            vi = _vs + jtt / 2;
            si = _vs * t + jtt * t / 6;
            GetPulseV(motorID, si, vi);
            t = t + dt;
        }
        else if (t > Tm && t <= tm_2)
        {
            float tt = t - tm_2;
            float jtt=J * tt * tt;
            vi = vsjtm_2 - jtt / 2;
            si = vsjtm_2 * t - jtt * tt / 6 - jtm3;
            GetPulseV(motorID,si, vi);
            t = t + dt;
            if(sps->AccPulseIndex>=sps->AccPulseCount)
                break;
        }
        else
        {
            break;
        }
    }

    u32 hasCount=sps->AccPulseIndex;
    if (accCount > hasCount)
    {
        u16 yxCount = (u16)(accCount - hasCount);
        accPerList[sps->AccPerCount - 1].PerCount += yxCount;
    }
    /*else if(accCount < hasCount) //前面已确保不会多出
    {
        u16 yxCount = (u16)(hasCount - accCount);
        sps->AccPerList[sps->AccPerCount - 1].PerCount -= yxCount;
    }*/
    sps->CurrAccTableRow = 0;
    sps->CurrRowMaxIndex = accPerList[sps->CurrAccTableRow].PerCount;
    sps->CurrDecTableRow = sps->AccPerCount-1;
    sps->CurrRowMinIndex = sps->AccPulseCount - accPerList[sps->CurrDecTableRow].PerCount;
    mcs->TimPeriod=accPerList[0].PerValue;
    TIMPeriodSet(motorID,mcs->TimPeriod);
    mcs->RunPulseCounter=0;
}

/// <summary>
/// 从表中查询指定脉冲数的预装值
/// </summary>
u16 GetAccPerFormTable(MotorIDEnum motorID)
{		
    MotorControlStruct* mcs=&MotorList[motorID];
    STableParaStruct* sps=&SPara[motorID];
	AccPerValueStruct* accPerList = AccPerList[motorID];
    if (mcs->RunPulseCounter >= sps->CurrRowMaxIndex && sps->CurrAccTableRow < sps->AccPerCount-1)
    {
        sps->CurrAccTableRow++;
        sps->CurrRowMaxIndex += accPerList[sps->CurrAccTableRow].PerCount;
    }
    return accPerList[sps->CurrAccTableRow].PerValue;	
}

/// <summary>
/// 从表中查询指定脉冲数的预装值
/// </summary>
u16 GetDecPerFormTable(MotorIDEnum motorID)
{
    MotorControlStruct* mcs=&MotorList[motorID];
    STableParaStruct* sps=&SPara[motorID];
	AccPerValueStruct* accPerList = AccPerList[motorID];
    u32 pulseIndex=sps->AccPulseCount-( mcs->RunPulseCounter-sps->Acc_Con_Count)-1;
    if (pulseIndex < sps->CurrRowMinIndex && sps->CurrDecTableRow>0)
    {
        sps->CurrDecTableRow--;
        sps->CurrRowMinIndex -= accPerList[sps->CurrDecTableRow].PerCount;
    }
    return accPerList[sps->CurrDecTableRow].PerValue;	
}

//点动处理
void PTRunProc_S(MotorIDEnum motorID)
{
    MotorControlStruct* mcs=&MotorList[motorID];
    STableParaStruct* sps=&SPara[motorID];
    if(!mcs->PTRunTag)//点动停止
    {
        mcs->RunMode=NormalRun;
        if(sps->CurveType==Con)//如果只有匀速，立即停
        {
            StopMotor_in(motorID);
        }
        else//进入减速停止
        {
            mcs->RunPulseCounter=sps->Acc_Con_Count;
        }
    }
    else //点动运行
    {
        if(sps->CurveType!=Con && mcs->RunPulseCounter<sps->AccPulseCount)//如果不全是匀速，且在加速段
        {
            mcs->TimPeriod=GetAccPerFormTable(motorID);
        }
        else
        {
            mcs->TimPeriod=SPara[motorID].ConPeriod;
        }
    }
}

void PWM_ST(MotorIDEnum motorID)
{		
    MotorControlStruct* mcs=&MotorList[motorID];
    STableParaStruct* sps=&SPara[motorID];
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
        PTRunProc_S(motorID);
        return;
    }

    //是否已走完所有脉冲
/*  if(mcs->RunPulseCounter>=mcs->RunPulseNum)
    {
        StopMotor_in_S(motorID);
    }*/
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
        if(mcs->RunPulseCounter<sps->Acc_Con_Count)
        {
            mcs->RunPulseCounter=sps->Acc_Con_Count;
        }
    }
	
	//插补加减速的第一种方式，在插补类中改变预装值
//    if(mcs->RunType==Runin)
//    {
//    	OnRunin(motorID);
//    	return;
//    }

    if((sps->CurveType==Con) ||(sps->ConPulseCount>0 && mcs->RunPulseCounter>=sps->AccPulseCount 
		&& mcs->RunPulseCounter<sps->Acc_Con_Count))//有匀速段，且在匀速段
    {
        mcs->TimPeriod=SPara[motorID].ConPeriod;
    }
    else if(mcs->RunPulseCounter<sps->AccPulseCount)//在加速段
    {
        mcs->TimPeriod=GetAccPerFormTable(motorID);
    }
    else//减速段
    {
        mcs->TimPeriod=GetDecPerFormTable(motorID);
    }
	//插补加减速的第二种方式，改变预装值后再调用插补
//	if(mcs->RunType==Runin)
//	{
//		OnRunin(motorID);
//	}
}
