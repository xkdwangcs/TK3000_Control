#ifndef _WORKDATAFILE_H_
#define _WORKDATAFILE_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
#include "FileOperate.h"
#include "Public/PublicFunction.h"
    
                
    //要统计的相关条目
	typedef struct
	{
		u32 totalNumber;				//总加工数量
		u32 qualifiedNumber;			//合格数量
		u32 unQualifiedNumber;			//不合格数量
	} StatisticsStruct;

    typedef struct
	{
       	StatisticsStruct DayStatistics;		//日统计数量
		StatisticsStruct TotalStatistics;	//总统计数量
		u8 DevieIniFailNum; 				//设备初始化失败次数
		u8 DeviceIniBadTag; 				//文件系统损坏的标志
		char LastOnPowerDate[20];           //最后一次开机时间，用于判断设备时间是否正确、非法注册等用途
		char ParaVer[7];					//参数版本，用于自动升级参数扇区或当扇区损坏时重写  
    }WorkDataStruct;//工作数据结构体
    
//参数实例
extern WorkDataStruct WorkData;
//从存储器中读取相关工作数据
void IniWorkData(void);
//把工作数据写入存储器
void WriteWorkData(void);
	
/**************************/	

    
#ifdef __cplusplus
}
#endif

#endif 
