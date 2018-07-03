/*
*ϵͳ������������֮��һ�㲻�ı�Ĳ���������ע����Ҫ�仯
*/
#include "SysParameter.h"
#include "FileOperate.h"
#include "SST25V.h"
#include "PublicFunction.h"

//ϵͳ����ʵ��
SysParameterStruct SysParameter;
//��ʼ������ʵ��
SysParameterStruct _iniSysParameter;
//ϵͳ������Ϣ�洢�����һ������
u16 _sysParaSection=FLASH_SECTOR_COUNT+9;
char _superPass[]="super";

//��ʼ��ϵͳ����ֵ
void IniSysParameterValue(void)
{
	strcpy(_iniSysParameter.DevicePass, "666666");
    strcpy(_iniSysParameter.ParaVer,"ver1.03"); 
}

//��ʼ��ϵͳ����
void IniSysParameter(void)
{
	IniSysParameterValue();
	ReadBytes(_sysParaSection,(u8 *)&SysParameter,0,sizeof(SysParameter));
	if(!StrCMP_n(_iniSysParameter.ParaVer,SysParameter.ParaVer,7))//�ж��Ƿ�Ҫ��������
	{
		WriteBytes(_sysParaSection,(u8 *) &_iniSysParameter,sizeof(_iniSysParameter));
		ReadBytes(_sysParaSection,(u8 *) &SysParameter,0,sizeof(SysParameter));	
	}	
	IniControlMPMsg();
}

//��ϵͳ����д��洢��
void WriteSysParameter(void)
{
	WriteBytes(_sysParaSection,(u8*)&SysParameter,sizeof(SysParameter));
}   

/********���������ƣ��ͻ������޸ĵ�*******/	
//����������ʵ��
ControlMPStruct ControlMPMsg;
ControlMPStruct _iniControlMPMsg;
u16 _ctMPMsgSection=FLASH_SECTOR_COUNT+8;
//��ʼ��ϵͳ����ֵ
void IniControlMPValue(void)
{
	strcpy(_iniControlMPMsg.ProdName, "��˿�Զ���������ϵͳ");
	strcpy(_iniControlMPMsg.ProdModel, "TK2000");
    strcpy(_iniControlMPMsg.ProdID,"18041200");
	strcpy(_iniControlMPMsg.SoftVer,"V4.00");
	strcpy(_iniControlMPMsg.HardWareVer,"V1.2");
	strcpy(_iniControlMPMsg.ProduDate, "2018-04-12");
	strcpy(_iniControlMPMsg.Tel, "18569081180");
	strcpy(_iniControlMPMsg.URL, "http://www.tkrobot.net");
	strcpy(_iniControlMPMsg.Company, "��ɳ�ؿ����ܿƼ����޹�˾");
	strcpy(_iniControlMPMsg.CompanyAddr, "����ʡ��ɳ�о��ü���������ʱ���ǳ�1��3205��");
    strcpy(_iniControlMPMsg.ParaVer,"ver1.20"); 
}

//��ʼ������������
void IniControlMPMsg(void)
{
	IniControlMPValue();
	ReadBytes(_ctMPMsgSection,(u8 *)&ControlMPMsg,0,sizeof(ControlMPMsg));
	if(!StrCMP_n(_iniControlMPMsg.ParaVer,ControlMPMsg.ParaVer,7))//�ж��Ƿ�Ҫ��������
	{
		WriteBytes(_ctMPMsgSection,(u8 *) &_iniControlMPMsg,sizeof(_iniControlMPMsg));
		ReadBytes(_ctMPMsgSection,(u8 *) &ControlMPMsg,0,sizeof(ControlMPMsg));	
	}
}

//�ѿ�����������洢��
void WriteControlMPMsg(void)
{
	WriteBytes(_ctMPMsgSection,(u8*)&ControlMPMsg,sizeof(ControlMPMsg));
}

