#include "CoordFile.h"
#include "MemoryAlloc.h"
 
 //放到头文件，用宏定义，才能分配扩展内存
//const u16 _fileMaxCount=200;  //最大文件数  
//const u16 _coordMaxCount=600 ;//最大坐标数
const u8 _oneCoordBytes = 12; //一个坐标的字节数

CoordFileAttri _workFile;//当前工作文件 
u16 _leftCoordCount=0;
u16 _rightCoordCount=0;
CoordStruct _coordListLeft[_coordMaxCount]__at(LeftCoordAddr);//左平台坐标列表
CoordStruct _coordListRight[_coordMaxCount] __at(RightCoordAddr);//右平台坐标列表
CoordFileAttri _fileList[_fileMaxCount] __at(CoordFileAttriAddr);//文件属性列表
//u8 _DMASendBuff_COM3[_sendBuffLeng_COM3] __at(LcdSendBuffAddr);//DMA发送缓存

char* _diskStrCrd="0:/";
char* _coordDir="0:/COORD";
char _pathNameBuff[50];
CoordFileAttri _cfa;//临时的文件特征信息结构
u16 _fileAttriLeng=sizeof(_cfa);//文件特征信息结构字节数

//由指定的文件名得到完整的路径
char* GetFullPath(char* fileName)
{
    memset(_pathNameBuff,0,sizeof(_pathNameBuff));
    sprintf(_pathNameBuff,"%s%s%s",_coordDir,"/",fileName);
    return _pathNameBuff;
}

//当前工作文件的完整路径
char* WorkFileFullPath(void)
{
  return GetFullPath(Parameter.WorkFileName);
}

//读取指定文件的相关属性
CoordFileAttri GetFileAttri(FIL* openedFile)
{
    u32 re;    
    f_lseek(openedFile,0);
    FRESULT res = f_read(openedFile,(u8*)&_cfa,_fileAttriLeng,&re);     
    return _cfa;
}

//保存指定文件的相关属性
void SaveFileAttri(FIL* openedFile,CoordFileAttri cfi)
{
    u32 re;  
	cfi.CoordCount=cfi.LeftCoordCount+cfi.RightCoordCount;
    f_lseek(openedFile,0);
	f_write(openedFile,(u8*)&cfi,_fileAttriLeng,&re);
}

//按修改时间降序排序比较器
int comp_des1(const void*a,const void*b)
{
	return (*(CoordFileAttri *)a).DateTime < (*(CoordFileAttri *)b).DateTime ? 1 : -1; 
}

//按修改时间降序排序,最近修改的放在前面
void ShortFile_TimeDES1(CoordFileAttri* sortFileList,u8 fileCount)
{
   qsort(sortFileList,fileCount,sizeof(sortFileList[0]),comp_des1);
}

//读取文件信息列表
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
 
//创建指定名称的文件
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
    strcpy(_cfa.FileName,_pathNameBuff);//其实没什么用，只是方便结构体的存储
    res = f_write(&file,(u8*)&_cfa,_fileAttriLeng,&reindex);
    res = f_close(&file);	
	res  = f_mount(NULL, _diskStrCrd, 0);
    
	SetWorkFile(_cfa);
}

//删除指定的文件
void DeleteFile(CoordFileAttri file)
{
	DeleteFileOnDisk(GetFullPath(file.FileName));
	if(StrCMP(file.FileName,Parameter.WorkFileName))//如果删除的是工作文件
    {
	   memset(Parameter.WorkFileName,0,sizeof(Parameter.WorkFileName));
       IniCoordFile();
    }
}

char newPathName[50];
//更改指定文件的文件名
void Rename(CoordFileAttri oldFile, char* newFileName)
{
    memset(newPathName,0,50);
	sprintf(newPathName,"%s%s%s%s",_coordDir,"/",newFileName,".dat");
	RenameOnDisk(GetFullPath(oldFile.FileName),newPathName);
	
	bool isWorkFileRe=StrCMP(oldFile.FileName,Parameter.WorkFileName);
	memset(oldFile.FileName,0,sizeof(oldFile.FileName));
	sprintf(oldFile.FileName,"%s%s",newFileName,".dat");
	if(isWorkFileRe)//如果更新的是工作文件
    {
		SetWorkFile(oldFile);
    }
}

//读取当前工作坐标文件属性
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

//初始化坐标文件
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

//设置指定文件为当前工作文件
void SetWorkFile(CoordFileAttri cfa)
{
	_workFile=cfa;
	strcpy(Parameter.WorkFileName,cfa.FileName);    
	WriteParameter(NULL);
}

/********************坐标操作******************/

//获取当前文件中的坐标列表
// platform:左平台或右平台；*coordLeng：引用，值为坐标个数
CoordStruct* GetCoordList(LeftRightPlatformEnum platform, u16* coordLeng)
{
	//第一种，直接从文件读。但导致调用此函数的任务内存要很大
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

//向指定的文件添加一个坐标
// platform:左平台或右平台；saveCoord：要保存的坐标
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
		
        InsetBytes(&file,(u8*)&coord,startAddr,_oneCoordBytes);  //这里是否可以优化？两次写合成一次
		SaveFileAttri(&file,cfi);
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 	
}

//向指定的文件添加多个坐标
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
		
        InsetBytes(&file,(u8*)coordList,startAddr,_oneCoordBytes*count);  //这里是否可以优化？两次写合成一次
		SaveFileAttri(&file,cfi);
		//ReadCoordList(&file,platform);	//读取相应的坐标列表到内存		
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 	
}

//删除一个坐标。
// platform:左平台或右平台；coordIndex:从0开始的坐标索引
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

//插入一个坐标。
// platform:左平台或右平台；MultiAxisCoordStruct coord；coordIndex:从0开始的坐标索引
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
        InsetBytes(&file,(u8*)&coord,startAddr,_oneCoordBytes);  //这里是否可以优化？两次写合成一次
		SaveFileAttri(&file,cfi);
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 
}

//删除指定平台的所有坐标
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

//修改一个坐标。
// platform:左平台或右平台；coordIndex:从0开始的坐标索引;saveCoord:新保存的坐标
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
		//ReadCoordList(&file,platform);	//读取相应的坐标列表到内存		
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 	   
}

//修改多个点的坐标。
// platform:左平台或右平台；saveCoord:新保存的坐标
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
		//ReadCoordList(&file,platform);//读取相应的坐标列表到内存		
    }
    f_close(&file);
    res  = f_mount(NULL, _diskStrCrd, 0); 	
}

/********************坐标操作 END******************/
