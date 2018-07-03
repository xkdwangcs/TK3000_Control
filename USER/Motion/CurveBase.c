#include "CurveBase.h"
#include "SCurve_Table.h"
//#include "SCurve_Table_NoZIP.h"
//#include "SCurve_Real.h"
#include "TCurve.h"
#include "Timer.h"
#include "MemoryAlloc.h"

float _vs = 5;//mm/s
AccDecCurveTypeEnum _curveType=SCurve_Five_Table;//TCurve;//SCurve_Five_Table//SCurve_Five_Real//SCurve_Five_Table_NOZIP; 

//��������������һ�����߲�����
void CreateCurve(MotorIDEnum motorID, u32 pulseCount,float runSpeed,float accSpeed)
{
	if(_curveType==SCurve_Five_Table)
		CreateSTable(motorID,pulseCount,runSpeed,accSpeed);	 			
	else
		CreateTCurve(motorID,pulseCount,runSpeed,accSpeed);
}

//PWM��ʱ���жϵ���
void PWM(MotorIDEnum motorID)
{
	if(_curveType==SCurve_Five_Table)
		PWM_ST(motorID);
	else
		PWM_T(motorID);
}

//ֹͣ������ڲ�����
void StopMotor_in(MotorIDEnum motorID)
{
	MotorControlStruct* mcs=&MotorList[motorID];
	mcs->IsRunning=false;	
	mcs->StopTag=false;
	mcs->PTRunTag=false;
	mcs->RunMode=NormalRun;
	PWM_TIMControl(motorID,DISABLE);
//	if(mcs->RunType==SameTimeGoto && mcs->PriSecMotor==PrimaryMotor)
//	{
//		OnPrimaryMotorStoped(motorID);
//	}
}

