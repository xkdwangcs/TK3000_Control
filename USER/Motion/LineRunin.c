#include "LineRunin.h"
#include "Parameter.h"
#include "Timer.h"
#include "TCurve.h"

//��һ�������һ������
//void SetOneMotorRun(MotorIDEnum motorID,u32 timePeriod)
//{
//	MotorList[motorID].TimPeriod=timePeriod;
//	
//}

u16 CalcTimePeriod(MotorIDEnum motorID,float speed)
{
	float pulseK=Parameter.PulseK[motorID];
	u32 timeCLK=MotorPortList[motorID].TimCLK;//SystemCoreClock/2;//��ʱ������Ƶ��
	u16 timePrescaler=MotorPortList[motorID].Prescaler;
	u32 _clk_preK=timeCLK/timePrescaler;
	float pSpeed=speed/pulseK;
	u16 timePeriod=(u16)(_clk_preK/pSpeed);	
	return timePeriod;
}


u16 period1;
u16 period2;
u32 ptList[MotorPortNum];
u32 pmList[MotorPortNum];
bool _isTimed[MotorPortNum]={true};//�Ƿ��Ѿ����������壨��ʱ���Ѿ���������

MotorIDEnum _motor1;//����X��
MotorIDEnum _motor2;//����Y��
//MotorIDEnum* _motorIDList=NULL;		//�岹�ĵ���б�
//u8 _cbNum=0;						//�岹�ĵ����

//����ֱ�߲岹���У�Ŀǰֻ֧����������ֱ�߲岹
void LineRuninRun_Motor(MotorIDEnum* motorIDs,s32* pulseList, u8 count,float runSpeed)
{	
	//_motorIDList=motorIDs;
	//_cbNum=count;
	_motor1=motorIDs[0];//����X��
	_motor2=motorIDs[1];//����Y��
	
	ptList[_motor1]=0;
	ptList[_motor2]=0;		
	pmList[_motor1]=abs(pulseList[0]-MotorList[_motor1].RealPulse);
	pmList[_motor2]=abs(pulseList[1]-MotorList[_motor2].RealPulse);
	period1=CalcTimePeriod(_motor1,runSpeed);
	period2=CalcTimePeriod(_motor2,runSpeed);
	PWM_TIMControl(_motor2,DISABLE);	
	TIMPeriodSet(_motor1,period1);
	PWM_TIMControl(_motor1,ENABLE);			
	ptList[_motor1]++;		

	/*while(true)
	{		
		if(ptList[motor1]>=pmList[motor1] && ptList[motor2]>=pmList[motor2])
		{			
			break;
		}
		s32 Fi=pmList[motor1]*ptList[motor2]-ptList[motor1]*pmList[motor2];
		if(Fi>=0 && ptList[motor1]<pmList[motor1]) //�����Ϸ���X����һ��
		{
			TIMPeriodSet(motor1,period1);
			PWM_TIMControl(motor1,ENABLE);			
			SetRealPulse(motor1);
			ptList[motor1]++;			
			_isTimed[motor1]=true;
			while(_isTimed[motor1]);
		}
		else if(ptList[motor2]<pmList[motor2])//Y����һ��
		{			
			TIMPeriodSet(motor2,period2);
			PWM_TIMControl(motor2,ENABLE);			
			SetRealPulse(motor2);
			ptList[motor2]++;			
			_isTimed[motor2]=true;
			while(_isTimed[motor2]);
		}
	}
	MotorList[motor1].IsRunning=false;	
	MotorList[motor2].IsRunning=false;*/	
}

void PWMProc(MotorIDEnum motorID)
{	
	MotorControlStruct* mcs=&MotorList[motorID];
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
	
	//�Ƿ���������������
	if(mcs->RunPulseCounter>=mcs->RunPulseNum)
	{
		StopMotor_in(motorID);
	}
	//����յ���ͣ��־����ͣ����
	if(mcs->StopTag)
	{
		//StopMotor_in(motorID);
		PWM_TIMControl(_motor1,DISABLE);
		PWM_TIMControl(_motor2,DISABLE);	
		MotorList[_motor1].IsRunning=false;	
		MotorList[_motor2].IsRunning=false;	
		return;
	}
	
	ptList[motorID]++;	
	if(ptList[_motor1]>=pmList[_motor1] && ptList[_motor2]>=pmList[_motor2])
	{			
		PWM_TIMControl(_motor1,DISABLE);
		PWM_TIMControl(_motor2,DISABLE);	
		MotorList[_motor1].IsRunning=false;	
		MotorList[_motor2].IsRunning=false;	
		return;
	}
	
	u32 k1=pmList[_motor1]*ptList[_motor2];
	u32 k2=ptList[_motor1]*pmList[_motor2];
	s32 Fi=(s32)(k1-k2);
	if(Fi>=0 && ptList[_motor1]<pmList[_motor1]) //�����Ϸ���X����һ��
	{
		PWM_TIMControl(_motor2,DISABLE);	
		TIMPeriodSet(_motor1,period1);
		PWM_TIMControl(_motor1,ENABLE);			
				
	}
	else if(ptList[_motor2]<pmList[_motor2])//Y����һ��
	{			
		PWM_TIMControl(_motor1,DISABLE);	
		TIMPeriodSet(_motor2,period2);
		PWM_TIMControl(_motor2,ENABLE);					
	}
}

//void PWMProc(MotorIDEnum motorID)
//{
//	//Ŀǰֻ��Ӧ������岹
//	MotorIDEnum motorOther;
//	for(u8 i=0;i<_cbNum;i++)
//	{
//		if(_motorIDList[i]!=motorID)
//		{
//			motorOther=_motorIDList[i];
//			break;
//		}
//	}
//	
//	if(ptList[motorID]>=pmList[motorID] && ptList[motorOther]>=pmList[motorOther])
//	{			
//		PWM_TIMControl(motorID,DISABLE);
//		PWM_TIMControl(motorOther,DISABLE);	
//		return;
//	}
//	
//	s32 Fi=pmList[motorID]*ptList[motorOther]-ptList[motorID]*pmList[motorOther];
//	if(Fi>=0 && ptList[motorID]<pmList[motorID]) //�����Ϸ���X����һ��
//	{
//		PWM_TIMControl(motorOther,DISABLE);	
//		TIMPeriodSet(motorID,period1);
//		PWM_TIMControl(motorID,ENABLE);			
//		SetRealPulse(motorID);
//		ptList[motorID]++;			
//	}
//	else if(ptList[motorOther]<pmList[motorOther])//Y����һ��
//	{			
//		PWM_TIMControl(motorID,DISABLE);	
//		TIMPeriodSet(motorOther,period2);
//		PWM_TIMControl(motorOther,ENABLE);			
//		SetRealPulse(motorOther);
//		ptList[motorOther]++;			
//	}
//}
