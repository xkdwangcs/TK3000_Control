#include "FileOperate.h"
#include "stdlib.h"
#include "stdbool.h"
#include "PublicFunction.h"
#include "MemoryAlloc.h"

u8 _fileBuff[FLASH_SECTOR_SIZE] __at(FileBuffAddr);
u8 _tempArray[FLASH_SECTOR_SIZE*4]__at(FileOPTempAddr);	

//完全路径缓存，包括
char _fullPathBuff[2][50];
//由磁盘编号得出磁盘字符串
char* GetDiskStr(u8 diskID)
{
     if(diskID==0)
         return "0:/";
     else if(diskID==1)
         return "1:/";
     else if(diskID==2)
         return "2:/";
}

char _diskStr[3];
//从完全路径中得到盘符,如果不存在盘符则返回"0:/"
char* GetDiskStrByFullPath(char* fullPath)
{
   if(fullPath[1]==0x3A && fullPath[2]==0x2F)
   {
      _diskStr[0]=fullPath[0];
      _diskStr[1]=fullPath[1];
      _diskStr[2]=fullPath[2];
      return _diskStr;
   }           
   return "0:/";
}

char _extName[5];
//从文件名中获取扩展名
char* GetExtName(char* fileName)
{
	u16 leng=strlen(fileName);
	u16 index=0;
	for(index=leng;index>=0;index--)
	{
		if(fileName[index]=='/')//如果先找到了目录符，表示无文件
		{
			return NULL;
		}
		if(fileName[index]=='.')
		{
			break;
		}
	}
	index++;
	memset(_extName,0,sizeof(_extName));
	u8 count=leng-index;
	for(u8 i=0;i<count;i++)
	{
		_extName[i]=fileName[i+index];
	}
	return _extName;
}

//由动器号及文件路径得取完整路径
char* GetFullPath_mdisk(u8 diskID,const char* dirFileName)
{
    char *fpb = _fullPathBuff[diskID];
    memset(fpb,0,sizeof(_fullPathBuff[0]));
    sprintf(fpb,"%s%s",GetDiskStr(diskID),dirFileName);
    return fpb; 
}

//格式化指定磁盘
void FormatDisk(u8 diskID)
{
    char* diskStr=GetDiskStr(diskID);   
	//FRESULT res = f_mkfs(diskStr,1,_MAX_SS); //0.12
	FRESULT res = f_mkfs(diskStr,FM_ANY,0,_fileBuff,sizeof(_fileBuff)); //0.12b
}


char pathBuff[80]={0};
char tpath[80]={0};
//在指定的磁盘上创建一个目录，如果目录已存在也不会出错。支持一次创建多级目录
//path:带盘符的路径，如果不带盘符时默认为0号磁盘，也可以是完全文件路径。如：1:/AA/BB/CC/test.txt
FRESULT CreateDir_in(char* path)
{
    FRESULT res=FR_OK;
    char* diskStr=GetDiskStrByFullPath(path);    

	memset(pathBuff,0,sizeof(pathBuff));
	memset(tpath,0,sizeof(tpath));
    tpath[0]=diskStr[0];
    tpath[1]=diskStr[1];
    u8 pcount=0;
    LIST* plist=SplitString(path,"/", &pcount);
    for(u8 i=0;i<pcount;i++)
    {   
        if(i==0 && strstr(plist[i].item,":") !=NULL)//如果是盘符，如：0:
            continue;
        if(i==pcount-1 && strstr(plist[i].item,".") !=NULL) //如果最后一级路径中有"."，则表示为文件。如"ABC.txt"
            break;
        memset(pathBuff,0,sizeof(pathBuff));
        sprintf(pathBuff,"%s%s%s",tpath,"/",plist[i].item); //要累加路径          
        FRESULT res = f_mkdir(pathBuff);
        if(res!=FR_OK && res!=FR_EXIST)
            return res;
        memset(tpath,0,sizeof(tpath));
        strcpy(tpath,pathBuff);
    }
    return res;
}
   
