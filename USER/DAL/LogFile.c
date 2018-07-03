#include <stm32f4xx.h>
#include "LogFile.h"
#include "LogFileView.h"
#include "MainForm.h"

char* _logDir="0:/LOG";
char _logFileName[50];
SortFileSturct _fileListLog[LogFileMaxCount];

u16 _logRowCount=0;//��ǰ��־��������
u32 _currLogBuffLeng=0;//��ǰ��־������ֽ���
char _logWriteBuff[512];//��־д�뻺�棬Ϊ�˽��Ƶ��д��־���ܴ������ȶ���

//�����־�ļ�����30����ɾ�����һ���ļ�
void DeleteLastFile(void);

//��ȡ�����ı�����β��"\r\n"��β������һ�ζ�ȡ����������rowStart:��ǰ/��һ�п�ʼ�ĵ�ַ
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
        u32 canReadMax=file.obj.objsize-startIndex;//���Զ�����󳤶�
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
//char _oneLogBuff[100];//һ����־�Ļ���
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
//	res = f_open(&file,_logFileName,FA_WRITE);  //�ļ��������������FA_CREATE_NEW������д���ʧ��
//	if(res!=FR_OK)
//    {		
//		res = f_mount(&fs,diskStr,0);
//        res = f_open(&file,_logFileName,FA_WRITE|FA_CREATE_ALWAYS);  //����������򴴽� 
//    }
//	if(res==FR_OK)
//    {
//		memset(_oneLogBuff,0,sizeof(_oneLogBuff));
//		sprintf(_oneLogBuff,"%s%s%s%s%s","[",GetCurrTimeStr(),"] ",_currLogTextInput,"\r\n");
//        InsetBytes(&file,(u8*)_oneLogBuff, file.obj.objsize,strlen(_oneLogBuff));
//        res = f_close(&file);	
//    }
//	res  = f_mount(NULL, diskStr, 0);
//	ShowDebugMsg("��־д��ɹ�");
//}

//void WriteLog(char* oneLineLog)
//{	
//	ShowDebugMsg("д��־");
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
	res = f_open(&file,_logFileName,FA_WRITE);  //�ļ��������������FA_CREATE_NEW������д���ʧ��
	if(res!=FR_OK)
    {		
		res = f_mount(&fs,diskStr,0);
        res = f_open(&file,_logFileName,FA_WRITE|FA_CREATE_ALWAYS);  //����������򴴽� 
    }
	if(res==FR_OK)
    {
        InsetBytes(&file,(u8*)_logWriteBuff, file.obj.objsize,_currLogBuffLeng);
        res = f_close(&file);	
    }
	res  = f_mount(NULL, diskStr, 0);
	_logRowCount=0;
	_currLogBuffLeng=0;
	ShowDebugMsg("��־д��ɹ�");
}

char _oneLogBuff[100];//һ����־�Ļ���
void WriteLog(char* oneLineLog)
{
	ShowDebugMsg("д����־����");
	memset(_oneLogBuff,0,sizeof(_oneLogBuff));
	sprintf(_oneLogBuff,"%s%s%s%s%s","[",GetCurrTimeStr(),"] ",oneLineLog,"\r\n");
	strcpy(_logWriteBuff+_currLogBuffLeng,_oneLogBuff);
	
	_logRowCount++;
	u16 leng=strlen(_oneLogBuff);
	_currLogBuffLeng+=leng;
	
	if(_logRowCount>=5)//5����־����һ��
	{
		ShowDebugMsg("��־����д���ļ�");
		_oneFatfsOperateFunc=WriteLogCall;
		StartFATFSTask();
	}
}

//�ֶ��ѻ����е�����д���ļ���ָ��־����С��ָ����Сʱ�ֶ�д��
void WriteLogBuffToFile(void)
{
	if(_logRowCount>0)
	{
		ShowDebugMsg("��־����д���ļ�");
		//��������̣߳��ڼ�ͣ�з���д����
		//_oneFatfsOperateFunc=WriteLogCall;
		//StartFATFSTask();
		WriteLogCall();//ֱ��д��
	}
}

//char _fileListName[LogFileMaxCount][10];
//��ȡ��־�ļ��б�,��������:������ļ�����ǰ��
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

//�����־�ļ�������������ɾ�����һ���ļ�
void DeleteLastFile()
{
	u8 logFileCount = GetLogFileList();//�����ж�ش���
	//s8 delCount =logFileCount-LogFileMaxCount;//Ҫɾ�����ļ���
	if(logFileCount>=LogFileMaxCount)//
	{
		ShowDebugMsg("ɾ���������־�ļ�");
		memset(_logFileName,0,sizeof(_logFileName));
		sprintf(_logFileName,"%s%s%s",_logDir,"/",_fileListLog[LogFileMaxCount-1].FileName);
		DeleteFileOnDisk(_logFileName);//�����ж�ش���
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
	ShowDebugMsg("д����־����");
	memset(_oneLogBuff,0,sizeof(_oneLogBuff));
	sprintf(_oneLogBuff,"%s%s%s%s%s","[",GetCurrTimeStr(),"] ",msgStr,"\r\n");
	strcpy(_logWriteBuff+_currLogBuffLeng,_oneLogBuff);
	
	_logRowCount++;
	u16 leng=strlen(_oneLogBuff);
	_currLogBuffLeng+=leng;
	
	//if(_logRowCount>=3)
	{
		ShowDebugMsg("��־����д���ļ�");
		_oneFatfsOperateFunc=WriteLogCall;
		StartFATFSTask();
	}
}

