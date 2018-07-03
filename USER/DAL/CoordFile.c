#include "CoordFile.h"
#include "MemoryAlloc.h"
 
 //�ŵ�ͷ�ļ����ú궨�壬���ܷ�����չ�ڴ�
//const u16 _fileMaxCount=200;  //����ļ���  
//const u16 _coordMaxCount=600 ;//���������
const u8 _oneCoordBytes = 12; //һ��������ֽ���

CoordFileAttri _workFile;//��ǰ�����ļ� 
u16 _leftCoordCount=0;
u16 _rightCoordCount=0;
CoordStruct _coordListLeft[_coordMaxCount]__at(LeftCoordAddr);//��ƽ̨�����б�
CoordStruct _coordListRight[_coordMaxCount] __at(RightCoordAddr);//��ƽ̨�����б�
CoordFileAttri _fileList[_fileMaxCount] __at(CoordFileAttriAddr);//�ļ������б�
//u8 _DMASendBuff_COM3[_sendBuffLeng_COM3] __at(LcdSendBuffAddr);//DMA���ͻ���

char* _diskStrCrd="0:/";
char* _coordDir="0:/COORD";
char _pathNameBuff[50];
CoordFileAttri _cfa;//��ʱ���ļ�������Ϣ�ṹ
u16 _fileAttriLeng=sizeof(_cfa);//�ļ�������Ϣ�ṹ�ֽ���

//��ָ�����ļ����õ�������·��
char* GetFullPath(char* fileName)
{
    memset(_pathNameBuff,0,sizeof(_pathNameBuff));
    sprintf(_pathNameBuff,"%s%s%s",_coordDir,"/",fileName);
    return _pathNameBuff;
}

//��ǰ�����ļ�������·��
char* WorkFileFullPath(void)
{
  return GetFullPath(Parameter.WorkFileName);
}

//��ȡָ���ļ����������
CoordFileAttri GetFileAttri(FIL* openedFile)
{
    u32 re;    
    f_lseek(openedFile,0);
    FRESULT res = f_read(openedFile,(u8*)&_cfa,_fileAttriLeng,&re);     
    return _cfa;
}

//����ָ���ļ����������
void SaveFileAttri(FIL* openedFile,CoordFileAttri cfi)
{
    u32 re;  
	cfi.CoordCount=cfi.LeftCoordCount+cfi.RightCoordCount;
    f_lseek(openedFile,0);
	f_write(openedFile,(u8*)&cfi,_fileAttriLeng,&re);
}

//���޸�ʱ�併������Ƚ���
int comp_des1(const void*a,const void*b)
{
	return (*(CoordFileAttri *)a).DateTime < (*(CoordFileAttri *)b).DateTime ? 1 : -1; 
}

//���޸�ʱ�併������,����޸ĵķ���ǰ��
void ShortFile_TimeDES1(CoordFileAttri* sortFileList,u8 fileCount)
{
   qsort(sortFileList,fileCount,sizeof(sortFileList[0]),comp_des1);
}

//��ȡ�ļ���Ϣ�б�
CoordFileAttri* ReadFileList(u16* fileCount)
{
	FATFS fs;
    FIL file;
	FILINFO finfo;
	DIR dirs;
	u16 fileIndex=0;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);
	res = f_opendir(&dirs, _coordDir);
	while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
	{
		if (finfo.fattrib & AM_DIR) 
		{ 
			continue;
		} 
		else 
		{  
			strcpy( _fileList[fileIndex].FileName,finfo.fname);
            res = f_open(&file,GetFullPath(finfo.fname),FA_READ);			
			CoordFileAttri cfi = GetFileAttri(&file);
			_fileList[fileIndex].CoordCount=cfi.CoordCount;
			_fileList[fileIndex].LeftCoordCount=cfi.LeftCoordCount;
			_fileList[fileIndex].RightCoordCount=cfi.RightCoordCount;
            _fileList[fileIndex].DateTime=(int)(finfo.fdate<<16|finfo.ftime);
			res = f_close(&file);
			fileIndex++;		
		}
	}
    res = f_closedir(&dirs);
    res = f_mount(NULL,_diskStrCrd, 0);
	*fileCount =fileIndex;
    ShortFile_TimeDES1( _fileList,fileIndex);
	return _fileList;
}
 
