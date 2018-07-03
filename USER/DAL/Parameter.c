/*
* Parameter.c
* ��ز�����д
*/
#include "Parameter.h"
#include "nvic.h"
#include "FileOperate.h"
#include "MemoryAlloc.h"
#include "AppTasks.h"

ParameterStruct Parameter __at(WorkParaAddr);
//��ʼ������ʵ��
ParameterStruct _iniParameter __at(DefaultParaAddr);
//��ǰȨ��
PermissionTypeEnum _currPermission=NoLogined;
//���������ļ�
char* _paraFileName="0:/CFG/Parameter.cfg";
//���������ļ�
char* _paraFileNameBK="0:/CFG/ParameterBK.cfg";

Dictionary _yesNoDic[2] = { { "��", "1" }, { "��", "0" } };

//��ʼ��д��
void IniParameterValue(void);

bool CheckIniIsOK(void)
{
	u32 leng=sizeof(Parameter);
	for(u32 i=0;i<leng;i++)
	{
		u8* pt=(u8*)&Parameter;
		if((*pt++)!=0x00 && (*pt++)!=0xFF)
		{
			return true;
		}			
	}
	return false;
	
//	if(Parameter.ProdName==0 && Parameter.ParaVer==0)
//		return false;
}

//��ʼ����������
bool IniParameter(void)
{	
	memset((u8*)&Parameter,0,sizeof(Parameter));
	IniParameterValue();
	bool isParaBKHave=true;//���������ļ��Ƿ����
	if(CreateFileOnDisk(_paraFileNameBK,false)!=FR_EXIST)//������������ļ�ԭ��������
	{
	   WriteBytesToFile(_paraFileNameBK,(u8*) &_iniParameter,0,sizeof(_iniParameter)); 
	   isParaBKHave=false;        
	}
	if(CreateFileOnDisk(_paraFileName,false)!=FR_EXIST)//��������ļ�ԭ��������
	{ 
		if(isParaBKHave)
		{
		   ReadBytesFormFile(_paraFileNameBK,(u8*) &Parameter,0,sizeof(Parameter)); 
		   WriteBytesToFile(_paraFileName,(u8*) &Parameter,0,sizeof(Parameter)); 
		}
		else
		{
		   WriteBytesToFile(_paraFileName,(u8*) &_iniParameter,0,sizeof(_iniParameter));  
		}
	}
	ReadBytesFormFile(_paraFileName,(u8*) &Parameter,0,sizeof(Parameter));
	if(!StrCMP_n(_iniParameter.ParaVer,Parameter.ParaVer,7))//�ж��Ƿ�Ҫ��������,Ҳ���������µ�������
	{
		WriteBytesToFile(_paraFileName,(u8*) &_iniParameter,0,sizeof(_iniParameter));
		WriteBytesToFile(_paraFileNameBK,(u8*) &_iniParameter,0,sizeof(_iniParameter));
		ReadBytesFormFile(_paraFileName,(u8*) &Parameter,0,sizeof(Parameter));        
	}
	return CheckIniIsOK();
}

EventHandlerNoPara _popOkDialog=NULL;
void WriteParameterCall(void)
{
	WriteBytesToFile(_paraFileName,(u8*) &Parameter,0,sizeof(Parameter));
    WriteBytesToFile(_paraFileNameBK,(u8*) &Parameter,0,sizeof(Parameter));
	if(_popOkDialog!=NULL)
		_popOkDialog();
} 
//��Parameter�ڴ�ֵд��洢��
void WriteParameter(EventHandlerNoPara popOkDialog)
{
	_popOkDialog=popOkDialog;
	_oneFatfsOperateFunc=WriteParameterCall;
	StartFATFSTask();
}   

