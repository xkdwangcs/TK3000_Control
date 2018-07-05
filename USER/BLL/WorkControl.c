#include "WorkControl.h"
#include "AppTasks.h"
#include "AxisLinkage.h"
#include "main.h"
#include "WorkControlBlow.h"
#include "WorkControlSuck.h"
#include "WorkControlOut.h"
#include "WorkControlCustomMove.h"
#include "MainForm.h"
#include "WorkDataFile.h"

//�豸��ǰ״̬
DeviceStatusEnum _devCurrStatus;

//���������У�Ҫ��ʾ��������������
ShowDataStruct _showDatas;
u16 _machiningCount = 0; //�ӹ�������N�θ�λ��
LeftRightPlatformEnum _currPlatformWork = LeftPlatform; 	//��ǰ���������ƽ̨
AxisIDEnum _currYAxis=Y1; 									//��ǰ�����Ӧ��Y��
WorkTaskStartParaStruct* _taskStartPara; 				//���ⲿ������������������������

bool _isStartJS=false;//һ�������Ƿ��Ѿ���ʼ��ʱ������Ť���������ڿ�ʼ��ʱ���������
u32 _startWorkTime;//һ��ƽ̨��ʼ����˿ʱ��
u32 _oneScrewStartTime;//һ����˿��ʼ���ʱ��
u32 _oneScrewCompleTime;//һ����˿��ɵ�ʱ����ms
LinkageMotorStruct _linkages[3];

//��������˿ǰ�����״̬�ļ�顣
//����trueΪͨ���������ʾ�������й�������
bool CheckStateBeforeWork(void);
//ȡ��˿���̺���
bool GetScrew(void);
//����˿����
bool ClampScrew(CoordStruct destCoord);

char _msgBuff[30];
//��ʾ��Ϣ����ƽ̨��Ϣ
void ShowWorkMsg_Plat(LeftRightPlatformEnum platform,char* msg)
{
	memset(_msgBuff, 0, sizeof(_msgBuff));
	if(Parameter.ProdType==DoublePlatform)
	{
		if(platform==LeftPlatform )
			sprintf(_msgBuff,"%s%s","��ƽ̨",msg);
		else
			sprintf(_msgBuff,"%s%s","��ƽ̨",msg);
		ShowWorkMsg(_msgBuff);
	}
	else
	{
		ShowWorkMsg(msg);
	}
}

//�򿪵���
void OpenScrewDriver()
{
	SetOutPortState(ScrewDriverPort,true);
}

//��ʾ����˿ʱ��
void ShowWorkTime(void)
{
	_showDatas.TotalTime=GetTimekeeperValue()-_startWorkTime;
	_showDatas.AvgTime=_showDatas.TotalTime/_showDatas.CurrScrewIndex;
	ShowWorkData(); 
}

//����������صĶ���
void TestCoordMove(void)
{
	ShowWorkMsg("�������ԣ�");
	if(Parameter.GetScrewType==SuckScrew)//����˿ 
		TestCoordMoveSuck();
	else if(Parameter.GetScrewType==BlowScrew)//����˿
		TestCoordMoveBlow();
	else if(Parameter.GetScrewType==OutScrew)//����˿
		TestCoordMoveOut();
}

//����ͬʱ��λ
void MultiAxisPositionReset()
{
	ShowDebugMsg("�Ḵλ");
	AxisIDEnum zAxis[1]={Z};
	MultiAxisMoveToPositionZeroAndMove(zAxis,1);
	
	AxisIDEnum axiss[3]={X,Y1,Y2};
	MultiAxisMoveToPositionZeroAndMove(axiss,3);
}

//�Զ���λ��N�θ�λ��
void AutoReset(void)
{
	_machiningCount++;
	//�ж�N�θ�λ
	if(_machiningCount >= Parameter.AutoResetNum)
	{
		_machiningCount=0;
		ShowWorkMsg("N���Զ���λ...");
		MultiAxisPositionReset();
	}	
}

void SystemReset(void)
{    
    for(u8 i=0;i<OutPortNum;i++)
	{
		bool bl=GetOutPortState_Index(i);
		*(__IO uint8_t *) (OutputStateAddrBase + i)=bl;
	}
    *(__IO uint8_t *) (PermissionAddr)=_currPermission;//����Ȩ��
	__disable_fault_irq(); 
	NVIC_SystemReset();
}

//�رձ���
void CloseAlarm(void)
{
	ShowDebugMsg("�رձ���");
	EnableAlarmTwinkle(false);//����˿�����в��ܼ�ͣ�رմ����񣬻������ʱ����
	SetOutPortState(AlarmSundPort,false);
	SetOutPortState(AlarmLampPort,false);
}