//在指定的磁盘上创建一个目录，如果目录已存在也不会出错。支持一次创建多级目录
//path:带盘符的路径，如果不带盘符时默认为0号磁盘，也可以是完全文件路径。如：1:/AA/BB/CC/test.txt
void CreateDirOnDisk(char* path)
{
    FATFS fs; 
    char* diskStr=GetDiskStrByFullPath(path);    
	FRESULT res = f_mount(&fs,diskStr,0);
	if (res != FR_OK)
	{
		return;
	}		
	//res = f_mkdir(path);
    res = CreateDir_in(path);
	res = f_mount(NULL, diskStr, 0);
}

//创建一个文件。fileName:带盘符，如果没带盘符则默认为0号磁盘。支持多级目录，如：1:/AA/BB/CC/test.txt
//如果目录不存在则自动创建。isConver:是否复盖原有文件,返回打开文件的状态
FRESULT CreateFileOnDisk(char* fileName,bool isCover)
{  
    u32 reindex;
    FIL file;
	FATFS fs;
    char* diskStr=GetDiskStrByFullPath(fileName);    

	u8 res = f_mount(&fs,diskStr,0);
	if (res != FR_OK)
	{
		return res;
	}
    res = CreateDir_in(fileName);//检查并创建相关路径
    u8 createMode= FA_CREATE_NEW;  //此属性当文件存在时不会重新创建，且写文件件时会FR_INVALID_OBJECT。在没有文件时可以正常创建
    if(isCover)
        createMode=FA_CREATE_ALWAYS;  //覆盖的时候创建模式要选这个重新创建一个文件，否则在写文件时会 res=FR_INVALID_OBJECT(09)
	res = f_open(&file,fileName,FA_READ | FA_WRITE | createMode);
    //下面的操作不要复盖文件打开的状态
	if (res == FR_OK)
	{	
		f_close(&file);			
	}
	f_mount(NULL, diskStr, 0);
	return res;
}

//在指定的磁盘上删除文件。fileName:带盘符，如果没带盘符则默认为0号磁盘。如：1:/AA/test.txt
void DeleteFileOnDisk(char* fileName)
{
    FATFS fs;
	FIL	file;
    char* diskStr=GetDiskStrByFullPath(fileName);     
    u8 res = f_mount(&fs,diskStr,0);
    res = f_unlink(fileName);	  /* Delete a File or Directory */
    res  = f_mount(NULL, diskStr, 0);
}

char fileName[50];
//删除指定目录下的所有文件
void DeleteAllFileOnDir(char* dirPath)
{
    FATFS fs;
    FIL file;
	FILINFO finfo;
	DIR dirs;
	u16 fileIndex=0;
    char* diskStr=GetDiskStrByFullPath(dirPath);
	FRESULT res = f_mount(&fs,diskStr,0);
	res = f_opendir(&dirs, dirPath);    
	while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
	{
		if (finfo.fattrib & AM_DIR) 
		{ 
			continue;
		} 
		else 
		{  
            memset(fileName,0, sizeof(fileName));
            sprintf(fileName,"%s%s%s",dirPath,"/",finfo.fname);
			res = f_unlink(fileName);	  /* Delete a File or Directory */	
		}
	}
}

//重命名文件。oldName：带盘符，如果没带盘符则为0号磁盘。如：1:/dir1/abc.txt。newName:1:/dir1/ok.txt
void RenameOnDisk(char* oldName,char* newName)
{
    FATFS fs;
	FIL	file;
    char* diskStr=GetDiskStrByFullPath(oldName); 
    u8 res = f_mount(&fs,diskStr,0);
    res = f_rename(oldName,newName);	 
    res  = f_mount(NULL, diskStr, 0);
}   

