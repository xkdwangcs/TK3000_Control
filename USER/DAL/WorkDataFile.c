#include "WorkDataFile.h"

//每次开机写一次的数据,如上一次登陆时间，倒数第三个扇区
u16 _workDataSection=FLASH_SECTOR_COUNT+7;
//工作数据实例
WorkDataStruct WorkData;
//工作数据实例
WorkDataStruct _iniWorkData;

//写入初始化文件数据
void IniWorkDataValue(void)
{
	memset(&_iniWorkData, 0, sizeof(_iniWorkData));
    _iniWorkData.DayStatistics.totalNumber = 0;
	_iniWorkData.DayStatistics.qualifiedNumber = 0;
	_iniWorkData.DayStatistics.unQualifiedNumber = 0;
	_iniWorkData.TotalStatistics.totalNumber = 0;
	_iniWorkData.TotalStatistics.qualifiedNumber = 0;
	_iniWorkData.TotalStatistics.unQualifiedNumber = 0; 
	_iniWorkData.DevieIniFailNum=0;
	_iniWorkData.DeviceIniBadTag=false;
	strcpy(_iniWorkData.LastOnPowerDate, "2016-09-01");//"2999-01-01"
	strcpy(_iniWorkData.ParaVer,"ver1.01");
}

//初始化工作参数
void IniWorkData(void)
{
    IniWorkDataValue();
	ReadBytes(_workDataSection,(u8 *)&WorkData,0,sizeof(WorkData));
	if(!StrCMP_n(_iniWorkData.ParaVer,WorkData.ParaVer,7))//判断是否要升级参数
	{
		WriteBytes(_workDataSection,(u8 *) &_iniWorkData,sizeof(_iniWorkData));
		ReadBytes(_workDataSection,(u8 *)&WorkData,0,sizeof(WorkData));
	}	
}

//把工作数据写入存储器
void WriteWorkData(void)
{
	WriteBytes(_workDataSection,(u8 *) &WorkData,sizeof(WorkData));
}