//�򿪱������������õĲ��������Ʊ����Ƽ�������
void OpenAlarm()
{
	ShowDebugMsg("�򿪱���");
	SetOutPortState(ScrewDriverPort,false);//�ص���
	if(Parameter.LockScrewType==QGDown)
		SetOutPortState(LockQGPort,false); //�ر�Z������
	ZMoveToCoord(Parameter.WorkHeight);//�ƶ��������߶��ٱ���
	SetOutPortState(AlarmLampPort,true); //����������
	if(Parameter.AlarmLampMode==Twinkle)
	{
		EnableAlarmTwinkle(true);
	}
	//LCDBeep(500);	
	SetOutPortState(AlarmSundPort,true);//����������
	Delay_s_OS(Parameter.AlarmBuzzerTime);
	SetOutPortState(AlarmSundPort,false);//�رձ�����	
}

//��������˿ǰ�����״̬�ļ�顣
//����trueΪͨ���������ʾ�������й�������
bool CheckStateBeforeWork(void)
{
	//��������������򲻽����κζ���,������һ����
	ShowDebugMsg("��������ǰ����ؼ��");
//	if(_currFormIndex!=1 && _currFormIndex!=71 && _currFormIndex!=23)
//		return false;
	if(Parameter.GetScrewPara.Coord.X<1)
	{
        char* msg="ȡ�������쳣��X<1";
        //WriteLog(msg);
		ShowWorkMsg(msg);
		return false;
	}
	u16 coordCount=0;
	GetCoordList(_taskStartPara->CurrPlatform, &coordCount);
	if(coordCount<=0)
	{
        char* msg= "δ�趨����";
        //WriteLog(msg);
		ShowWorkMsg_Plat(_taskStartPara->CurrPlatform, msg);
		return false;
	}
	if(Parameter.IsWorkpieceCheck)//������ù������
	{
		IOPortStruct workpiecePort=LeftWorkpiecePort;
		if(_taskStartPara->CurrPlatform==RightPlatform)
			workpiecePort=RightWorkpiecePort;
		if(GetInPortState(workpiecePort)==0)
		{
			Delay_ms_OS(100);
			if(GetInPortState(workpiecePort)==0)
				return true;
		}
		char* msg= "δ��⵽������";
        //WriteLog(msg);
		ShowWorkMsg(msg);
		return false;
	}
	return true;
}

bool _isTorqueOK=false;//Ť���ź��Ƿ����յ�
//Ť���źż��
void CheckTorquePort(void)
{
	if(Parameter.FaultMode==IGNORE || !Parameter.IsGetScrew)//�����ȡ��
	{
		return;
	}
	if(!GetInPortState(TorquePort) && !_isTorqueOK)//��Ϊ�������Ҫ���߼�������ֹ�ظ����
	{
		ShowDebugMsg("�յ�Ť���ź�");
		if(_isStartJS)//����Ѿ���ʼ��ʱ
			CalcLockTime();
		else
			_oneScrewCompleTime=0;//����Ť���źŸտ�ʼ������
		_isTorqueOK=true;
		SetOutPortState(ScrewDriverPort,false);//�ص���
		StopAxisLinkageRun(_linkages,3);
	}
}

//ȡ��˿���̺���
bool GetScrew(void)
{
	ShowDebugMsg("׼��ȡ�ϣ�");
	if(Parameter.GetScrewType==SuckScrew)//����˿
		return GetScrewSuck();
	else if(Parameter.GetScrewType==BlowScrew)//����˿
		return GetScrewBlow(); 	
	else if(Parameter.GetScrewType==OutScrew)//����˿
		return GetScrewOut();
}

//��ʼ��ʱ
 void StartJS()
{
	ShowDebugMsg("��ʼ��ʱ");
	_isStartJS=true;
	_oneScrewStartTime=GetTimekeeperValue();//��ʼ��ʱ
}

//��������˿ʱ��
void CalcLockTime(void)
{
	_oneScrewCompleTime=GetTimekeeperValue()-_oneScrewStartTime;
}

//����˿����
bool ClampScrew(CoordStruct destCoord)
{	
	ShowDebugMsg("��ʼ����");
	if(Parameter.GetScrewType==SuckScrew)
		return ClampScrewSuck(destCoord);
	else if(Parameter.GetScrewType==BlowScrew)//����˿
		return ClampScrewBlow(destCoord);
	else if(Parameter.GetScrewType==OutScrew)//����˿
		return ClampScrewOut(destCoord);	
}

