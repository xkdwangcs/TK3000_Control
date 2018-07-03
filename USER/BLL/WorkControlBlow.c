#include "WorkControl.h"
#include "WorkControlBlow.h"
#include "AppTasks.h"
#include "AxisLinkage.h"
#include "main.h"

//����������صĶ���
void TestCoordMoveBlow(void)
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
			XYZLinkageToCoord(leftCoordList[i].X, leftCoordList[i].Y, leftCoordList[i].Z,NULL,0);//Ŀ���
		}	
		XYZLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.GetScrewPara.Coord.Y, Parameter.WorkHeight,NULL,0); //�ż�λ�ã�X����ȡ�ϵ�,Y������λ,Z���ڹ����߶�

		if(Parameter.ProdType==SinglePlatform)
		  continue;
		for (int i = 0; i < rightCoordCount;i++ )//��ƽ̨
		{					
			XY2ZLinkageToCoord(rightCoordList[i].X, rightCoordList[i].Y, rightCoordList[i].Z,NULL,0);//Ŀ���
		}			
		XY2ZLinkageToCoord(Parameter.GetScrewPara.Coord.X, Parameter.GetScrewPara.Coord.Y, Parameter.WorkHeight,NULL,0); //�ż�λ�ã�X����ȡ�ϵ�,Y������λ,Z���ڹ����߶�
	}
}


//ȡ��˿���̺���
bool _screwIsOK=false;//�����Ƿ�ɹ�
u32 _blowTimeStart=0;//��ʼ�������ڵļ�ʱ��ֵ
bool GetScrewBlow(void)
{
	if(Parameter.IsGetScrew)
	{
		_screwIsOK=false;
		SetOutPortState(BlowMaterPort,true);//�򿪴��Ͽ���		
		_blowTimeStart=GetTimekeeperValue();
	}
	return true; 
}

//���ϼ��
bool BlowCheck(void)
{
//	_screwIsOK=true;//ģ�����ϳɹ�
//	return true;
	if(Parameter.IsGetScrew)
	{
		ShowDebugMsg("���ϼ��");
		u32 blowDelayms=Parameter.BlowDelay*1000;
		while(true)
		{			
			u32 blowTimeLeng=GetTimekeeperValue()-_blowTimeStart;
			if(!GetInPortState(ScrewInhalePort))//����д��ϳɹ��ź�
			{
				_screwIsOK=true;
				break;
			}
			if(blowTimeLeng>=blowDelayms)
			{				
				break;
			}
		}
		SetOutPortState(BlowMaterPort,false);//�رմ��Ͽ���		
		if(!_screwIsOK)	//û����˿
		{
			char* msg="��˿�����쳣��";
			//ShowWorkMsg(msg);
			if(Parameter.FaultMode==WAIT)
			{
				//WriteLog(msg);
				OpenAlarm();
				return false;
			}
		}
		return true;
	}
	return true;
}

//���ɡ����׹��ú���
bool TH_QGDownCommBlow(CoordStruct destCoord,float openScrewDriverCoord,float startJSCoord)
{
	AxisRunningActionStruct zDownAction[3];
	CreateSingAction(&zDownAction[0],Z,openScrewDriverCoord,OpenScrewDriver);	//����������
	CreateSingAction(&zDownAction[1],Z,startJSCoord,StartJS);	//��ʼ��ʱ����
	CreateMultiAction(&zDownAction[2],Z,openScrewDriverCoord,destCoord.Z,CheckTorquePort);//�������Ϳ�ʼ���Ť��
	//CreateMultiAction(&zDownAction[2],Z,-1,-1,CheckTorquePort);
	VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zDownAction,3,NULL,0};
		
	CreateLinkageOneAxis(&_linkages[0],X,destCoord.X,NULL,0,NULL);
	CreateLinkageOneAxis(&_linkages[1],_currYAxis,destCoord.Y,NULL,0,NULL);
	CreateLinkageOneAxis(&_linkages[2],Z,destCoord.Z,NULL,0,&zAxisLink);
	_isTorqueOK=false;		//Ť���źű�־
	ShowDebugMsg("��������������");
	MultiAxisLinkage(_linkages,3);
	
	if(Parameter.IsGetScrew)
	{
		if(!BlowCheck())
			return false;
	}
	SetOutPortState(BlowMaterPort,false);//�رմ��Ͽ���
	return true;
}