//打开一个指定的文件
FRESULT OpenFile(char* fileName,u8 openMode,FIL* file)
{
    FATFS fs;
    DIR dirs;
    u32 re;
    char* diskStr=GetDiskStrByFullPath(fileName);
	FRESULT res = f_mount(&fs,diskStr,0);
	if (res != FR_OK)
		return res;
	res = f_open(file,fileName,openMode);
    if(res!=FR_OK && res!=FR_EXIST)
        f_mount(NULL, diskStr, 0); 
    return res;
}

//关闭指定的文件
FRESULT CloseFile(FIL* file,char* fileName)
{
    FRESULT res = f_close(file);
    char* diskStr=GetDiskStrByFullPath(fileName);
    res  = f_mount(NULL, diskStr, 0); 
    return res;
}

//读取指定目录下的所有文件名,返回文件个数。dirPath:目录路径，fileList:调用者提供的缓存，二维数组
//rowCount:有多少行，rowItemCount:每行的元素个数，fileterExt:用于过滤文件名的扩展名，NULL时不过滤
u16 GetFileList_Dir(char* dirPath, char* fileList,u16 rowCount,u16 rowItemCount,char* filterExt)
{
	if(dirPath==NULL || fileList==NULL)
		return 0;
	FATFS fs;
    FIL file;
	FILINFO finfo;
	DIR dirs;
	u16 fileIndex=0;
	u16 ddd=sizeof(*fileList);
	char* diskStr=GetDiskStrByFullPath(dirPath);
	FRESULT res=f_mount(&fs, diskStr, 0);
	res = f_opendir(&dirs, dirPath);
	if(res) return 0;
	while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
	{
		if (finfo.fattrib & AM_DIR) 
		{ 
			continue;
		} 
		else 
		{  
			if(*finfo.fname=='\0')
				continue;
			if(fileIndex>=rowCount)//如果超出最大行数，则跳出
				break;
			//执行了过滤的这几行代码后，将导致USB打开出现FR_NO_ENABLE，原因待查
			//考虑目前用不上此功能，暂时不用
//			if(filterExt!=NULL) 
//			{				
//				char* extName = GetExtName(finfo.fname);
//				if(!StrCMP_n(extName,filterExt,strlen(extName)))
//					continue;
//			}
			strcpy(fileList+fileIndex*rowItemCount,finfo.fname);
			fileIndex++;		
		}
	}
    res = f_closedir(&dirs);
    res = f_mount(NULL,diskStr, 0);
	return fileIndex;
}

//读取指定目录下的所有文件名及修改时间,返回文件个数。dirPath:目录路径，fileList:调用者提供的缓存
//rowCount:缓存最大有多少行，fileterExt:用于过滤文件名的扩展名，NULL时不过滤
u16 GetFileListDateTime_Dir(char* dirPath, SortFileSturct* fileList,u16 rowCount,char* filterExt)
{
	if(dirPath==NULL || fileList==NULL)
		return 0;
	FATFS fs;
    FIL file;
	FILINFO finfo;
	DIR dirs;
	u16 fileIndex=0;
	u16 ddd=sizeof(*fileList);
	char* diskStr=GetDiskStrByFullPath(dirPath);
	FRESULT res=f_mount(&fs, diskStr, 0);
	res = f_opendir(&dirs, dirPath);
	if(res) return 0;
	while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
	{
		if (finfo.fattrib & AM_DIR) 
		{ 
			continue;
		} 
		else 
		{  
			if(*finfo.fname=='\0')
				continue;
			if(fileIndex>=rowCount)//如果超出最大行数，则跳出
				break;
			//执行了过滤的这几行代码后，将导致USB打开出现FR_NO_ENABLE，原因待查
			//考虑目前用不上此功能，暂时不用
//			if(filterExt!=NULL) 
//			{				
//				char* extName = GetExtName(finfo.fname);
//				if(!StrCMP_n(extName,filterExt,strlen(extName)))
//					continue;
//			}
			strcpy(fileList[fileIndex].FileName,finfo.fname);
			fileList[fileIndex].DateTime=(int)(finfo.fdate<<16|finfo.ftime);
			fileIndex++;		
		}
	}
    res = f_closedir(&dirs);
    res = f_mount(NULL,diskStr, 0);
	return fileIndex;
}

