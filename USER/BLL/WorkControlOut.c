#include "WorkControl.h"
#include "WorkControlOut.h"
#include "AppTasks.h"
#include "AxisLinkage.h"
#include "main.h"

//����������صĶ���
void TestCoordMoveOut(void)
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
bool GetScrewOut(void)
{
	return true;
}

//���ɡ����׹��ú���
void TH_QGDownCommOut(CoordStruct destCoord,float openScrewDriverCoord,float startJSCoord)
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
	MultiAxisLinkage(_linkages,3);
}

//����˿����
bool ClampScrewOut(CoordStruct destCoord)
{	
	float _clampZSpeed = Parameter.ScrewLockSpeed;//����˿�ٶȣ�Z����ѹģʽʱ��
	float openScrewDriverCoord = destCoord.Z - Parameter.DPTQLength- Parameter.ScrewPara.length;//��������λ��
	float startJSCoord = destCoord.Z - Parameter.ScrewPara.length;//��ʼ��ʱ��λ��,������ѹģʽʱ��
	if(Parameter.LockScrewType==THDown)//������ѹ����˿ģʽ
	{
		TH_QGDownCommOut(destCoord,openScrewDriverCoord,startJSCoord);
	}
	else if(Parameter.LockScrewType==QGDown)//������ѹ����˿ģʽ
	{
		TH_QGDownCommOut(destCoord,openScrewDriverCoord,startJSCoord);
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
	else //Z������ѹ����˿ģʽ
	{
		AxisRunningActionStruct zDownAction[1];
		CreateSingAction(&zDownAction[0],Z,openScrewDriverCoord,OpenScrewDriver);	//����������
		VerticalAxisLinkStruct zAxisLink={Parameter.WorkHeight,zDownAction,1,NULL,0};
			
		CreateLinkageOneAxis(&_linkages[0],X,destCoord.X,NULL,0,NULL);
		CreateLinkageOneAxis(&_linkages[1],_currYAxis,destCoord.Y,NULL,0,NULL);
		CreateLinkageOneAxis(&_linkages[2],Z,destCoord.Z- Parameter.ScrewPara.length,NULL,0,&zAxisLink);			
		MultiAxisLinkage(_linkages,3);

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