//����ָ�����Ƶ��ļ�
void CreateFile(char* fileName)
{	
	u32 reindex;
    FIL file;
	FATFS fs;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);	
    memset(_pathNameBuff,0,sizeof(_pathNameBuff));
    sprintf(_pathNameBuff,"%s%s%s%s",_coordDir,"/",fileName,".dat");
	res = CreateDir_in(_pathNameBuff);
	res = f_open(&file,_pathNameBuff,FA_READ | FA_WRITE | FA_CREATE_NEW);
	
	memset((u8*)&_cfa,0,_fileAttriLeng);
	memset(_pathNameBuff,0,sizeof(_pathNameBuff));
	sprintf(_pathNameBuff,"%s%s",fileName,".dat");
    strcpy(_cfa.FileName,_pathNameBuff);//��ʵûʲô�ã�ֻ�Ƿ���ṹ��Ĵ洢
    res = f_write(&file,(u8*)&_cfa,_fileAttriLeng,&reindex);
    res = f_close(&file);	
	res  = f_mount(NULL, _diskStrCrd, 0);
    
	SetWorkFile(_cfa);
}

//ɾ��ָ�����ļ�
void DeleteFile(CoordFileAttri file)
{
	DeleteFileOnDisk(GetFullPath(file.FileName));
	if(StrCMP(file.FileName,Parameter.WorkFileName))//���ɾ�����ǹ����ļ�
    {
	   memset(Parameter.WorkFileName,0,sizeof(Parameter.WorkFileName));
       IniCoordFile();
    }
}

char newPathName[50];
//����ָ���ļ����ļ���
void Rename(CoordFileAttri oldFile, char* newFileName)
{
    memset(newPathName,0,50);
	sprintf(newPathName,"%s%s%s%s",_coordDir,"/",newFileName,".dat");
	RenameOnDisk(GetFullPath(oldFile.FileName),newPathName);
	
	bool isWorkFileRe=StrCMP(oldFile.FileName,Parameter.WorkFileName);
	memset(oldFile.FileName,0,sizeof(oldFile.FileName));
	sprintf(oldFile.FileName,"%s%s",newFileName,".dat");
	if(isWorkFileRe)//������µ��ǹ����ļ�
    {
		SetWorkFile(oldFile);
    }
}

//��ȡ��ǰ���������ļ�����
void ReadCurrCoordFileAttri(void)
{
	u32 re;
	FIL file;
	FATFS fs;    
	u32 reindex;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);	                        
    res = f_open(&file,WorkFileFullPath(),FA_READ | FA_WRITE);
	if (res == FR_OK)
	{
		CoordFileAttri cfi = GetFileAttri(&file);
		_workFile.CoordCount=cfi.CoordCount;
		_workFile.LeftCoordCount=cfi.LeftCoordCount;
		_workFile.RightCoordCount=cfi.RightCoordCount;
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 
}

//��ʼ�������ļ�
void IniCoordFile(void)
{
	memset((u8*)&_fileList,0,sizeof(_fileList));
	u16 fileCount=0;
	ReadFileList(&fileCount);
	if (fileCount > 0)
	{
        if(*Parameter.WorkFileName=='\0')
        {
            SetWorkFile(_fileList[0]);
            return;
        }
		for(u16 i=0;i<fileCount;i++)
        {
            if(StrCMP(_fileList[i].FileName,Parameter.WorkFileName))
            {
               _workFile=_fileList[i];
               break;
            }
        }		
	}
	else
	{
		CreateFile("NewFile");
	}
}

//����ָ���ļ�Ϊ��ǰ�����ļ�
void SetWorkFile(CoordFileAttri cfa)
{
	_workFile=cfa;
	strcpy(Parameter.WorkFileName,cfa.FileName);    
	WriteParameter(NULL);
}

/********************�������******************/

//��ȡ��ǰ�ļ��е������б�
// platform:��ƽ̨����ƽ̨��*coordLeng�����ã�ֵΪ�������
CoordStruct* GetCoordList(LeftRightPlatformEnum platform, u16* coordLeng)
{
	//��һ�֣�ֱ�Ӵ��ļ����������µ��ô˺����������ڴ�Ҫ�ܴ�
	CoordStruct* currCoordList=_coordListLeft;
	u32 re;
	FIL file;
	FATFS fs;    
	u32 reindex;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);	                        
	res = f_open(&file,WorkFileFullPath(),FA_READ);
	if (res == FR_OK)
	{
        CoordFileAttri cfi = GetFileAttri(&file);
        u16 startAddr=_fileAttriLeng;  
        u16 coordCount=cfi.LeftCoordCount;
        if(platform==RightPlatform)
        {
            startAddr=_fileAttriLeng+_oneCoordBytes*cfi.LeftCoordCount;
            coordCount=cfi.RightCoordCount;
            currCoordList=_coordListRight;
        }           
        f_lseek(&file,startAddr);
		memset(currCoordList,0,_coordMaxCount);
		res = f_read(&file,(u8*)currCoordList,coordCount*_oneCoordBytes,&re);          
        *coordLeng = coordCount;
	}
	f_close(&file);
	res  = f_mount(NULL, _diskStrCrd, 0); 	     
	return currCoordList;
}