//设置文件的修改日期及时间
FRESULT SetFileDateTime(char* path,DataTimeStruct* dateTime)
{
    FILINFO fno;
    fno.fdate=(WORD)(((dateTime->Year-1980)*512U)| dateTime->Mon*32U |dateTime->Day);
    fno.ftime= (WORD)(dateTime->Hour*2048U | dateTime->Min*32U | dateTime->Sec/2U);
    return f_utime(path,&fno);
}

//按修改时间降序排序比较器
int comp_des(const void*a,const void*b)
{
	return (*(SortFileSturct *)a).DateTime < (*(SortFileSturct *)b).DateTime ? 1 : -1; 
}

//按修改时间升序排序比较器
int comp_asc(const void*a,const void*b)
{
	return (*(SortFileSturct *)a).DateTime > (*(SortFileSturct *)b).DateTime ? 1 : -1; 
}

//按修改时间降序排序,最近修改的放在前面
void ShortFile_TimeDES(SortFileSturct* sortFileList,u8 fileCount)
{
   qsort(sortFileList,fileCount,sizeof(sortFileList[0]),comp_des);
}

//按修改时间升序排序,最近修改的放在后面
void ShortFile_TimeASC(SortFileSturct* sortFileList,u8 fileCount)
{
   qsort(sortFileList,fileCount,sizeof(sortFileList[0]),comp_asc);
}

//按文件名降序排序比较器
int comp_namedes(const void* a,const void* b)
{
	return strcmp((*(SortFileSturct *)b).FileName,(*(SortFileSturct *)a).FileName);
	//return strcmp(b, a);
}

//按文件名升序排序比较器
int comp_nameasc(const void* a,const void* b)
{
	return strcmp((*(SortFileSturct *)a).FileName,(*(SortFileSturct *)b).FileName);
	//return strcmp(a, b);
}

//按文件名降序排序
void ShortFile_NameDES(SortFileSturct* sortFileList,u8 fileCount)
{
   qsort(sortFileList,fileCount,sizeof(sortFileList[0]),comp_namedes);
}

//按文件名升序排序
void ShortFile_NameASC1(SortFileSturct* sortFileList,u8 fileCount)
{
  qsort(sortFileList,fileCount,sizeof(sortFileList[0]),comp_nameasc);
}


//向指定的文件的指定位置开始写入数据。如果起始位置大于文件大小将在文件最后开始写入
//fileName:包括盘符的完全路径,如:0:/CFG/Parameter.txt。
void WriteBytesToFile(char* fileName,u8* data,u32 startIndex,u32 length)
{
    FIL file;
	FATFS fs;    
	u32 reindex;
    char* diskStr=GetDiskStrByFullPath(fileName);
	FRESULT res = f_mount(&fs,diskStr,0);	                        
    res = f_open(&file,fileName,FA_WRITE);
	if (res == FR_OK)
	{	
        if(startIndex>file.obj.objsize)
            startIndex=file.obj.objsize;
		f_lseek(&file,startIndex);
		res = f_write(&file,data,length,&reindex);
	}
	f_close(&file);
    res  = f_mount(NULL, diskStr, 0);
}

