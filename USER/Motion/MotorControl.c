#include "MotorControl.h"
#include "Parameter.h"
#include "Timer.h"
#include "TCurve.h"
#include "LineRunin.h"

//������Ʋ����б�
MotorControlStruct MotorList[MotorPortNum];
//�㶯���в����б�
PTRunParaStruct PTRunPara[MotorPortNum];
//�㶯�����ٶ��б����ٶ�Ŀǰ���ã�����ٶ�һ��
//��һ��������ʾ�������С��죻�ڶ���������ʾ�����ٶȡ����ٶȡ����ٶ�
//const float _ptSpeed[3][3]={{10,50,50},{50,250,250},{100,1000,1000}};
const float _ptSpeed[3][3]={{50,800,800},{400,2000,2000},{500,5000,5000}};
//��λ��־������㶯��ԭ����ϵͳ��λ��ì�ܵ�����
bool ResetFlag= false;

//��һ�����ID�õ�����ַ���
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

//��һ������ַ��õ�������ö��,������ǵ���ַ�����9999
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

//���õ����ת����
void SetMotorDirect(MotorIDEnum motorID, DirectEnum direct)
{
	MotorList[motorID].Direct = direct;
	if(direct == ZFX)
	{
		GPIO_SetBits(MotorPortList[motorID].DirectPort, MotorPortList[motorID].DirectPin);//����DIR
	}		
	else
	{
		GPIO_ResetBits(MotorPortList[motorID].DirectPort, MotorPortList[motorID].DirectPin);
	}	
}

//�����ʼ��,�����е�����г�ʼ��
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

//�㶯�������ã�һ�����ǳ��ȣ���ת��ʱ�ǽǶ�
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

//ԭ������λ�жϴ�����
void OriginLimitFunc(MotorIDEnum motorID)
{
	if (!GetInPortState(MotorPortList[motorID].LimitPort) && MotorList[motorID].Direct == FFX)
	{		
		MotorList[motorID].LimitOrigin = true;
		if(ResetFlag)//�ֶ���������Ҳ���������㣬���ǿ졢�С���������Ӱ��
			MotorList[motorID].RealPulse = (s32)(-1*Parameter.OffsetZeroPoint[motorID]/Parameter.PulseK[motorID]);
	}
}

//����г���λ�жϲ�����,����չΪ���ģʽ
void MaxLengLimitFunc(MotorIDEnum motorID)
{
	if(MotorList[motorID].RealPulse>MotorList[motorID].MaxSteps)
		MotorList[motorID].LimitMaxLeng = true; 
}

//���ָ���ĵ���ܷ���
bool CheckMotorIsCanRun(MotorIDEnum motorID,DirectEnum direct)
{
	MotorControlStruct* mcs=&MotorList[motorID];
	if(mcs->IsRunning || (mcs->LimitOrigin && direct==FFX)|| (mcs->LimitMaxLeng && direct==ZFX))
		return false;
	return true;
}

//һ���������ǰ��׼������
bool MotorRunIniState(MotorIDEnum motorID,s32 pulseNum)
{
	if(pulseNum==0)
		return false;
	DirectEnum direct=ZFX;
	if(pulseNum<0)
		direct=FFX;
	if(!CheckMotorIsCanRun(motorID,direct))//��������������
		return false;
	SetMotorDirect(motorID,direct);	
	MotorList[motorID].RunPulseNum=abs(pulseNum);
	MotorList[motorID].RunPulseCounter=0;
	MotorList[motorID].IsRunning=true;	
	return true;
}

//������к�����motorID:Ҫ���еĵ���ţ�pulseNum��Ҫ�ߵ������������ű�ʾ��ת����
//runSpeed�������ٶ�;accSpeed�����ٶ�
void MotorRun_OutSpeed(MotorIDEnum motorID,s32 pulseNum,float runSpeed,float accSpeed)
{	
	if(!MotorRunIniState(motorID,pulseNum))
		return;
	MotorList[motorID].RunType=SingleMotor;
	CreateCurve(motorID,MotorList[motorID].RunPulseNum,runSpeed,accSpeed);
	PWM_TIMControl(motorID,ENABLE);	
}

//�����������ٶ����ã�����ͬʱ���У����Ч��
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

//ʹָ���ĵ���������С�motorID:Ҫ���еĵ���ţ�pulseNum��Ҫ�ߵ������������ű�ʾ��ת����
//�����ٶ�Ϊ�������õ��ٶȼ��ٷֱ�
//���Զ�����ͬʱ���У�Ϊ���Ż�ִ��Ч�ʣ�2018.04.02
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

//ָֹͣ�����,Ҫ��Ƽ���ֹͣ
void MotorStop(MotorIDEnum motorID)
{
	//��Ҫ���ϴ��жϣ����򵱵��ֹͣʱ���ô˱�־���´��ƶ�����Ϊֹͣ��־������
	if(MotorList[motorID].IsRunning)
		MotorList[motorID].StopTag=true;
}

//�����ʼ�㶯
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

//���ֹͣ�㶯
void MotorPTStop(MotorIDEnum motorID)
{
	MotorList[motorID].PTRunTag=false;
}