//��ָ�����ļ����һ������
// platform:��ƽ̨����ƽ̨��saveCoord��Ҫ���������
void AddCoordinate(LeftRightPlatformEnum platform,MultiAxisCoordStruct saveCoord)
{
	u32 re;
	FIL file;
	FATFS fs;    
	u32 reindex;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);	                        
    res = f_open(&file,WorkFileFullPath(),FA_READ | FA_WRITE);
	if (res == FR_OK)
	{
		CoordFileAttri cfi = GetFileAttri(&file);
        CoordStruct coord;
        coord.X=saveCoord.X;
        coord.Z=saveCoord.Z;
        u16 startAddr=_fileAttriLeng + cfi.LeftCoordCount*_oneCoordBytes;  
        if(platform==LeftPlatform)
        {
            cfi.LeftCoordCount++;
            coord.Y=saveCoord.Y1;
        }
        else
        {
            cfi.RightCoordCount++;
            coord.Y=saveCoord.Y2;
            startAddr=_fileAttriLeng + cfi.CoordCount*_oneCoordBytes;  
        }
		
        InsetBytes(&file,(u8*)&coord,startAddr,_oneCoordBytes);  //�����Ƿ�����Ż�������д�ϳ�һ��
		SaveFileAttri(&file,cfi);
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 	
}

//��ָ�����ļ���Ӷ������
void AddMultiCoord(LeftRightPlatformEnum platform,CoordStruct* coordList,u16 count)
{
	u32 re;
	FIL file;
	FATFS fs;    
	u32 reindex;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);	                        
    res = f_open(&file,WorkFileFullPath(),FA_READ | FA_WRITE);
	if (res == FR_OK)
	{
		CoordFileAttri cfi = GetFileAttri(&file);
        u16 startAddr=_fileAttriLeng + cfi.LeftCoordCount*_oneCoordBytes;  
        if(platform==LeftPlatform)
        {
            cfi.LeftCoordCount+=count;
        }
        else
        {
            cfi.RightCoordCount+=count;
            startAddr=_fileAttriLeng + cfi.CoordCount*_oneCoordBytes;  
        }
		
        InsetBytes(&file,(u8*)coordList,startAddr,_oneCoordBytes*count);  //�����Ƿ�����Ż�������д�ϳ�һ��
		SaveFileAttri(&file,cfi);
		//ReadCoordList(&file,platform);	//��ȡ��Ӧ�������б��ڴ�		
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 	
}

//ɾ��һ�����ꡣ
// platform:��ƽ̨����ƽ̨��coordIndex:��0��ʼ����������
void DeleteCoordinate(LeftRightPlatformEnum platform,u16 coordIndex)
{
	u32 re;
	FIL file;
	FATFS fs;    
	u32 reindex;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);	                        
    res = f_open(&file,WorkFileFullPath(),FA_READ | FA_WRITE);
	if (res == FR_OK)
	{
		CoordFileAttri cfi = GetFileAttri(&file);
        u16 startAddr;  
		if(platform==LeftPlatform)
        {
			startAddr=_fileAttriLeng + coordIndex*_oneCoordBytes;  
			cfi.LeftCoordCount--;
        }
        else
        {
            startAddr=_fileAttriLeng +(cfi.LeftCoordCount+ coordIndex)*_oneCoordBytes;
			cfi.RightCoordCount--;
        }
        DeleteBytes(&file,startAddr,_oneCoordBytes);
		SaveFileAttri(&file,cfi);			
    }    
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 	
}