//��ʼ������ֵ
void IniParameterValue(void)
{
	memset(&_iniParameter, 0, sizeof(_iniParameter));
	strcpy(_iniParameter.ProdName, ControlMPMsg.ProdName);
	strcpy(_iniParameter.ProdModel, ControlMPMsg.ProdModel);
    strcpy(_iniParameter.ProdID,ControlMPMsg.ProdID);
	strcpy(_iniParameter.ProduDate, ControlMPMsg.ProduDate);
	strcpy(_iniParameter.Tel, ControlMPMsg.Tel);
	strcpy(_iniParameter.URL, ControlMPMsg.URL);
	strcpy(_iniParameter.Company, ControlMPMsg.Company);
	strcpy(_iniParameter.CompanyAddr, ControlMPMsg.CompanyAddr);
	
	_iniParameter.DeviceAddr = 0;
	_iniParameter.Baud = 115200;    
	_iniParameter.CommChannel=RS232;
	//��˿������ ��С�ٶ�2.56��С�ڸ�ֵ����
	for(u8 i=0;i<MotorPortNum;i++)
	{
		_iniParameter.PulseK[i] = 0.009375;
		_iniParameter.RunSpeed[i] = 1000;
		_iniParameter.AccSpeed[i] = 30000;
		_iniParameter.DecSpeed[i] = 30000;
		_iniParameter.LimitMode[i] = 0;
		_iniParameter.OffsetZeroPoint[i] = 2;
		_iniParameter.AxisLength[i] = 320;
	}
	_iniParameter.LinkageSafeSmooth=10;
	_iniParameter.Y1MoveCoord= _iniParameter.AxisLength[AxisList[Y1].MotorID];
	_iniParameter.Y2MoveCoord= _iniParameter.AxisLength[AxisList[Y2].MotorID];
	_iniParameter.LockScrewType=MotorDown;
	_iniParameter.GetScrewType=SuckScrew;//Ĭ��Ϊ����˿
	_iniParameter.IsShowDebugMsg=false;

	_iniParameter.WorkHeight = 0;
	_iniParameter.ScrewLockSpeed= 50;
	_iniParameter.ScrewPara.length = 6;
	_iniParameter.ScrewPara.TimeDLimit=2;
	_iniParameter.ScrewPara.TimeULimit=4;
	_iniParameter.GetScrewPara.Coord.X = 30;
	_iniParameter.GetScrewPara.Coord.Y = 0;
	_iniParameter.GetScrewPara.Coord.Z = 5;
	_iniParameter.GetScrewPara.heigth = 4;
	_iniParameter.GetScrewPara.getScrewSpeed = 10;
	_iniParameter.GetScrewPara.getDelayTime = 0.5;
	_iniParameter.ResetSpeed = 100; //mm/s
	_iniParameter.AutoResetNum = 10;
	_iniParameter.StartWorkDelay = 2;
	_iniParameter.FaultMode = 0;
	_iniParameter.IsGetScrew = false;
	_iniParameter.IsScrewCheck = false;
	_iniParameter.IsWorkpieceCheck = 0;
	_iniParameter.IsFJQD = 0;
	_iniParameter.QFMode = QFQQ;
	_iniParameter.AlarmLampMode = Keep;
	_iniParameter.AlarmBuzzerTime = 1;
	_iniParameter.GainWorkpieceDelayTiem=4;
	strcpy(_iniParameter.AdminiPass, "888888");
	strcpy(_iniParameter.UserPass, "0000");
	_iniParameter.ZKTQLength=5;
	_iniParameter.DPTQLength=5;
	_iniParameter.PTSpeed=80; //Ĭ��80%
	_iniParameter.WorkSpeedPercent=80;
	_iniParameter.ProdType = DoublePlatform;
	_iniParameter.IsFGBJ = true;
	_iniParameter.IsFYBJ = true; 
	_iniParameter.NLDelay=0.5;
	_iniParameter.ScrewTimeDown=200;
	_iniParameter.ScrewTimeUP=500;
	_iniParameter.LockQGDelay=0.5;
	_iniParameter.PushDelay=0.2;							//���Ͽ�����ʱ
	_iniParameter.BlowDelay=0.2;							//���Ͽ�����ʱ
    strcpy(_iniParameter.ParaVer,"ver1.02");
}

//�ָ���������
void WritDefaultPara(void)
{
	IniParameterValue();
     WriteBytesToFile(_paraFileName,(u8*) &_iniParameter,0,sizeof(_iniParameter));
     WriteBytesToFile(_paraFileNameBK,(u8*) &_iniParameter,0,sizeof(_iniParameter));
     ReadBytesFormFile(_paraFileName,(u8*) &Parameter,0,sizeof(Parameter)); 
}

