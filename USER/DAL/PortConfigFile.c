#include "PortConfigFile.h"
#include "string.h"
#include "FileOperate.h"
#include "PublicFunction.h"

char* _portFileName="0:/CFG/PortMotor.cfg";//����˿ڼ�����˿������ļ���
PortMotorCFGStruct PortMotorCFG;//����˿ڡ�����˿�ʵ��
PortMotorCFGStruct _iniPortMotorCFG;//����˿ڡ�����˿�ʵ��

//��ʼ��Ĭ�ϵĶ˿������ļ�
void IniDefaultPortCFG(void)
{	
	memset(&_iniPortMotorCFG,0,sizeof(_iniPortMotorCFG));
	/***********�ض�������˿�***********/
	_iniPortMotorCFG.PortCFG[0].Outport=&ScrewDriverPort;
	strcpy(_iniPortMotorCFG.PortCFG[0].PortStr,"�������ƶ˿�");
	_iniPortMotorCFG.PortCFG[0].PortIndex=0;
	
	_iniPortMotorCFG.PortCFG[1].Outport=&VacuumValvePort;
	strcpy(_iniPortMotorCFG.PortCFG[1].PortStr,"��յ�ŷ�");
	_iniPortMotorCFG.PortCFG[1].PortIndex=2;
	
	_iniPortMotorCFG.PortCFG[2].Outport=&AlarmSundPort;
	strcpy(_iniPortMotorCFG.PortCFG[2].PortStr,"�������˿�");
	_iniPortMotorCFG.PortCFG[2].PortIndex=3;
	
	_iniPortMotorCFG.PortCFG[3].Outport=&AlarmLampPort;
	strcpy(_iniPortMotorCFG.PortCFG[3].PortStr,"�����ƶ˿�");
	_iniPortMotorCFG.PortCFG[3].PortIndex=4;
	
	_iniPortMotorCFG.PortCFG[4].Outport=&LockQGPort;
	strcpy(_iniPortMotorCFG.PortCFG[4].PortStr,"�������׶˿�");
	_iniPortMotorCFG.PortCFG[4].PortIndex=13;
	
//	_iniPortMotorCFG.PortCFG[5].Outport=&PushMaterPort;
//	strcpy(_iniPortMotorCFG.PortCFG[5].PortStr,"���Ͽ��ƶ˿�");
//	_iniPortMotorCFG.PortCFG[5].PortIndex=10;
	
	_iniPortMotorCFG.PortCFG[5].Outport=&BlowMaterPort;
	strcpy(_iniPortMotorCFG.PortCFG[5].PortStr,"���Ͽ��ƶ˿�");
	_iniPortMotorCFG.PortCFG[5].PortIndex=12;
	
	/***********����˿�***********/
	//��������ŵ�ö��ֵ��һ���ģ����Կ�����forѭ����ֵ
//	for(u8 i=0;i<AxisNum;i++)
//	{
//		_iniPortMotorCFG.MotorCFG[i].AxixID=(AxisIDEnum)i;
//		_iniPortMotorCFG.MotorCFG[i].MotorID=(MotorIDEnum)i;
//	}
	
	_iniPortMotorCFG.MotorCFG[0].AxixID=X;
	_iniPortMotorCFG.MotorCFG[0].MotorID=M1;
	
	_iniPortMotorCFG.MotorCFG[1].AxixID=Y1;
	_iniPortMotorCFG.MotorCFG[1].MotorID=M2;
	
	_iniPortMotorCFG.MotorCFG[2].AxixID=Y2;
	_iniPortMotorCFG.MotorCFG[2].MotorID=M4;
	
	_iniPortMotorCFG.MotorCFG[3].AxixID=Z;
	_iniPortMotorCFG.MotorCFG[3].MotorID=M3;
	
	strcpy(_iniPortMotorCFG.ParaVer,"ver1.05");
}

//��ʼ���˿������ļ�
void IniPortCFGFile(void)
{
	memset((u8*)&PortMotorCFG,0,sizeof(PortMotorCFG));
	IniDefaultPortCFG();
	if(CreateFileOnDisk(_portFileName,false)!=FR_EXIST)//����ļ�ԭ��������
    { 		
		WriteBytesToFile(_portFileName,(u8*) &_iniPortMotorCFG,0,sizeof(PortMotorCFG));  
    }
	ReadBytesFormFile(_portFileName,(u8*) &PortMotorCFG,0,sizeof(PortMotorCFG));
	if(!StrCMP_n(_iniPortMotorCFG.ParaVer,PortMotorCFG.ParaVer,7))//�ж��Ƿ�Ҫ��������
	{
		WriteBytesToFile(_portFileName,(u8*) &_iniPortMotorCFG,0,sizeof(PortMotorCFG)); 
		ReadBytesFormFile(_portFileName,(u8*) &PortMotorCFG,0,sizeof(PortMotorCFG));		
	}
	for(u8 i=0;i<FixPortNum;i++)
	{
		//PortMotorCFG.PortCFG[i].Outport->Port=OutPortList[PortMotorCFG.PortCFG[i].PortIndex].Port;
		//PortMotorCFG.PortCFG[i].Outport->Pin=OutPortList[PortMotorCFG.PortCFG[i].PortIndex].Pin;b
		CopyBytes((u8*)PortMotorCFG.PortCFG[i].Outport,0,(u8*)&OutPortList[PortMotorCFG.PortCFG[i].PortIndex],sizeof(OutPortList[0]));
	}
}

//����˿������ļ�
void SavePortConfig(void)
{
	WriteBytesToFile(_portFileName,(u8*) &PortMotorCFG,0,sizeof(PortMotorCFG));  
}

//�ָ�����Ĭ�ϵĶ˿������ļ�
void WritDefaultPortCFG(void)
{
	IniDefaultPortCFG();
	WriteBytesToFile(_portFileName,(u8*) &PortMotorCFG,0,sizeof(PortMotorCFG));
	for(u8 i=0;i<FixPortNum;i++)
	{
		PortMotorCFG.PortCFG[i].Outport=&OutPortList[PortMotorCFG.PortCFG[i].PortIndex];
	}	
}