//从指定的文件的指定位置开始读出内容。如果起始位置大于文件大小将不会执行读操作
//fileName:包括盘符的完全路径,如:0:/CFG/Parameter.txt。
void ReadBytesFormFile(char* fileName,u8* readBuff,u32 startIndex,u32 readLenght)
{
    FATFS fs;
	FIL	file;
    u32 re;
    char* diskStr=GetDiskStrByFullPath(fileName);
	FRESULT res = f_mount(&fs,diskStr,0);
	res = f_open(&file,fileName,FA_READ);
	if (res == FR_OK)
	{
        if(startIndex<=file.obj.objsize)
        {
			u32 canReadMax=file.obj.objsize-startIndex;//可以读的最大长度
			if(readLenght>canReadMax)
				readLenght=canReadMax;
			f_lseek(&file,startIndex);
            res = f_read(&file,readBuff,readLenght,&re);
        }
	}
	res = f_close(&file);
	res  = f_mount(NULL, diskStr, 0);
}

//删除指定文件相关内容。startIndex:从0开始的索引，开始删除的位置；count:要删除的字节个数
void DeleteBytes(FIL* openedFile,u32 startIndex,u32 count)
{
       u32 re,res;
       u32 fileBytes = openedFile->obj.objsize;//FATFS10:openedFile->fsize;//指定文件的字节长度
       u32 destAddr=startIndex+count;
       u32 moveBytes=fileBytes-destAddr;
       if(fileBytes <= startIndex)
           return;     
       if(fileBytes>destAddr)
       {
		   //启始文件中的堆分配要足够大（Heap_Size），否则出错。粗略测试当文件有6个扇区时堆至少分配0x00006000
		   //u8* tempArray=(u8*) malloc(moveBytes);
		   //u8 tempArray[moveBytes];	
		   f_lseek(openedFile,destAddr);
		   res = f_read(openedFile,_tempArray,moveBytes,&re);
		   f_lseek(openedFile,startIndex);
           res = f_write(openedFile,_tempArray,moveBytes,&re);
		   //free(tempArray);		   
        }
       else //只是删除末尾的
       {
		   //不作任何移动，只是文件大小改变
       }
       openedFile->obj.objsize=fileBytes-count;
}

//u8 _fileBuff[FLASH_SECTOR_SIZE]; 
/*//删除指定文件相关内容。startIndex:从0开始的索引，开始删除的位置；count:要删除的字节个数
void DeleteBytes_fat_bk(FIL* openedFile,u32 startIndex,u32 count)
{
       u32 re,res;
       u32 fileBytes = openedFile->obj.objsize;//FATFS10:openedFile->fsize;//指定文件的字节长度
       u32 destAddr=startIndex+count;
       u32 moveBytes=fileBytes-destAddr;
       u32 _buffLength=sizeof(_fileBuff);
       if(fileBytes <= startIndex)
           return;     
       if(fileBytes>destAddr)
       {
           u32 moveCount =  moveBytes/_buffLength;
           for(u32 i=0;i<moveCount;i++)
           {
               memset(_fileBuff,0,_buffLength);
               u32 offset=i * _buffLength;
               f_lseek(openedFile,destAddr + offset);
               res = f_read(openedFile,_fileBuff,_buffLength,&re);
               f_lseek(openedFile,startIndex + offset);
               res = f_write(openedFile,_fileBuff,_buffLength,&re);
           }
           u32 moveEnd =  moveBytes % _buffLength;
           if(moveEnd>0)
           {
               memset(_fileBuff,0,_buffLength);
               u32 offset=moveCount * _buffLength;
               f_lseek(openedFile,destAddr + offset);
               res = f_read(openedFile,_fileBuff,moveEnd,&re);
               f_lseek(openedFile,startIndex + offset);
               res = f_write(openedFile,_fileBuff,moveEnd,&re);
           }
        }
       else //只是删除末尾的
       {
           count=fileBytes-startIndex;
           memset(_fileBuff,0,count);
           f_lseek(openedFile,startIndex);
           res = f_write(openedFile,_fileBuff,count,&re);
       }
       openedFile->obj.objsize=fileBytes-count;
}  */

