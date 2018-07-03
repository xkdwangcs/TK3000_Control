#include "Timer.h"
#include "Parameter.h"
#include "MemoryAlloc.h"
#include "math.h"
#include "PublicFunction.h"
#include "SCurve_Table.h"
//#include "RuninBase.h"

//�Ѳ����б�Ԥװֵ�б�ֿ��������ٶ�
AccPerValueStruct AccPerList[MotorPortNum][_accPerPoints]__at(MotionCurveBuff);		//���ٶ�Ԥװֵ�б�
STableParaStruct SPara[MotorPortNum];		//���ٶβ����б�

//const u16 _perBuffLeng=100; //˫���泤��
//u8 _currWriteBuff=1;		//��ǰд���������
//u8 _currReadBuff=1;			//��ǰ�����������
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

//�������ٶ��б�
void CreateSTable(MotorIDEnum motorID, u32 pulseCount,float runSpeed,float accSpeed)
{
    float pulseK=Parameter.PulseK[motorID];
    float L = pulseCount*pulseK;
    float dt = 0.001; 		//ʱ����ɢ
    float J = accSpeed * 5; //�Ӽ��ٶ�
    float vm=runSpeed;
    MotorControlStruct* mcs=&MotorList[motorID];
    AccPerValueStruct* accPerList = AccPerList[motorID];
    mcs->ConSpeed=runSpeed;
    mcs->AccSpeed=accSpeed;
    mcs->DecSpeed=accSpeed;

    u32 timeCLK=SystemCoreClock/2;//��ʱ������Ƶ��
    u16 timePrescaler=MotorPortList[motorID].Prescaler;
    float v_perK=timeCLK*pulseK/timePrescaler;
    STableParaStruct* sps=&SPara[motorID];
    sps->VPerK=v_perK;
    /*if (pulseCount < 100)//������������٣�ֻ�����ټ���
    {
        sps->CurveType=Con;
        sps->ConPulseCount=pulseCount;
        sps->ConPeriod=(u16)(v_perK / _vs);
        return;
    }*/

    float Tm = sqrt((vm - _vs) / J);
    float Sa = 2 * _vs * Tm + J * Tm * Tm * Tm;
    //float T3 = 0;
    if (L > 2 * Sa)//�����ٶ�
    {
        //T3 = (L - 2 * Sa) / vm;
        sps->CurveType=Acc_Con_Dec;
    }
    else
    {
        float k1 = L * sqrt(J) / 4;
        float k2 = sqrt(L * L * J / 16 + 8 * _vs * _vs * _vs / 27);
        float tk = Sign(k1 - k2) * pow(fabs(k1 - k2), 1.0 / 3); //Ҫע�⣬C�����е�abs������int,fabs���Ǹ�����
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
    sps->ConPulseCount=pulseCount- accCount-sps->DecPulseCount;//���ٶ�������
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
    /*else if(accCount < hasCount) //ǰ����ȷ��������
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
/// �ӱ��в�ѯָ����������Ԥװֵ
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
/// �ӱ��в�ѯָ����������Ԥװֵ
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

//�㶯����
void PTRunProc_S(MotorIDEnum motorID)
{
    MotorControlStruct* mcs=&MotorList[motorID];
    STableParaStruct* sps=&SPara[motorID];
    if(!mcs->PTRunTag)//�㶯ֹͣ
    {
        mcs->RunMode=NormalRun;
        if(sps->CurveType==Con)//���ֻ�����٣�����ͣ
        {
            StopMotor_in(motorID);
        }
        else//�������ֹͣ
        {
            mcs->RunPulseCounter=sps->Acc_Con_Count;
        }
    }
    else //�㶯����
    {
        if(sps->CurveType!=Con && mcs->RunPulseCounter<sps->AccPulseCount)//�����ȫ�����٣����ڼ��ٶ�
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

    //�ߵ�������λ����ֹͣ
    if((mcs->LimitOrigin && mcs->Direct==FFX)||(mcs->LimitMaxLeng && mcs->Direct==ZFX))
    {
        StopMotor_in(motorID);
    }

    //�㶯���д���
    if(mcs->RunMode==PTRun)
    {
        PTRunProc_S(motorID);
        return;
    }

    //�Ƿ���������������
/*  if(mcs->RunPulseCounter>=mcs->RunPulseNum)
    {
        StopMotor_in_S(motorID);
    }*/
	//�Ƿ��������������壬�岹ʱ���������ɲ岹�������ж�
	if(mcs->RunPulseCounter>=mcs->RunPulseNum && mcs->RunType!=Runin)
	{
		StopMotor_in(motorID);
	}	
    //����յ���ͣ��־����ͣ����
    if(mcs->StopTag)
    {
        //mcs->StopTag=false; //ֻ��һ��,����Ӵ˾�ֹͣЧ�������룬Ϊʲô??
        //����Ѿ��ڼ��ٶΣ��������ȴ���ɼ���
        //������ڼ��ٶ�,����������λ�����ٶ�
        if(mcs->RunPulseCounter<sps->Acc_Con_Count)
        {
            mcs->RunPulseCounter=sps->Acc_Con_Count;
        }
    }
	
	//�岹�Ӽ��ٵĵ�һ�ַ�ʽ���ڲ岹���иı�Ԥװֵ
//    if(mcs->RunType==Runin)
//    {
//    	OnRunin(motorID);
//    	return;
//    }

    if((sps->CurveType==Con) ||(sps->ConPulseCount>0 && mcs->RunPulseCounter>=sps->AccPulseCount 
		&& mcs->RunPulseCounter<sps->Acc_Con_Count))//�����ٶΣ��������ٶ�
    {
        mcs->TimPeriod=SPara[motorID].ConPeriod;
    }
    else if(mcs->RunPulseCounter<sps->AccPulseCount)//�ڼ��ٶ�
    {
        mcs->TimPeriod=GetAccPerFormTable(motorID);
    }
    else//���ٶ�
    {
        mcs->TimPeriod=GetDecPerFormTable(motorID);
    }
	//�岹�Ӽ��ٵĵڶ��ַ�ʽ���ı�Ԥװֵ���ٵ��ò岹
//	if(mcs->RunType==Runin)
//	{
//		OnRunin(motorID);
//	}
}
