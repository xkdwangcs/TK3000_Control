#ifndef __COORDFILE_H_
#define __COORDFILE_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "string.h"
#include "FileOperate.h"
#include "Parameter.h"
#include "AxisLinkage.h"

#define _fileMaxCount 20 //最大文件数  
#define _coordMaxCount 500 //最大坐标数
	 
typedef enum
{
	LeftPlatform=0,//左平台
	RightPlatform,//右平台
}LeftRightPlatformEnum;//左右平台枚举      

typedef struct
{
	char FileName[32];	//文件名称	
	u16 CoordCount;		//坐标总数
	u16 LeftCoordCount;	//左平台坐标数
	u16 RightCoordCount;//右平台坐标数
    int DateTime; //排序用
} CoordFileAttri;//坐标文件属性结构体

typedef struct
{
    u16 FileCount;
    CoordFileAttri FileList[_fileMaxCount];    
}FileListStruct; //文件列表相关数据

extern FileListStruct _fileList;//对外开放，用于地址计算
extern CoordStruct _coordListLeft[_coordMaxCount];//对外开放，用于地址计算

//当前工作文件
extern CoordFileAttri _workFile;
//初始化文件记录
void IniFileRecord(void);
//创建一个新文件
void CreateFile(char* fileName);
//读取文件信息列表
FileListStruct* ReadFileList();
//删除指定的文件
void DeleteFile(CoordFileAttri file);
//初始化坐标文件
void IniCoordFile(void);
//设置指定文件为当前工作文件
void SetWorkFile(CoordFileAttri file);
//更改指定文件的文件名
void Rename(CoordFileAttri file, char* newFileName);
void SaveFileAttri(FIL* openedFile,CoordFileAttri cfi);

//读取当前坐标文件属性,
void ReadCurrCoordFileAttri(void);
//获取指定文件中的坐标列表
// platform:左平台或右平台；*coordLeng：引用，值为坐标个数
CoordStruct* GetCoordList(LeftRightPlatformEnum platform, u16* coordLeng);
//向指定的文件添加一个坐标,TIME6会影响写入，导致死机
// platform:左平台或右平台；saveCoord：要保存的坐标
void AddCoordinate(LeftRightPlatformEnum platform,MultiAxisCoordStruct saveCoord);
//向指定的文件添加多个坐标
void AddMultiCoord(LeftRightPlatformEnum platform,CoordStruct* coordList,u16 count);
//插入一个坐标。
// platform:左平台或右平台；MultiAxisCoordStruct coord；coordIndex:从0开始的坐标索引
void InsertCoordinate(LeftRightPlatformEnum platform,MultiAxisCoordStruct saveCoord,u16 coordIndex);
//删除一个坐标。
// platform:左平台或右平台；coordIndex:从0开始的坐标索引
void DeleteCoordinate(LeftRightPlatformEnum platform,u16 coordIndex);
//删除指定平台的所有坐标
void DeletePlatCoord(LeftRightPlatformEnum platform);
//修改一个坐标。
// platform:左平台或右平台；coordIndex:从0开始的坐标索引;saveCoord:新保存的坐标
void UpdateCoordinate(LeftRightPlatformEnum platform,u16 coordIndex, MultiAxisCoordStruct* saveCoord);
//修改多个点的坐标。
// platform:左平台或右平台；saveCoords:新保存的坐标
void UpdateCoords(LeftRightPlatformEnum platform,CoordStruct* saveCoords,u16 coordCount);


#ifdef __cplusplus
}
#endif

#endif