//删除指定文件相关内容。startIndex:从0开始的索引，开始删除的位置；count:要删除的字节个数
//fileName:包括盘符的完全路径,如:0:/DATA/NewFile.dat。
void DeleteBytesOnFile(char* fileName,u32 startIndex,u32 count)
{
	FIL file;
	FATFS fs;    
	u32 reindex;
    char* diskStr=GetDiskStrByFullPath(fileName);
	FRESULT res = f_mount(&fs,diskStr,0);	                        
    res = f_open(&file,fileName,FA_READ | FA_WRITE);
	if (res == FR_OK)
	{	
        DeleteBytes(&file,startIndex,count);
	}
	f_close(&file);
    res  = f_mount(NULL, diskStr, 0);
}

//向文件插入数据。startIndex:从0开始的索引，开始插入的位置；count:要插入的字节个数
//内存消耗大，算法简单。并且应该可以减少写入次数，增加使用寿命
void InsetBytes(FIL* openedFile,u8* data,u32 startIndex,u32 count)
{
       u32 re,res;
       u32 fileBytes = openedFile->obj.objsize;
       u32 moveBytes=fileBytes-startIndex;
       if(startIndex>fileBytes)
           return; 
	   //启始文件中的堆分配要足够大（Heap_Size），否则出错。粗略测试当文件有6个扇区时堆至少分配0x00006000
	   //u8* tempArray=(u8*) malloc(moveBytes);
	   //u8 tempArray[moveBytes];
	   if(startIndex<fileBytes)//如果插入位置在中间
       {
		   f_lseek(openedFile,startIndex);
		   res = f_read(openedFile,_tempArray,moveBytes,&re);
		   f_lseek(openedFile,startIndex + count);
           res = f_write(openedFile,_tempArray,moveBytes,&re);
		   f_lseek(openedFile,startIndex);
           res = f_write(openedFile,data,count,&re);
		   //free(tempArray);
	   }
	   else
	   {
		    f_lseek(openedFile,startIndex);
			res = f_write(openedFile,data,count,&re);
	   }
	   openedFile->obj.objsize=fileBytes+count;  
}

/*
//向文件插入数据。startIndex:从0开始的索引，开始插入的位置；count:要插入的字节个数
void InsetBytes_fat_bk(FIL* openedFile,u8* data,u32 startIndex,u32 count)
{
       u32 re,res;
       u32 fileBytes = openedFile->obj.objsize;
       u32 moveBytes=fileBytes-startIndex;
       u32 _buffLength=sizeof(_fileBuff);
       if(fileBytes < startIndex)
           return;  
       if(startIndex<fileBytes)//如果插入位置在中间
       {
           u32 moveCount =  moveBytes /_buffLength;
           for(u32 i=0;i<moveCount;i++)
           {
              memset(_fileBuff,0,_buffLength);
              u32 readAddr=fileBytes-i * _buffLength;
              f_lseek(openedFile,readAddr);
              res = f_read(openedFile,_fileBuff,_buffLength,&re);
              f_lseek(openedFile,readAddr + count);
              res = f_write(openedFile,_fileBuff,_buffLength,&re);
           }
           u32 moveEnd =  moveBytes % _buffLength;
           if(moveEnd>0)
           {        
               u32 tind=fileBytes-moveCount*_buffLength-moveEnd;
               f_lseek(openedFile,startIndex);
               res = f_read(openedFile,_fileBuff,moveBytes,&re);
               f_lseek(openedFile,startIndex+count);
               res = f_write(openedFile,_fileBuff,moveBytes,&re);
           }
        }
       f_lseek(openedFile,startIndex);
       res = f_write(openedFile,data,count,&re);
       openedFile->obj.objsize=fileBytes+count;
}  */

