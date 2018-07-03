#include "WorkControl.h"
#include "WorkControlSuck.h"
#include "AppTasks.h"
#include "AxisLinkage.h"
#include "MainForm.h"

//ȡ��˿���̺���
bool GetScrew(void);
//����˿����
bool ClampScrew(CoordStruct destCoord);

//����յ�ŷ�
void OpenVacuumValve()
{
	SetOutPortState(VacuumValvePort,true);
}

//�ر���յ�ŷ�
void CloseVacuumValve()
{
	SetOutPortState(VacuumValvePort,false);
}

//����������صĶ���
void TestCoordMoveSuck(void)
{
	u16 leftCoordCount=0;
	CoordStruct* leftCoordList = GetCoordList(LeftPlatform, &leftCoordCount);	
	u16 rightCoordCount=0;
	CoordStruct* rightCoordList = GetCoordList(RightPlatform, &rightCoordCount);	
	while(true)
	{
		ZMoveToCoord(Parameter.WorkHeight);		
		for (int i = 0; i < leftCoordCount;i++ )//��ƽ̨
		{		
			XZLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.GetScrewPara.Coord.Z,NULL,0);	//ȡ�ϵ�
			XYZLinkageToCoord(leftCoordList[i].X, leftCoordList[i].Y, leftCoordList[i].Z,NULL,0);//Ŀ���
		}	
		XYZLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.GetScrewPara.Coord.Y, Parameter.WorkHeight,NULL,0); //�ż�λ�ã�X����ȡ�ϵ�,Y������λ,Z���ڹ����߶�

		if(Parameter.ProdType==SinglePlatform)
		  continue;
		for (int i = 0; i < rightCoordCount;i++ )//��ƽ̨
		{		
			XZLinkageToCoord(Parameter.GetScrewPara.Coord.X,Parameter.GetScrewPara.Coord.Z,NULL,0);	//ȡ�ϵ�
			XY2ZLinkageToCoord(rightCoordList[i].X, rightCoordList[i].Y, rightCoordList[i].Z,NULL,0);//Ŀ���
		}			
		XY2ZLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.GetScrewPara.Coord.Y, Parameter.WorkHeight,NULL,0); //�ż�λ�ã�X����ȡ�ϵ�,Y������λ,Z���ڹ����߶�
	}
}

bool _isScrewInhaleOK=true;//��ʽ����˿ǰ����ȡ�Ƿ�����
//��˿��ȡ������񣬴���ȡ�굽����˿ǰ��Ҫִ��
void ScrewInhaleCheck(void)
{
	if(Parameter.FaultMode==IGNORE || !(Parameter.IsScrewCheck && Parameter.IsGetScrew))//�����ȡ�ϻ���ռ��
	{
		//_isScrewInhaleOK=true;//����Ҫ�ڴ˸�ֵ
		return;
	}
	if(GetInPortState(ScrewInhalePort) && _isScrewInhaleOK)	
	{
		_isScrewInhaleOK=false;
		char* msg="��ȡ�쳣,������ź�!";
		//WriteLog(msg);
		ShowWorkMsg(msg);
		StopAxisLinkageRun(_linkages,3);
	}
}

//ȡ��˿���̺���
bool GetScrewSuck(void)
{
	//����˿׼���ź�
	if (GetInPortState(ScrewRedyPort) && Parameter.FaultMode==WAIT && Parameter.IsGetScrew)
	{
		char* msg= "��˿��׼���쳣!";
    //WriteLog(msg);
		ShowWorkMsg(msg);
		OpenAlarm();
		return false;
	}
	//�ƶ������ȡ��λ��
	if(Parameter.IsGetScrew)
	{
		float openVacuumCoord=Parameter.GetScrewPara.Coord.Z-Parameter.ZKTQLength;//����յ�����
		AxisRunningActionStruct actions[1];
		CreateSingAction(&actions[0],Z,openVacuumCoord,OpenVacuumValve);
		XZLinkageToCoord(Parameter.GetScrewPara.Coord.X,Parameter.GetScrewPara.Coord.Z,actions,1);
	}
	else
	{
		XZLinkageToCoord(Parameter.GetScrewPara.Coord.X,Parameter.GetScrewPara.Coord.Z,NULL,0);
	}
	Delay_s_OS(Parameter.GetScrewPara.getDelayTime);
	return true;
}

//���ɡ����׹��ú���
bool TH_QGDownCommSuck(CoordStruct destCoord,float openScrewDriverCoord,float closeVacuumCoord,float startJSCoord)
{
	for(u8 i=0;i<3;i++)
	{		
		AxisRunningActionStruct xAction[1];
		CreateMultiAction(&xAction[0],X,-1,-1,ScrewInhaleCheck);
		
		AxisRunningActionStruct zUpAction[1];
		CreateMultiAction(&zUpAction[0],Z,-1,-1,ScrewInhaleCheck);
		AxisRunningActionStruct zDownAction[4];
		CreateSingAction(&zDownAction[0],Z,openScrewDriverCoord,OpenScrewDriver);	//����������
		CreateSingAction(&zDownAction[1],Z,startJSCoord,StartJS);	//��ʼ��ʱ����
		CreateSingAction(&zDownAction[2],Z,closeVacuumCoord,CloseVacuumValve);		//���������		
		CreateMultiAction(&zDownAction[2],Z,openScrewDriverCoord,destCoord.Z,CheckTorquePort);//�������Ϳ�ʼ���Ť��
		//CreateMultiAction(&zDownAction[3],Z,-1,-1,CheckTorquePort);	
		VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zDownAction,4,zUpAction,1};
			
		CreateLinkageOneAxis(&_linkages[0],X,destCoord.X,xAction,1,NULL);
		CreateLinkageOneAxis(&_linkages[1],_currYAxis,destCoord.Y,NULL,0,NULL);
		CreateLinkageOneAxis(&_linkages[2],Z,destCoord.Z,NULL,0,&zAxisLink);
		_isScrewInhaleOK=true;	//����źű�־
		_isTorqueOK=false;		//Ť���źű�־
		MultiAxisLinkage(_linkages,3);
		if(_isScrewInhaleOK)
		{
			break;
		}
		else
		{
			SetOutPortState(VacuumValvePort,false);//����յ�ŷ�
			ZMoveToCoord(Parameter.WorkHeight);
			GetScrew();
		}
	}
	if(!_isScrewInhaleOK)//��������ȡ�󻹲��������򱨾�
	{
		SetOutPortState(VacuumValvePort,false);//����յ�ŷ�
		SetOutPortState(ScrewDriverPort,false);//�ص���
		OpenAlarm();
		return false;
	}
	return true;
}

