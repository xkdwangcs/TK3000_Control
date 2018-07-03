#include "WorkDataFile.h"

//ÿ�ο���дһ�ε�����,����һ�ε�½ʱ�䣬��������������
u16 _workDataSection=FLASH_SECTOR_COUNT+7;
//��������ʵ��
WorkDataStruct WorkData;
//��������ʵ��
WorkDataStruct _iniWorkData;

//д���ʼ���ļ�����
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

//��ʼ����������
void IniWorkData(void)
{
    IniWorkDataValue();
	ReadBytes(_workDataSection,(u8 *)&WorkData,0,sizeof(WorkData));
	if(!StrCMP_n(_iniWorkData.ParaVer,WorkData.ParaVer,7))//�ж��Ƿ�Ҫ��������
	{
		WriteBytes(_workDataSection,(u8 *) &_iniWorkData,sizeof(_iniWorkData));
		ReadBytes(_workDataSection,(u8 *)&WorkData,0,sizeof(WorkData));
	}	
}

//�ѹ�������д��洢��
void WriteWorkData(void)
{
	WriteBytes(_workDataSection,(u8 *) &WorkData,sizeof(WorkData));
}