//����˿����
bool ClampScrewBlow(CoordStruct destCoord)
{		
	float _clampZSpeed = Parameter.ScrewLockSpeed;//����˿�ٶȣ�Z����ѹģʽʱ��
	float openScrewDriverCoord = destCoord.Z - Parameter.DPTQLength- Parameter.ScrewPara.length;//��������λ��
	float startJSCoord = destCoord.Z - Parameter.ScrewPara.length;//��ʼ��ʱ��λ��,������ѹģʽʱ��
	if(Parameter.LockScrewType==THDown)//������ѹ����˿ģʽ
	{
		bool bl=TH_QGDownCommBlow(destCoord,openScrewDriverCoord,startJSCoord);
		if(!bl)	return false;
	}
	else if(Parameter.LockScrewType==QGDown)//������ѹ����˿ģʽ
	{
		bool bl = TH_QGDownCommBlow(destCoord,openScrewDriverCoord,startJSCoord);
		if(!bl)	return false;
		if(Parameter.IsGetScrew)
		{
			ShowDebugMsg("����������");
			SetOutPortState(LockQGPort,true);//��Z������		
			float nlDelay=Parameter.LockQGDelay*1000;//Z��������ʱ
			u32 delayCount=(u32)(nlDelay/10);			
			for(u32 i=0;i<delayCount;i++)
			{
				Delay_ms_OS(10);
				if(!GetInPortState(TorquePort))
				{		
					ShowDebugMsg("��⵽Ť��!�ص�������������");					
					SetOutPortState(ScrewDriverPort,false);//�ص���
					SetOutPortState(LockQGPort,false); //�ر�Z������
					//Delay_ms_OS(800);  //Ϊʲô���������ᵼ�¿��٣����㽭�ͻ�����ʱ��
					_isTorqueOK=true;
					CalcLockTime();
					break;
				}
			}
		}		
	}
	else //Z������ѹ����˿ģʽ
	{
		AxisRunningActionStruct zDownAction[1];
		CreateSingAction(&zDownAction[0],Z,openScrewDriverCoord,OpenScrewDriver);	//����������
		VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zDownAction,1,NULL,0};
			
		CreateLinkageOneAxis(&_linkages[0],X,destCoord.X,NULL,0,NULL);
		CreateLinkageOneAxis(&_linkages[1],_currYAxis,destCoord.Y,NULL,0,NULL);
		CreateLinkageOneAxis(&_linkages[2],Z,destCoord.Z- Parameter.ScrewPara.length,NULL,0,&zAxisLink);			
		MultiAxisLinkage(_linkages,3);
		if(!BlowCheck())
			return false;
		SetOutPortState(ScrewDriverPort,true);//����������ֹ������ǰΪ0ʱ������û�д�
		if(Parameter.IsGetScrew)
		{
			_isTorqueOK=false;
			StartJS();//��ʼ��ʱ
			AxisRunningActionStruct actions1[1];
			CreateMultiAction(&actions1[0],Z,-1,-1,CheckTorquePort);		
			MoveToCoordRunActionPara(Z,destCoord.Z,actions1,1,10*_clampZSpeed,_clampZSpeed);		
		}
		else
		{
			MoveToCoordRunActionPara(Z,destCoord.Z,NULL,0,10*_clampZSpeed,_clampZSpeed);
		}
	}
	
	bool bl=GZJC_FGFY();//�����ж�
	SetOutPortState(ScrewDriverPort,false);//�ص���
	if(Parameter.LockScrewType==QGDown)//������ѹ����˿ģʽ
		SetOutPortState(LockQGPort,false); //�ر�Z������
	return bl;
}