//����˿����
bool ClampScrewSuck(CoordStruct destCoord)
{	
	float _clampZSpeed = Parameter.ScrewLockSpeed;//����˿�ٶȣ�Z����ѹģʽʱ��
	float openScrewDriverCoord = destCoord.Z - Parameter.DPTQLength- Parameter.ScrewPara.length;//��������λ��
	float startJSCoord = destCoord.Z - Parameter.ScrewPara.length;//��ʼ��ʱ��λ��,������ѹģʽʱ��
	float closeVacuumCoord = destCoord.Z - Parameter.ScrewPara.length /2 ;//����յ�λ��
	if(Parameter.LockScrewType==THDown)//������ѹ����˿ģʽ
	{
		TH_QGDownCommSuck(destCoord,openScrewDriverCoord,closeVacuumCoord,startJSCoord);
	}
	else if(Parameter.LockScrewType==QGDown)//������ѹ����˿ģʽ
	{
		TH_QGDownCommSuck(destCoord,openScrewDriverCoord,closeVacuumCoord,startJSCoord);
		if(Parameter.IsGetScrew)
		{
			SetOutPortState(LockQGPort,true);//��Z������		
			float nlDelay=Parameter.LockQGDelay*1000;//Z��������ʱ
			u32 delayCount=(u32)(nlDelay/10);			
			for(u32 i=0;i<delayCount;i++)
			{
				Delay_ms_OS(10);
				if(!GetInPortState(TorquePort))
				{					
					SetOutPortState(ScrewDriverPort,false);//�ص���
					SetOutPortState(LockQGPort,false); //�ر�Z������
					_isTorqueOK=true;
					CalcLockTime();
					break;
				}
			}
		}
	}
	else  if(Parameter.LockScrewType==MotorDown) //Z������ѹ����˿ģʽ
	{
		for(u8 i=0;i<3;i++)
		{
			AxisRunningActionStruct xAction[1];
			CreateMultiAction(&xAction[0],X,-1,-1,ScrewInhaleCheck);
			
			AxisRunningActionStruct zUpAction[1];
			CreateMultiAction(&zUpAction[0],Z,-1,-1,ScrewInhaleCheck);
			AxisRunningActionStruct zDownAction[2];
			CreateSingAction(&zDownAction[0],Z,openScrewDriverCoord,OpenScrewDriver);	//����������
			CreateSingAction(&zDownAction[1],Z,closeVacuumCoord,CloseVacuumValve);		//���������
			VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zDownAction,2,zUpAction,1};
				
			CreateLinkageOneAxis(&_linkages[0],X,destCoord.X,xAction,1,NULL);
			CreateLinkageOneAxis(&_linkages[1],_currYAxis,destCoord.Y,NULL,0,NULL);
			CreateLinkageOneAxis(&_linkages[2],Z,destCoord.Z- Parameter.ScrewPara.length,NULL,0,&zAxisLink);
			_isScrewInhaleOK=true;
			MultiAxisLinkage(_linkages,3);
			if(_isScrewInhaleOK)
			{
				break;
			}
			else
			{
				SetOutPortState(VacuumValvePort,false);//����յ�ŷ�
				ZMoveToCoord(Parameter.WorkHeight);
				GetScrew();
			}
		}
		if(!_isScrewInhaleOK)//��������ȡ�󻹲��������򱨾�
		{
			SetOutPortState(VacuumValvePort,false);//����յ�ŷ�
			SetOutPortState(ScrewDriverPort,false);//�ص���
			OpenAlarm();
			return false;
		}
		SetOutPortState(ScrewDriverPort,true);//����������ֹ������ǰΪ0ʱ������û�д�
		if(Parameter.IsGetScrew)
		{
			_isTorqueOK=false;
			StartJS();
			AxisRunningActionStruct actions1[1];
			CreateMultiAction(&actions1[0],Z,-1,-1,CheckTorquePort);		
			MoveToCoordRunActionPara(Z,destCoord.Z,actions1,1,10*_clampZSpeed,_clampZSpeed);		
		}
		else
		{
			MoveToCoordRunActionPara(Z,destCoord.Z,NULL,0,10*_clampZSpeed,_clampZSpeed);
		}
	}	
	SetOutPortState(VacuumValvePort,false);//����յ�ŷ�
	
	bool bl=GZJC_FGFY();//�����ж�
	SetOutPortState(ScrewDriverPort,false);//�ص���
	if(Parameter.LockScrewType==QGDown)//������ѹ����˿ģʽ
		SetOutPortState(LockQGPort,false); //�ر�Z������
	return bl;
}
	
