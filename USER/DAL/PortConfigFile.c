#include "PortConfigFile.h"
#include "string.h"
#include "FileOperate.h"
#include "PublicFunction.h"

char* _portFileName="0:/CFG/PortMotor.cfg";//输出端口及电机端口配置文件名
PortMotorCFGStruct PortMotorCFG;//输出端口、电机端口实例
PortMotorCFGStruct _iniPortMotorCFG;//输出端口、电机端口实例

//初始化默认的端口配置文件
void IniDefaultPortCFG(void)
{	
	memset(&_iniPortMotorCFG,0,sizeof(_iniPortMotorCFG));
	/***********特定的输出端口***********/
	_iniPortMotorCFG.PortCFG[0].Outport=&ScrewDriverPort;
	strcpy(_iniPortMotorCFG.PortCFG[0].PortStr,"电批控制端口");
	_iniPortMotorCFG.PortCFG[0].PortIndex=0;
	
	_iniPortMotorCFG.PortCFG[1].Outport=&VacuumValvePort;
	strcpy(_iniPortMotorCFG.PortCFG[1].PortStr,"真空电磁阀");
	_iniPortMotorCFG.PortCFG[1].PortIndex=2;
	
	_iniPortMotorCFG.PortCFG[2].Outport=&AlarmSundPort;
	strcpy(_iniPortMotorCFG.PortCFG[2].PortStr,"报警声端口");
	_iniPortMotorCFG.PortCFG[2].PortIndex=3;
	
	_iniPortMotorCFG.PortCFG[3].Outport=&AlarmLampPort;
	strcpy(_iniPortMotorCFG.PortCFG[3].PortStr,"报警灯端口");
	_iniPortMotorCFG.PortCFG[3].PortIndex=4;
	
	_iniPortMotorCFG.PortCFG[4].Outport=&LockQGPort;
	strcpy(_iniPortMotorCFG.PortCFG[4].PortStr,"锁付气缸端口");
	_iniPortMotorCFG.PortCFG[4].PortIndex=13;
	
//	_iniPortMotorCFG.PortCFG[5].Outport=&PushMaterPort;
//	strcpy(_iniPortMotorCFG.PortCFG[5].PortStr,"推料控制端口");
//	_iniPortMotorCFG.PortCFG[5].PortIndex=10;
	
	_iniPortMotorCFG.PortCFG[5].Outport=&BlowMaterPort;
	strcpy(_iniPortMotorCFG.PortCFG[5].PortStr,"吹料控制端口");
	_iniPortMotorCFG.PortCFG[5].PortIndex=12;
	
	/***********电机端口***********/
	//轴号与电机号的枚举值是一样的，所以可以用for循环赋值
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

//初始化端口配置文件
void IniPortCFGFile(void)
{
	memset((u8*)&PortMotorCFG,0,sizeof(PortMotorCFG));
	IniDefaultPortCFG();
	if(CreateFileOnDisk(_portFileName,false)!=FR_EXIST)//如果文件原来不存在
    { 		
		WriteBytesToFile(_portFileName,(u8*) &_iniPortMotorCFG,0,sizeof(PortMotorCFG));  
    }
	ReadBytesFormFile(_portFileName,(u8*) &PortMotorCFG,0,sizeof(PortMotorCFG));
	if(!StrCMP_n(_iniPortMotorCFG.ParaVer,PortMotorCFG.ParaVer,7))//判断是否要升级参数
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

//保存端口配置文件
void SavePortConfig(void)
{
	WriteBytesToFile(_portFileName,(u8*) &PortMotorCFG,0,sizeof(PortMotorCFG));  
}

//恢复出厂默认的端口配置文件
void WritDefaultPortCFG(void)
{
	IniDefaultPortCFG();
	WriteBytesToFile(_portFileName,(u8*) &PortMotorCFG,0,sizeof(PortMotorCFG));
	for(u8 i=0;i<FixPortNum;i++)
	{
		PortMotorCFG.PortCFG[i].Outport=&OutPortList[PortMotorCFG.PortCFG[i].PortIndex];
	}	
}