//向文件插入数据。startIndex:从0开始的索引，开始插入的位置；count:要插入的字节个数
//fileName:包括盘符的完全路径,如:0:/DATA/NewFile.dat。
void InsetBytesOnFile(char* fileName,u8* data,u32 startIndex,u32 count)
{
	FIL file;
	FATFS fs;    
	u32 reindex;
    char* diskStr=GetDiskStrByFullPath(fileName);
	FRESULT res = f_mount(&fs,diskStr,0);	                        
    res = f_open(&file,fileName,FA_READ | FA_WRITE);
	if (res == FR_OK)
	{	
        InsetBytes(&file,data,startIndex,count);
	}
	f_close(&file);
    res  = f_mount(NULL, diskStr, 0);
}

//向文件尾添加一行文本内容，文本后面以"/0"结尾
//fileName:包括盘符的完全路径,如:0:/DATA/NewFile.dat。
void AppendCharsToFile(char* fileName,char* data)
{
    FIL file;
	FATFS fs;    
	u32 reindex;
    char* diskStr=GetDiskStrByFullPath(fileName);
	u8 res = f_mount(&fs,diskStr,0);	                        
    res = f_open(&file,fileName,FA_READ | FA_WRITE);
	u32 length = strlen(data);
	if (res == FR_OK)
	{	
        f_lseek(&file,file.obj.objsize);  //FATFS10:file.fsize
		res = f_write(&file,data,length,&reindex);
		if (res == FR_OK)
		{			
			f_close(&file);
			reindex = 0x00;									
		}
	}
    res  = f_mount(NULL, diskStr, 0);
}

//向指定文件的末尾添加字节数据。count:要插入的字节个数
void AppendBytesToFile(FIL* openedFile,u8* data,u32 count)
{
    InsetBytes(openedFile,data, openedFile->obj.objsize,count);
}


char sourDisk[3];//源磁盘
char destDisk[3];//目标磁盘
char sourFilePath[100];//源文件
char destFilePath[100];//目标文件
char copyMsg[50];

//复制一个文件
//sourPathFile:源文件，如：0:/CDATA/abc.txt。 destPathFile：目标文件，如: 1:/CDATA/efd.txt。isCoverFile:是否覆盖现有文件
bool CopyFile(char* sourPathFile,char* destPathFile,bool isCoverFile)
{
	FATFS fs[2];      
    FIL fsrc, fdst;      
    FRESULT res;         
    UINT br, bw;         
	DIR dirs;
	FILINFO finfo;
	strcpy(sourDisk,GetDiskStrByFullPath(sourPathFile));
	strcpy(destDisk,GetDiskStrByFullPath(destPathFile));
	f_mount(&fs[0], sourDisk, 0);
	if(sourDisk[0]!=destDisk[0])//如果不是同一个磁盘
	{
		f_mount(&fs[1], destDisk, 0);
	} 
    
    res = f_open(&fsrc, sourPathFile, FA_READ);//打开源文件
	if (res) return false;
    res = CreateDir_in(destPathFile);//检查并创建目录
    u8 createMode= FA_CREATE_NEW;
    if(isCoverFile)
        createMode=FA_CREATE_ALWAYS;  //覆盖的时候创建模式一定要选这个，否则在写文件时会 res=FR_INVALID_OBJECT(09)
    res = f_open(&fdst, destPathFile, FA_WRITE | createMode);
	if(res==FR_OK ||(res==FR_EXIST && isCoverFile))
	{
		//复制源数据到目标文件
		for (;;) 
		{
			memset(_fileBuff,0,sizeof(_fileBuff));
			res = f_read(&fsrc, _fileBuff, FLASH_SECTOR_SIZE, &br);  
			if (res || br == 0) break;
			res = f_write(&fdst, _fileBuff, br, &bw);          
			if (res || bw < br) break; 
		}
	}
	f_close(&fsrc);
	f_close(&fdst);
    f_mount(NULL, sourDisk, 0);
	if(sourDisk[0]!=destDisk[0])//如果不是同一个磁盘
	{
		f_mount(NULL, destDisk, 0);
	}
	if(res==FR_EXIST && !isCoverFile)//如果有同名文件且不复盖
	{
		return false;
	}
	return true;
}

