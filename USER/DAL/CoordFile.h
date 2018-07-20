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

#define _fileMaxCount 20 //����ļ���  
#define _coordMaxCount 500 //���������
	 
typedef enum
{
	LeftPlatform=0,//��ƽ̨
	RightPlatform,//��ƽ̨
}LeftRightPlatformEnum;//����ƽ̨ö��      

typedef struct
{
	char FileName[32];	//�ļ�����	
	u16 CoordCount;		//��������
	u16 LeftCoordCount;	//��ƽ̨������
	u16 RightCoordCount;//��ƽ̨������
    int DateTime; //������
} CoordFileAttri;//�����ļ����Խṹ��

typedef struct
{
    u16 FileCount;
    CoordFileAttri FileList[_fileMaxCount];    
}FileListStruct; //�ļ��б��������

extern FileListStruct _fileList;//���⿪�ţ����ڵ�ַ����
extern CoordStruct _coordListLeft[_coordMaxCount];//���⿪�ţ����ڵ�ַ����

//��ǰ�����ļ�
extern CoordFileAttri _workFile;
//��ʼ���ļ���¼
void IniFileRecord(void);
//����һ�����ļ�
void CreateFile(char* fileName);
//��ȡ�ļ���Ϣ�б�
FileListStruct* ReadFileList();
//ɾ��ָ�����ļ�
void DeleteFile(CoordFileAttri file);
//��ʼ�������ļ�
void IniCoordFile(void);
//����ָ���ļ�Ϊ��ǰ�����ļ�
void SetWorkFile(CoordFileAttri file);
//����ָ���ļ����ļ���
void Rename(CoordFileAttri file, char* newFileName);
void SaveFileAttri(FIL* openedFile,CoordFileAttri cfi);

//��ȡ��ǰ�����ļ�����,
void ReadCurrCoordFileAttri(void);
//��ȡָ���ļ��е������б�
// platform:��ƽ̨����ƽ̨��*coordLeng�����ã�ֵΪ�������
CoordStruct* GetCoordList(LeftRightPlatformEnum platform, u16* coordLeng);
//��ָ�����ļ����һ������,TIME6��Ӱ��д�룬��������
// platform:��ƽ̨����ƽ̨��saveCoord��Ҫ���������
void AddCoordinate(LeftRightPlatformEnum platform,MultiAxisCoordStruct saveCoord);
//��ָ�����ļ���Ӷ������
void AddMultiCoord(LeftRightPlatformEnum platform,CoordStruct* coordList,u16 count);
//����һ�����ꡣ
// platform:��ƽ̨����ƽ̨��MultiAxisCoordStruct coord��coordIndex:��0��ʼ����������
void InsertCoordinate(LeftRightPlatformEnum platform,MultiAxisCoordStruct saveCoord,u16 coordIndex);
//ɾ��һ�����ꡣ
// platform:��ƽ̨����ƽ̨��coordIndex:��0��ʼ����������
void DeleteCoordinate(LeftRightPlatformEnum platform,u16 coordIndex);
//ɾ��ָ��ƽ̨����������
void DeletePlatCoord(LeftRightPlatformEnum platform);
//�޸�һ�����ꡣ
// platform:��ƽ̨����ƽ̨��coordIndex:��0��ʼ����������;saveCoord:�±��������
void UpdateCoordinate(LeftRightPlatformEnum platform,u16 coordIndex, MultiAxisCoordStruct* saveCoord);
//�޸Ķ��������ꡣ
// platform:��ƽ̨����ƽ̨��saveCoords:�±��������
void UpdateCoords(LeftRightPlatformEnum platform,CoordStruct* saveCoords,u16 coordCount);


#ifdef __cplusplus
}
#endif

#endif