//����һ�����ꡣ
// platform:��ƽ̨����ƽ̨��MultiAxisCoordStruct coord��coordIndex:��0��ʼ����������
void InsertCoordinate(LeftRightPlatformEnum platform,MultiAxisCoordStruct saveCoord,u16 coordIndex)
{
	u32 re;
	FIL file;
	FATFS fs;    
	u32 reindex;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);	                        
    res = f_open(&file,WorkFileFullPath(),FA_READ | FA_WRITE);
	if (res == FR_OK)
	{
		CoordFileAttri cfi = GetFileAttri(&file);
        CoordStruct coord;
        coord.X=saveCoord.X;
        coord.Z=saveCoord.Z;
        u16 startAddr= _fileAttriLeng + coordIndex*_oneCoordBytes;    
        if(platform==LeftPlatform)
        {
            cfi.LeftCoordCount++;
            coord.Y=saveCoord.Y1;
        }
        else
        {
            cfi.RightCoordCount++;
            coord.Y=saveCoord.Y2;
            startAddr=_fileAttriLeng +(cfi.LeftCoordCount+ coordIndex)*_oneCoordBytes;
        }		
        InsetBytes(&file,(u8*)&coord,startAddr,_oneCoordBytes);  //�����Ƿ�����Ż�������д�ϳ�һ��
		SaveFileAttri(&file,cfi);
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 
}

//ɾ��ָ��ƽ̨����������
void DeletePlatCoord(LeftRightPlatformEnum platform)
{
	u32 re;
	FIL file;
	FATFS fs;    
	u32 reindex;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);	                        
    res = f_open(&file,WorkFileFullPath(),FA_READ | FA_WRITE);
	if (res == FR_OK)
	{
		CoordFileAttri cfi = GetFileAttri(&file);
        u16 startAddr; 
		u16 deleteByteCount;
		if(platform==LeftPlatform)
        {
			startAddr=_fileAttriLeng;  
			deleteByteCount=cfi.LeftCoordCount*_oneCoordBytes;
			cfi.LeftCoordCount=0;
        }
        else
        {
            startAddr=_fileAttriLeng +cfi.LeftCoordCount*_oneCoordBytes;
			deleteByteCount=cfi.RightCoordCount*_oneCoordBytes;
			cfi.RightCoordCount=0;
        }
		if(deleteByteCount>0)
		{
			DeleteBytes(&file,startAddr,_oneCoordBytes);
			SaveFileAttri(&file,cfi);	
		}			
    }    
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 	
}

//�޸�һ�����ꡣ
// platform:��ƽ̨����ƽ̨��coordIndex:��0��ʼ����������;saveCoord:�±��������
void UpdateCoordinate(LeftRightPlatformEnum platform,u16 coordIndex, MultiAxisCoordStruct* saveCoord)
{
	u32 re;
	FIL file;
	FATFS fs;    
	u32 reindex;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);	                        
    res = f_open(&file,WorkFileFullPath(),FA_READ | FA_WRITE);
	if (res == FR_OK)
	{
        CoordFileAttri cfi = GetFileAttri(&file);
        CoordStruct coord;
        coord.X=saveCoord->X;
        coord.Z=saveCoord->Z;
        u16 startAddr=_fileAttriLeng + coordIndex*_oneCoordBytes;  
        if(platform==LeftPlatform)
        {
            coord.Y=saveCoord->Y1;            
        }
        else
        {
            coord.Y=saveCoord->Y2;
            startAddr=_fileAttriLeng +cfi.LeftCoordCount*_oneCoordBytes+ coordIndex*_oneCoordBytes;
        }
		f_lseek(&file,startAddr);
        u8 res = f_write(&file,(u8*)&coord,_oneCoordBytes,&re);
		//ReadCoordList(&file,platform);	//��ȡ��Ӧ�������б��ڴ�		
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 	   
}

//�޸Ķ��������ꡣ
// platform:��ƽ̨����ƽ̨��saveCoord:�±��������
void UpdateCoords(LeftRightPlatformEnum platform,CoordStruct* saveCoords,u16 coordCount)
{
	u32 re;
	FIL file;
	FATFS fs;    
	u32 reindex;
	FRESULT res = f_mount(&fs,_diskStrCrd,0);	                        
    res = f_open(&file,WorkFileFullPath(),FA_READ | FA_WRITE);
	if (res == FR_OK)
	{
        CoordFileAttri cfi = GetFileAttri(&file);
        u16 startAddr=_fileAttriLeng;
        u16 platBytes=cfi.LeftCoordCount*_oneCoordBytes;
        if(platform==RightPlatform)
        {
            startAddr=_fileAttriLeng +cfi.LeftCoordCount*_oneCoordBytes;
            platBytes=cfi.RightCoordCount*_oneCoordBytes;
        }
        f_lseek(&file,startAddr);
        u8 res = f_write(&file,(u8*)saveCoords,platBytes,&re);
		//ReadCoordList(&file,platform);//��ȡ��Ӧ�������б��ڴ�		
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 	
}

/********************������� END******************/