LeftRightPlatformEnum _yReadyPlat=LeftPlatform;
void YReadyTask(void)
{
	LeftRightPlatformEnum plat=_yReadyPlat;//*(LeftRightPlatformEnum*)arg;
	AxisIDEnum yAxis=Y1;
	u16 coordCount=0;
	CoordStruct* coordList = GetCoordList(plat, &coordCount);
	if(coordCount<1)
		return;
	if(plat==RightPlatform)
	{
		yAxis=Y2;
	}
	ShowWorkMsg_Plat(plat, "����׼��״̬");
	MoveToCoord(yAxis,coordList[0].Y,false);//�ƶ�����ƽ̨Y��ĵ�һ������		
}

bool _isWorkHG=true;//���ι����Ƿ�ϸ�
bool _isWaitGetWorkpiece=false;//�Ƿ��ѵȴ�ȡ��
//һ��ƽ̨��ɺ��ͳ�ƹ�����
void WorkCompletedStatistics(void)
{
	if(!_isWorkHG)
	{
		WorkData.TotalStatistics.unQualifiedNumber++;
		WorkData.TotalStatistics.qualifiedNumber--;
		WriteWorkData();
	}
	ShowWorkTime();
	StopTimekeeper();		
	_taskStartPara->WorkTaskState=TaskNoRun;		
	ContinueNextPlatWork(_taskStartPara->CurrPlatform);	
}

//������˿�󣬵ȴ�ȡ����Ҫ�����˹�����⡢ǰ�ź�ȡʱ������ 
void WaitGetWorkpiece(LeftRightPlatformEnum platform)
{
	if(Parameter.IsWorkpieceCheck && Parameter.QFMode==QFHQ)//������ù�����⡢ǰ�ź�ȡ
	{
		_isWaitGetWorkpiece=true;
		WorkCompletedStatistics();
		ShowWorkMsg_Plat(_taskStartPara->CurrPlatform, "�ȴ�ȡ��");
		ContinueNextPlatWork(_taskStartPara->CurrPlatform);
		IOPortStruct workpiecePort;//��������������
		if(platform==LeftPlatform)
		{
			XYLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.AxisLength[Y1]); //X����ȡ�ϵ㣬Y��������г̴�
			workpiecePort=LeftWorkpiecePort;
		}
		else
		{
			XY2LinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.AxisLength[Y2]); //X����ȡ�ϵ㣬Y��������г̴�
			workpiecePort=RightWorkpiecePort;
		}
        
		while(true)//�ȴ�ȡ����
		{
			if(GetInPortState(workpiecePort))//�յ�ȡ������ź�
			{
				Delay_ms_OS(300);
				if(GetInPortState(workpiecePort))
					break;
			}
		}
		Delay_s_OS(Parameter.GainWorkpieceDelayTiem);//ȡ����ʱ
	}	
}

//���ϼ�⣬���ߡ�����
bool GZJC_FGFY(void)
{	
	if(Parameter.IsGetScrew)//���ȡ�ϣ�����ж��Ƿ���Ť��
	{
		ShowDebugMsg("���ϼ���ж�");
		u32 fyMaxTime=999999;
		if(!_isTorqueOK)//����Z���ѵ�Ŀ�����꣬�ڶ���������δ��⵽��Ť��
		{
			float nlDelay=Parameter.NLDelay*1000;
			u16 delayCount=(u16)(nlDelay/10);			
			for(u16 i=0;i<delayCount;i++)
			{
				Delay_ms_OS(10);
				if(!GetInPortState(TorquePort))
				{
					_isTorqueOK=true;
					SetOutPortState(ScrewDriverPort,false);//���ص�������ֹɨͷ
					break;
				}
			}
			if(GetInPortState(TorquePort) && !_isTorqueOK)//�������Ť���ź�
			{
				_oneScrewCompleTime=fyMaxTime;//�趨һ����ֵ�����ں�����жϡ�ʵ��ֵ�����ܴ��ڴ�ֵ
			}
			else
			{
				CalcLockTime();
			}		
		}
		if(Parameter.FaultMode==WAIT)//������ϵȴ�
		{
			char msg[20]={0};
			bool isFG=false;
			if(_oneScrewCompleTime<Parameter.ScrewTimeDown)//С���趨��ʱ�����ޣ�����
			{
				isFG=true;
				sprintf(msg,"���ߣ�T=%d(ms)",_oneScrewCompleTime);		
				ShowWorkMsg(msg);
				if(Parameter.IsFGBJ) //�����Ƿ񱨾�
				{
					//WriteLog(msg);
					OpenAlarm();
					return false;
				}
				return true;
			}

			if(!isFG && _oneScrewCompleTime>Parameter.ScrewTimeUP)//�����趨��ʱ�����ޣ�����
			{
				if(_oneScrewCompleTime==fyMaxTime)
				{
					strcpy(msg,"��������Ť���ź�!");
				}
				else
				{
					sprintf(msg,"������T=%d(ms)",_oneScrewCompleTime);	
				}						
				ShowWorkMsg(msg);
				if(Parameter.IsFYBJ) //�����Ƿ񱨾�
				{
					//WriteLog(msg);
					OpenAlarm();
					return false;
				}
				return true;
			}
			else if(!isFG) //������Χ��OK
			{
				sprintf(msg,"����������T=%d(ms)",_oneScrewCompleTime);			
				ShowWorkMsg(msg);
				return true;
			}
		}			
	}
	return true;
}

