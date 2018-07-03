#include "TCurve.h"
#include "Timer.h"
#include "Parameter.h"
#include "Timekeeper.h"

float _clk_preK;
u16 _conPeriod[MotorPortNum]; //����Ԥװֵ
float AccTList[MotorPortNum];
float DecTList[MotorPortNum];
float _currMaxSpeed[MotorPortNum];//�ɼ��ٶȾ���һ��ʱ��󵽴����������ٶȣ���һ�����ڲ������õ��ٶ�
CurveParaStruct CurveList[MotorPortNum];//���߲����б�

//��������������һ�����߲�����
void CreateTCurve(MotorIDEnum motorID, u32 pulseCount,float runSpeed,float accSpeed)
{	
	MotorControlStruct* mcs=&MotorList[motorID];
	mcs->ConSpeed=runSpeed;
	mcs->AccSpeed=accSpeed;
	mcs->DecSpeed=accSpeed;
	float pulseK=Parameter.PulseK[motorID];
	CurveParaStruct* cps=&CurveList[motorID];
	u32 timeCLK=MotorPortList[motorID].TimCLK;//��ʱ������Ƶ��
	u16 timePrescaler=MotorPortList[motorID].Prescaler;
	_clk_preK=timeCLK/timePrescaler;
	
	if (runSpeed <= _vs * 3 || pulseCount < 10)//����ٶȺ�С�����������٣�ֻ�����ټ���
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
				u32 accCount=pulseIndex+1;//���ٶ�������				
				if(pulseCount>2*accCount)//���������ٶ�
				{		
					cps->CurveType=Acc_Con_Dec;					
					cps->AccStartIndex=0;
					cps->AccEndIndex=accCount-1;
					
					u32 conCount=pulseCount-2* accCount;//���ٶ�������
					cps->ConStartIndex=cps->AccEndIndex+1;
					cps->ConEndIndex=accCount+conCount-1;
					
					cps->DecStartIndex=cps->ConEndIndex+1;
					cps->DecEndIndex=pulseCount-1;
				}			
				else//û�����ٶ�
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
	if(cps->CurveType!=Con)//�����ȫΪ����
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

//��ȡָ������ٶ�Ԥװֵ,�岹�Ӽ��ٵĵ�һ�ַ�ʽ
u16 GetCurrPeriodValue_TCurve_Out(MotorIDEnum motorID,u32 currPulseIndex)
{
	CurveParaStruct* cps=&CurveList[motorID];
	u16 currPeriod=0;
	if((cps->CurveType==Con) ||
	   (cps->CurveType==Acc_Con_Dec && currPulseIndex>=cps->ConStartIndex && currPulseIndex<=cps->ConEndIndex))//�����ٶΣ��������ٶ�
	{
		currPeriod=_conPeriod[motorID];
	}
	else if(currPulseIndex>=cps->AccStartIndex && currPulseIndex<=cps->AccEndIndex)//�ڼ��ٶ�
	{
		currPeriod=CalcAccPeriodValue(motorID);
	}
	else//���ٶ�
	{
		currPeriod=CalcDecPeriodValue(motorID);
	}
	return currPeriod;	
}

//�㶯����
void PTRunProc(MotorIDEnum motorID)
{
	MotorControlStruct* mcs=&MotorList[motorID];
	CurveParaStruct* cps=&CurveList[motorID];
	if(!mcs->PTRunTag)//�㶯ֹͣ
	{
		mcs->RunMode=NormalRun;
		if(cps->CurveType==Con)//���ֻ�����٣�����ͣ
		{
			StopMotor_in(motorID);
		}
		else//�������ֹͣ
		{
			u32 decCount=cps->DecEndIndex-cps->DecEndIndex+1;
			cps->DecStartIndex=mcs->RunPulseCounter;
			cps->DecEndIndex=cps->DecStartIndex+decCount-1;
			mcs->RunPulseCounter=cps->DecStartIndex;
		}		
	}
	else //�㶯����
	{
		if(cps->CurveType!=Con && mcs->RunPulseCounter<=cps->AccEndIndex)//�����ȫ�����٣����ڼ��ٶ�
		{
			mcs->TimPeriod=CalcAccPeriodValue(motorID);
		}
		else
		{
			mcs->TimPeriod=_conPeriod[motorID];
		}
	}
}

//T�μӼ���PWM
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
	
	//�ߵ�������λ����ֹͣ
	if((mcs->LimitOrigin && mcs->Direct==FFX)||(mcs->LimitMaxLeng && mcs->Direct==ZFX))
	{
		StopMotor_in(motorID);
	}	
	
	//�㶯���д���
	if(mcs->RunMode==PTRun)
	{
		PTRunProc(motorID);
		return;
	}
	
	//�Ƿ���������������
//	if(mcs->RunPulseCounter>=mcs->RunPulseNum)
//	{
//		StopMotor_in(motorID);
//	}
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
		if(mcs->RunPulseCounter<cps->DecStartIndex) 
		{
			mcs->RunPulseCounter=cps->DecStartIndex;
		}		
	}
		
	//�岹�Ӽ��ٵĵ�һ�ַ�ʽ���ڲ岹���иı�Ԥװֵ
//	if(mcs->RunType==Runin)
//	{
//		OnRunin(motorID);
//		return;
//	}	
	if((cps->CurveType==Con) ||(cps->CurveType==Acc_Con_Dec && mcs->RunPulseCounter>=cps->ConStartIndex 
		&& mcs->RunPulseCounter<=cps->ConEndIndex))//�����ٶΣ��������ٶ�
	{
		mcs->TimPeriod=_conPeriod[motorID];
	}
	else if(mcs->RunPulseCounter>=cps->AccStartIndex && mcs->RunPulseCounter<=cps->AccEndIndex)//�ڼ��ٶ�
	{
		mcs->TimPeriod=CalcAccPeriodValue(motorID);
	}
	else//���ٶ�
	{
		mcs->TimPeriod=CalcDecPeriodValue(motorID);
	}			
	//�岹�Ӽ��ٵĵڶ��ַ�ʽ���ı�Ԥװֵ���ٵ��ò岹
//	if(mcs->RunType==Runin)
//	{
//		OnRunin(motorID);
//	}
}
