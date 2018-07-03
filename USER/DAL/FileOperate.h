#ifndef FILEOPERATE_H_
#define FILEOPERATE_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stm32f4xx.h>
#include "string.h"
#include "SST25V.h"
#include "stdbool.h"
#include "FATFS/src/ff.h"
#include "FATFS/src/integer.h"
#include "FATFS/src/diskio.h"
#include "RTC_F4xx.h"
#include "Handlers.h"
	 
typedef struct
{
	char FileName[50];	//文件名称	
	int DateTime;	    //修改日期及时间
} SortFileSturct;       //要排序的文件结构体

extern u8 _tempArray[FLASH_SECTOR_SIZE*4]; //用于扩展内存分配
void GetFileListTask(void);
	 
//由磁盘编号得出磁盘字符串
char* GetDiskStr(u8 diskID);
//从完全路径中得到盘符,如果不存在盘符则返回"0:/"
char* GetDiskStrByFullPath(char* fullPath);    
//从文件名中获取扩展名
char* GetExtName(char* fileName);	 
//格式化指定磁盘
void FormatDisk(u8 diskID);
//在指定的磁盘上创建一个目录，如果目录已存在也不会出错。支持一次创建多级目录
//path:带盘符的路径，如果不带盘符时默认为0号磁盘，也可以是完全文件路径。如：1:/AA/BB/CC/test.txt
FRESULT CreateDir_in(char* path);
//在指定的磁盘上创建一个目录，如果目录已存在也不会出错。支持一次创建多级目录
//path:带盘符的路径，如果不带盘符时默认为0号磁盘
void CreateDirOnDisk(char* path);
//创建一个文件。fileName:带盘符，如果没带盘符则默认为0号磁盘。支持多级目录，如：1:/AA/BB/CC/test.txt
//如果目录不存在则自动创建。isConver:是否复盖原有文件
FRESULT CreateFileOnDisk(char* fileName,bool isConver);
//在指定的磁盘上删除文件。fileName:带盘符，如果没带盘符则默认为0号磁盘。如：1:/AA/test.txt
void DeleteFileOnDisk(char* fileName);
//删除指定目录下的所有文件
void DeleteAllFileOnDir(char* dirPath);
//重命名文件。oldName：带盘符，如果没带盘符则默认为0号磁盘。如：1:/dir1/abc.txt。newName:1:/dir1/ok.txt
void RenameOnDisk(char* oldName,char* newName);
//读取指定目录下的所有文件名,返回文件个数。dirPath:目录路径，fileList:调用者提供的缓存，二维字符数组
//rowCount:有多少行，rowItemCount:每行的元素个数，fileterExt:用于过滤文件名的扩展名，NULL时不过滤
u16 GetFileList_Dir(char* dirPath, char* fileList,u16 rowCount,u16 rowItemCount,char* filterExt);
//读取指定目录下的所有文件名及修改时间,返回文件个数。dirPath:目录路径，fileList:调用者提供的缓存
//rowCount:缓存最大有多少行，fileterExt:用于过滤文件名的扩展名，NULL时不过滤
u16 GetFileListDateTime_Dir(char* dirPath, SortFileSturct* fileList,u16 rowCount,char* filterExt);
//打开一个指定的文件
FRESULT OpenFile(char* fileName,u8 openMode,FIL* fs);
//关闭指定的文件
FRESULT CloseFile(FIL* file,char* fileName);
//设置文件的创建日期及时间
FRESULT SetFileDateTime(char* path,DataTimeStruct* dateTime);
//按修改时间降序排序,最近修改的放在前面
void ShortFile_TimeDES(SortFileSturct* sortFileList,u8 fileCount);
//按修改时间升序排序,最近修改的放在后面
void ShortFile_TimeASC(SortFileSturct* sortFileList,u8 fileCount);
//按文件名降序排序
void ShortFile_NameDES(SortFileSturct* sortFileList,u8 fileCount);
//按文件名升序排序
void ShortFile_NameASC(SortFileSturct* sortFileList,u8 fileCount);

//向指定的文件的指定位置开始写入数据。如果起始位置大于文件大小将在文件最后开始写入
//fileName:包括盘符的完全路径,如:0:/CFG/Parameter.txt
void WriteBytesToFile(char* fileName,u8* data,u32 startIndex,u32 length);
//从指定的文件的指定位置开始读出内容。如果起始位置大于文件大小将不会执行读操作
//fileName:包括盘符的完全路径,如:0:/CFG/Parameter.txt。
void ReadBytesFormFile(char* fileName,u8* readBuff,u32 startIndex,u32 readLenght);
//删除指定文件相关内容。startIndex:从0开始的索引，开始删除的位置；count:要删除的字节个数
void DeleteBytes(FIL* openedFile,u32 startIndex,u32 count);
//删除指定文件相关内容。startIndex:从0开始的索引，开始删除的位置；count:要删除的字节个数
//fileName:包括盘符的完全路径,如:0:/DATA/NewFile.dat。
void DeleteBytesOnFile(char* fileName,u32 startIndex,u32 count);
//向文件插入数据。startIndex:从0开始的索引，开始插入的位置；count:要插入的字节个数
void InsetBytes(FIL* openedFile,u8* data,u32 startIndex,u32 count);
//向文件插入数据。startIndex:从0开始的索引，开始插入的位置；count:要插入的字节个数
void InsetBytesOnFile(char* filePath,u8* data,u32 startIndex,u32 count);

//向文件尾添加一行文本内容，文本后面以"/0"结尾
void AppendCharsToFile(char* fileName,char* data);
//向指定文件的末尾添加字节数据。count:要插入的字节个数
void AppendBytesToFile(FIL* openedFile,u8* data,u32 count);
//复制一个文件
//sourPathFile:源文件，如：0:/CDATA/abc.txt。 destPathFile：目标文件，如: 1:/CDATA/efd.txt。isCoverFile:是否覆盖现有文件
bool CopyFile(char* sourPathFile,char* destPathFile,bool isCoverFile);
//把一个目录下的所有文件复制到目标目录。如果目标目录不存在则自动创建，可以跨磁盘复制。
//sourcDir:源目录，如：0:/CDATA。 destDir：目标目录，如: 1:/CDATA。isCoverFile:是否覆盖现有文件
bool CopyAllFile_Dir(char* sourcDir,char* destDir,bool isCoverFile,OneStrParameterHandler showMSG);

#ifdef __cplusplus
}
#endif

#endif