//����˿��������
void LockScrewTask(void *arg)
{
	SetCurrStatus(DevWorking,"�豸���ڹ�����");
	_taskStartPara =(WorkTaskStartParaStruct*)arg;
	_taskStartPara->WorkTaskState=TaskRunning;
	if(!CheckStateBeforeWork())
	{
		_taskStartPara->WorkTaskState=TaskNoRun;
		return;//continue;
	}
	if(_taskStartPara->CurrPlatform==LeftPlatform)
	{
		_currPlatformWork=LeftPlatform;
		_currYAxis=Y1;
	}
	else
	{
		_currPlatformWork=RightPlatform;
		_currYAxis=Y2;
	}
	u16 coordCount=0;
	CoordStruct* coordList = GetCoordList(_currPlatformWork, &coordCount);
	bool currWorkState=true;//����״̬��trueʱΪ������falseʱΪ����
	StartTimekeeper();
	_startWorkTime=GetTimekeeperValue();//��ʼ����˿ʱ��ʱ��
	if(_taskStartPara->StartSource==FJQD)
		Delay_s_OS(Parameter.StartWorkDelay);		
	ZMoveToCoord(Parameter.WorkHeight);
	if(_currPlatformWork==LeftPlatform)
		WorkBeforMove_Left();
	else
		WorkBeforMove_Right();
	_isWorkHG=true;//�������Ƿ�ϸ�
	//��Ҫ�����Ҫ�ۼӸ���
	WorkData.DayStatistics.totalNumber++; 
	WorkData.TotalStatistics.totalNumber++;
	WorkData.TotalStatistics.qualifiedNumber++;
	WriteWorkData();
	ShowWorkTime();
	for (u16 coordIndex = 0; coordIndex < coordCount; )//Ϊʲô�ֲ��������У�coordIndex++���ǵ���0
	{		
		CoordStruct currCoord=coordList[coordIndex];
		_showDatas.CurrScrewIndex =coordIndex+1;
		ShowWorkMsg_Plat(_currPlatformWork,"�����ӹ���...");
		ShowWorkData();
		currWorkState = GetScrew();//ȡ��˿����   

BreakLoop: 
		if(!currWorkState)//����й��ϣ��ȴ��û���Ӧ
		{
			_isWaitSemaphore=true;
			while(true)
			{
				//if(xSemaphoreTake(xSemaphoreRedo, portMAX_DELAY) == pdTRUE)
				//��Ϊ0ʱ�������� ���ź���������ѯ
				if(xSemaphoreTake(_xSemaphoreRedo, 0) == pdTRUE)
				{
					CloseAlarm();
					ShowWorkMsg_Plat(_currPlatformWork,"������˿��");
					break;
				}
				if(xSemaphoreTake(_xSemaphoreNext, 0) == pdTRUE)
				{
					coordIndex++;
					CloseAlarm();
					ShowWorkMsg_Plat(_currPlatformWork,"����һ����˿��");
					_isWorkHG=false;
					break;
				}
				Delay_ms_OS(200);
				ShowWorkTime();
			}
			continue;
		}
		_isStartJS=false;//��ʼ��ʱ��ʾ�÷�
		_oneScrewCompleTime=2;//��˿ʱ����λ����������
		currWorkState =ClampScrew(currCoord);//����˿���� 
		if(!currWorkState) 
			goto BreakLoop;
		coordIndex++;
		ShowWorkTime();
	}	

	//��������е������
	if(_currPlatformWork==LeftPlatform)
		WorkCompletedMove_Left();
	else
		WorkCompletedMove_Right();
	if(!_isWaitGetWorkpiece)
	{
		WorkCompletedStatistics();
	}
	SetCurrStatus(DevReady,"�豸׼������");
	ShowWorkMsg_Plat(_currPlatformWork,"�ӹ���ϣ�");	
}
	
//һ��ƽ̨(�����)��ʼ����˿����ں���
void WorkRun(void *arg)
{
	for(;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  
		if(Parameter.ProdType!=CustomMovePlat)
		{
			LockScrewTask(arg);
		}
		else
		{
			RunCustomMove(arg);
		}
	}
}