//����ȡ������
void SaveGainScrewCoord(CoordStruct getScrewCoord)
{
	Parameter.GetScrewPara.Coord.X = getScrewCoord.X;
	Parameter.GetScrewPara.Coord.Y= getScrewCoord.Y;
	Parameter.GetScrewPara.Coord.Z = getScrewCoord.Z;
	_popOkDialog=NULL;
	WriteParameterCall();
	//GetGainScrewCoord();
}

//��ȡȡ�������ĳһ������
float GetScrewOneAxisCoord(AxisIDEnum axisID)
{
	if(axisID==X) return Parameter.GetScrewPara.Coord.X;
	if(axisID==Y1 || axisID==Y2) return Parameter.GetScrewPara.Coord.Y;
	if(axisID==Z) return Parameter.GetScrewPara.Coord.Z;
}

//��ȡ��ǰ�û�Ȩ��
PermissionTypeEnum GetCurrPermission(void)
{
	return _currPermission;
}

//��ȡָ��Ȩ�޵��ַ�˵��
char* GetPerStr(PermissionTypeEnum per)
{
	switch(per)
	{
		case NoLogined:
			return "δ��¼";
		case SuperPer:
			return "����Ȩ��";
		case DevicePer:
			return "�豸����Ȩ��";
		case AdminiPer:
			return "����ԱȨ��";
		case UserPer:
			return "һ���û�Ȩ��";		
	}
}

//��ȡ��ǰȨ�޵��ַ�˵��
char* GetCurrPerStr(void)
{
	return GetPerStr(_currPermission);
}

//��ǰ¼���Ȩ���жϡ�destPer��Ŀ��Ȩ��
bool JudgePermission(char* passWord,PermissionTypeEnum destPer)
{
	if(StrCMP(_superPass,passWord))
	{
		_currPermission=SuperPer;
	}
	else if(StrCMP(SysParameter.DevicePass,passWord))
	{
		_currPermission=DevicePer;
	}
	else if(StrCMP(Parameter.AdminiPass,passWord))
	{
		_currPermission=AdminiPer;
	}
	else if(StrCMP(Parameter.UserPass,passWord))
	{
		_currPermission=UserPer;
	}
	else
	{
		_currPermission=NoLogined;
	}	
	if(_currPermission>=destPer)
		return true;
	return false;
	//return _currPermission;
}

//��ǰ¼���Ȩ���ж�
PermissionTypeEnum JudgePermission_bk(char* passWord)
{
	if(StrCMP(_superPass,passWord))
	{
		_currPermission=SuperPer;
	}
	else if(StrCMP(SysParameter.DevicePass,passWord))
	{
		_currPermission=DevicePer;
	}
	else if(StrCMP(Parameter.AdminiPass,passWord))
	{
		_currPermission=AdminiPer;
	}
	else if(StrCMP(Parameter.UserPass,passWord))
	{
		_currPermission=UserPer;
	}
	else
	{
		_currPermission=NoLogined;
	}	
	return _currPermission;
}

//�жϴ����Ƿ�δ��ʽ��������ǣ����ʽ��
void PDFormatFlash(void)
{
	char* fn = "0:/temp.txt";	
	char* diskStr=GetDiskStrByFullPath(fn);   
	bool isFormat=false;
	u32 reindex;
    FIL file;
	FATFS fs;
	u8 res = f_mount(&fs,diskStr,0);
	res = f_open(&file,fn,FA_READ | FA_WRITE | FA_CREATE_NEW);
	if(res==FR_NO_FILESYSTEM || res==FR_INVALID_DRIVE)
	{	
       isFormat=true;					
	}
	else if(res==FR_OK || res==FR_EXIST)
	{
		res = f_close(&file);
	}
	res = f_mount(NULL, diskStr, 0);	
	if(isFormat)
	{
		FormatDisk(SPI_FLASH); //��ʽ����ĵ�һ����ʾ���������ڶ��ξͿ�����
		//SystemReset();   //������������û��
	}
}

