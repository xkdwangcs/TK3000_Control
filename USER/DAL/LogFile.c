#include <stm32f4xx.h>
#include "LogFile.h"
#include "LogFileView.h"
#include "MainForm.h"

char* _logDir="0:/LOG";
char _logFileName[50];
SortFileSturct _fileListLog[LogFileMaxCount];

u16 _logRowCount=0;//当前日志缓存条数
u32 _currLogBuffLeng=0;//当前日志缓存的字节数
char _logWriteBuff[512];//日志写入缓存，为了解决频繁写日志可能带来不稳定性

//如果日志文件超过30个则删除最后一个文件
void DeleteLastFile(void);

//读取多行文本，行尾用"\r\n"结尾，返回一次读取到的行数。rowStart:当前/下一行开始的地址
u8 ReadLogFileMultiRow(char* logFileName,u32* rowStart)
{
    memset(_logFileName,0,sizeof(_logFileName));
    sprintf(_logFileName,"%s%s%s",_logDir,"/",logFileName);
    u32 br;  
    FIL file;
	FRESULT res = OpenFile(_logFileName,FA_READ,&file);
    if(res!=FR_OK) 
        return 0;
    u32 startIndex=*rowStart;
    u8 readRowIndex=0;
    for(readRowIndex=0;readRowIndex<OnePageLogMaxRow;readRowIndex++)
    {
        if(startIndex>=file.obj.objsize)
            break;
        u32 canReadMax=file.obj.objsize-startIndex;//可以读的最大长度
        u16 readLenght=OneLogMaxLeng;
        if(readLenght>canReadMax)
            readLenght=canReadMax;
        memset(_logTextBuff[readRowIndex],0,readLenght);
        f_lseek(&file,startIndex);       
        res = f_read(&file, _logTextBuff[readRowIndex], readLenght, &br); 
        u32 currRowLeng=readLenght;
        for(u16 i=0;i<readLenght-1;i++)
        {
            if(_logTextBuff[readRowIndex][i]==0x0D && _logTextBuff[readRowIndex][i+1]==0x0A)
            {
                for(u16 kk=i+2;kk<readLenght;kk++)
                {
                   _logTextBuff[readRowIndex][kk]=0x00; 
                }
                currRowLeng=i+2;
                break;
            }
        }
        startIndex=startIndex+currRowLeng;
        if(startIndex>=file.obj.objsize)
        {
            startIndex=0;            
            break;
        }          
    }
    *rowStart=startIndex;
    CloseFile(&file,_logFileName);
    return readRowIndex+1;    
}

//char _currLogTextInput[60]={0};
//char _oneLogBuff[100];//一条日志的缓存
//void WriteLogCall(void)
//{
//    u32 reindex;
//    FIL file;
//	FATFS fs;
//	DeleteLastFile();
//    char* diskStr=GetDiskStrByFullPath(_logDir);
//	FRESULT res = f_mount(&fs,diskStr,0);
//    if(res!=FR_OK)
//        return;	
//    char* currDate=GetCurrDateStr();
//    memset(_logFileName,0,sizeof(_logFileName));
//    sprintf(_logFileName,"%s%s%s%s",_logDir,"/",currDate,".log");
//	res = CreateDir_in(_logFileName);
//	res = f_open(&file,_logFileName,FA_WRITE);  //文件如果存在则不能用FA_CREATE_NEW，否则写入会失败
//	if(res!=FR_OK)
//    {		
//		res = f_mount(&fs,diskStr,0);
//        res = f_open(&file,_logFileName,FA_WRITE|FA_CREATE_ALWAYS);  //如果不存在则创建 
//    }
//	if(res==FR_OK)
//    {
//		memset(_oneLogBuff,0,sizeof(_oneLogBuff));
//		sprintf(_oneLogBuff,"%s%s%s%s%s","[",GetCurrTimeStr(),"] ",_currLogTextInput,"\r\n");
//        InsetBytes(&file,(u8*)_oneLogBuff, file.obj.objsize,strlen(_oneLogBuff));
//        res = f_close(&file);	
//    }
//	res  = f_mount(NULL, diskStr, 0);
//	ShowDebugMsg("日志写入成功");
//}

//void WriteLog(char* oneLineLog)
//{	
//	ShowDebugMsg("写日志");
//	memset(_currLogTextInput,0,sizeof(_currLogTextInput));
//	strcpy(_currLogTextInput,oneLineLog);
//	_oneFatfsOperateFunc=WriteLogCall;
//	StartFATFSTask();
//}

