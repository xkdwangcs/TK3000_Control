/*
*系统级参数，出厂之后一般不改变的参数。但是注册码要变化
*/
#include "SysParameter.h"
#include "FileOperate.h"
#include "SST25V.h"
#include "PublicFunction.h"

//系统参数实例
SysParameterStruct SysParameter;
//初始化参数实例
SysParameterStruct _iniSysParameter;
//系统参数信息存储在最后一个扇区
u16 _sysParaSection=FLASH_SECTOR_COUNT+9;
char _superPass[]="super";

//初始化系统参数值
void IniSysParameterValue(void)
{
	strcpy(_iniSysParameter.DevicePass, "666666");
    strcpy(_iniSysParameter.ParaVer,"ver1.03"); 
}

//初始化系统参数
void IniSysParameter(void)
{
	IniSysParameterValue();
	ReadBytes(_sysParaSection,(u8 *)&SysParameter,0,sizeof(SysParameter));
	if(!StrCMP_n(_iniSysParameter.ParaVer,SysParameter.ParaVer,7))//判断是否要升级参数
	{
		WriteBytes(_sysParaSection,(u8 *) &_iniSysParameter,sizeof(_iniSysParameter));
		ReadBytes(_sysParaSection,(u8 *) &SysParameter,0,sizeof(SysParameter));	
	}	
	IniControlMPMsg();
}

//把系统参数写入存储器
void WriteSysParameter(void)
{
	WriteBytes(_sysParaSection,(u8*)&SysParameter,sizeof(SysParameter));
}   

/********控制器铭牌，客户不能修改的*******/	
//控制器铭牌实例
ControlMPStruct ControlMPMsg;
ControlMPStruct _iniControlMPMsg;
u16 _ctMPMsgSection=FLASH_SECTOR_COUNT+8;
//初始化系统参数值
void IniControlMPValue(void)
{
	strcpy(_iniControlMPMsg.ProdName, "螺丝自动锁付控制系统");
	strcpy(_iniControlMPMsg.ProdModel, "TK2000");
    strcpy(_iniControlMPMsg.ProdID,"18041200");
	strcpy(_iniControlMPMsg.SoftVer,"V4.00");
	strcpy(_iniControlMPMsg.HardWareVer,"V1.2");
	strcpy(_iniControlMPMsg.ProduDate, "2018-04-12");
	strcpy(_iniControlMPMsg.Tel, "18569081180");
	strcpy(_iniControlMPMsg.URL, "http://www.tkrobot.net");
	strcpy(_iniControlMPMsg.Company, "长沙拓控智能科技有限公司");
	strcpy(_iniControlMPMsg.CompanyAddr, "湖南省长沙市经济技术开发区时代星城1栋3205号");
    strcpy(_iniControlMPMsg.ParaVer,"ver1.20"); 
}

//初始化控制器铭牌
void IniControlMPMsg(void)
{
	IniControlMPValue();
	ReadBytes(_ctMPMsgSection,(u8 *)&ControlMPMsg,0,sizeof(ControlMPMsg));
	if(!StrCMP_n(_iniControlMPMsg.ParaVer,ControlMPMsg.ParaVer,7))//判断是否要升级参数
	{
		WriteBytes(_ctMPMsgSection,(u8 *) &_iniControlMPMsg,sizeof(_iniControlMPMsg));
		ReadBytes(_ctMPMsgSection,(u8 *) &ControlMPMsg,0,sizeof(ControlMPMsg));	
	}
}

//把控制器铭牌入存储器
void WriteControlMPMsg(void)
{
	WriteBytes(_ctMPMsgSection,(u8*)&ControlMPMsg,sizeof(ControlMPMsg));
}