bool CopyAllFile_Dir(char* sourcDir,char* destDir,bool isCoverFile,OneStrParameterHandler showMSG)
{
    FATFS fs[2];      
    FIL fsrc, fdst;     
    FRESULT res;         
    UINT br, bw;        
	DIR dirs;
	FILINFO finfo;
	
	strcpy(sourDisk,GetDiskStrByFullPath(sourcDir));
	strcpy(destDisk,GetDiskStrByFullPath(destDir));
	res=f_mount(&fs[0], sourDisk, 0);
	if(sourDisk[0]!=destDisk[0])//如果不是同一个磁盘
	{
		res=f_mount(&fs[1], destDisk, 0);
	} 
	
	res = f_opendir(&dirs, sourcDir);//打开源文件夹
	if(res) 
	{
		if(showMSG!=NULL) showMSG("打开源文件夹错误！");
		return false;
	}
    res = CreateDir_in(destDir);//创建目标文件夹
	if(res!=FR_OK && res!=FR_EXIST) 
	{
		if(showMSG!=NULL) showMSG("创建目标文件夹错误！");
		return false;
	}
	
	bool isOK=false;
	while ((f_readdir(&dirs, &finfo) == FR_OK) && finfo.fname[0]) 
	{
		if (finfo.fattrib & AM_DIR) 
		{ 
			continue;
		} 
		else 
		{  
			memset(sourFilePath,0,sizeof(sourFilePath));
			sprintf(sourFilePath,"%s%s%s",sourcDir,"/",finfo.fname);
			res = f_open(&fsrc, sourFilePath, FA_READ | FA_OPEN_EXISTING);//打开源文件
			if (res) continue;
			
			memset(destFilePath,0,sizeof(destFilePath));
			sprintf(destFilePath,"%s%s%s",destDir,"/",finfo.fname);
            u32 createMode= FA_CREATE_NEW;  //此属性当文件存在时不会重新创建，且写文件件时会FR_INVALID_OBJECT。在没有文件时可以正常创建
            if(isCoverFile)
               createMode=FA_CREATE_ALWAYS;  //覆盖的时候创建模式要选这个重新创建一个文件，否则在写文件时会 res=FR_INVALID_OBJECT(09)
			res = f_open(&fdst, destFilePath, FA_WRITE | createMode);
			if(res==FR_OK ||(res==FR_EXIST && isCoverFile))
			{
				if(showMSG!=NULL)
                {                    
                    memset(copyMsg,0,sizeof(copyMsg));
                    sprintf(copyMsg,"%s%s","正在复制：",finfo.fname);					
					showMSG(copyMsg);
                }
				//复制源数据到目标文件
				for (;;) 
				{
					memset(_fileBuff,0,sizeof(_fileBuff));
					res = f_read(&fsrc, _fileBuff, FLASH_SECTOR_SIZE, &br);
					if (res || br == 0) break; // error or eof
					res = f_write(&fdst, _fileBuff, br, &bw);       
					if (res || bw < br) break; // error or disk full
				}
				isOK=true;
			}
			if((res!=FR_OK && res!=FR_EXIST )&& showMSG!=NULL)
			{
				showMSG("读写U盘异常！");
			}
			res = f_close(&fsrc);
			res = f_close(&fdst);
		}
	}
    res = f_closedir(&dirs);
    res = f_mount(NULL, sourDisk, 0);
	if(sourDisk[0]!=destDisk[0])//如果不是同一个磁盘
	{
		res = f_mount(NULL, destDisk, 0);
	} 
	if(isOK)
	{
		if(showMSG!=NULL)
		{
			showMSG("复制完毕，请拨出U盘！");
		}
		return true;
	}
	else
	{
		if(showMSG!=NULL) showMSG("源文件夹无文件！");
	}
	return false;
}