//char _currData[11];
void WriteLogCall(void)
{
    u32 reindex;
    FIL file;
	FATFS fs;
	DeleteLastFile();
    char* diskStr=GetDiskStrByFullPath(_logDir);
	FRESULT res = f_mount(&fs,diskStr,0);
    if(res!=FR_OK)
        return;	
    char* currDate=GetCurrDateStr();//_currData;
    memset(_logFileName,0,sizeof(_logFileName));
    sprintf(_logFileName,"%s%s%s%s",_logDir,"/",currDate,".log");
	res = CreateDir_in(_logFileName);
	res = f_open(&file,_logFileName,FA_WRITE);  //文件如果存在则不能用FA_CREATE_NEW，否则写入会失败
	if(res!=FR_OK)
    {		
		res = f_mount(&fs,diskStr,0);
        res = f_open(&file,_logFileName,FA_WRITE|FA_CREATE_ALWAYS);  //如果不存在则创建 
    }
	if(res==FR_OK)
    {
        InsetBytes(&file,(u8*)_logWriteBuff, file.obj.objsize,_currLogBuffLeng);
        res = f_close(&file);	
    }
	res  = f_mount(NULL, diskStr, 0);
	_logRowCount=0;
	_currLogBuffLeng=0;
	ShowDebugMsg("日志写入成功");
}

char _oneLogBuff[100];//一条日志的缓存
void WriteLog(char* oneLineLog)
{
	ShowDebugMsg("写入日志缓存");
	memset(_oneLogBuff,0,sizeof(_oneLogBuff));
	sprintf(_oneLogBuff,"%s%s%s%s%s","[",GetCurrTimeStr(),"] ",oneLineLog,"\r\n");
	strcpy(_logWriteBuff+_currLogBuffLeng,_oneLogBuff);
	
	_logRowCount++;
	u16 leng=strlen(_oneLogBuff);
	_currLogBuffLeng+=leng;
	
	if(_logRowCount>=5)//5条日志保存一次
	{
		ShowDebugMsg("日志缓存写入文件");
		_oneFatfsOperateFunc=WriteLogCall;
		StartFATFSTask();
	}
}

//手动把缓存中的数据写入文件。指日志条数小于指定大小时手动写入
void WriteLogBuffToFile(void)
{
	if(_logRowCount>0)
	{
		ShowDebugMsg("日志缓存写入文件");
		//如果调用线程，在急停中反而写不进
		//_oneFatfsOperateFunc=WriteLogCall;
		//StartFATFSTask();
		WriteLogCall();//直接写入
	}
}

//char _fileListName[LogFileMaxCount][10];
//获取日志文件列表,逆序排列:最近的文件排在前面
u8 GetLogFileList(void)
{ 	
	u8 index=GetFileListDateTime_Dir(_logDir,_fileListLog,LogFileMaxCount,NULL);
	ShortFile_NameDES(_fileListLog,index);	
	return index;
	
//	u8 index=GetFileListDateTime_Dir(_logDir,_fileListLog,LogFileMaxCount,NULL);//"log");
//	memset(_fileListName,0,sizeof(_fileListName));
//	for(u8 i=0;i<index;i++)
//	{
//		strcpy(_fileListName[i],_fileListLog[i].FileName);
//	}
//	StringArrayShortASC((char*)_fileListName,index,10);
//	return index;
}

//如果日志文件超过最大个数则删除最后一个文件
void DeleteLastFile()
{
	u8 logFileCount = GetLogFileList();//里面会卸载磁盘
	//s8 delCount =logFileCount-LogFileMaxCount;//要删除的文件数
	if(logFileCount>=LogFileMaxCount)//
	{
		ShowDebugMsg("删除最早的日志文件");
		memset(_logFileName,0,sizeof(_logFileName));
		sprintf(_logFileName,"%s%s%s",_logDir,"/",_fileListLog[LogFileMaxCount-1].FileName);
		DeleteFileOnDisk(_logFileName);//里面会卸载磁盘
		//f_unlink(_logFileName);
	}
}  

void DeleteAllLogFile()
{
    DeleteAllFileOnDir(_logDir);
}

void TestLogFile(char* dataStr,char* msgStr)
{
	//memset(_currData,0,sizeof(_currData));
	//strcpy(_currData,dataStr);
	ShowDebugMsg("写入日志缓存");
	memset(_oneLogBuff,0,sizeof(_oneLogBuff));
	sprintf(_oneLogBuff,"%s%s%s%s%s","[",GetCurrTimeStr(),"] ",msgStr,"\r\n");
	strcpy(_logWriteBuff+_currLogBuffLeng,_oneLogBuff);
	
	_logRowCount++;
	u16 leng=strlen(_oneLogBuff);
	_currLogBuffLeng+=leng;
	
	//if(_logRowCount>=3)
	{
		ShowDebugMsg("日志缓存写入文件");
		_oneFatfsOperateFunc=WriteLogCall;
		StartFATFSTask();
	}
}

