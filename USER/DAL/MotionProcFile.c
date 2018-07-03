#include "MotionProcFile.h"
#include "string.h"
#include "FileOperate.h"
#include "PublicFunction.h"
#include "MemoryAlloc.h"
#include "LogFile.h"
#include "MainForm.h"

//��ʱ����ȡ�ַ����ķ�ʽ������
//char* _workBeforStr="Y=150,O11_ON=200,O11_OFF=100,OUT_ON=300";
//MovesStruct Moves;//�ڴ治�����ŵ�CCM�ڴ���
//���������ļ�
char* _moveFileName="0:/MOT/MotionProc.mot";
MovesStruct Moves __at(MovesBuffAddr);
MovesStruct _iniMoves __at(MovesBKBuffAddr);
bool _isIniMotionFile=false;//�Ƿ��ʼ���˶����ļ�

//��ʼ��Ĭ�϶����ļ�ֵ
void IniDefaultMove(void)
{
	strcpy(_iniMoves.MoveName,"Ĭ�ϵ������ļ�");
	strcpy(_iniMoves.ParaVer,"ver1.02");
	/*************��ƽ̨����*************/
	memset(_iniMoves.WorkBeforMoves_Left,0,sizeof(_iniMoves.WorkBeforMoves_Left));
	//����˿ǰ���������ã�Ĭ���޶���
	_iniMoves.WorkBeforMoveCount_Left=0;	//�޶���
	
	//������˿�����������
	OneMoveStruct oms;
	u16 omsLeng=sizeof(oms);
	oms.MotionType=AxisOrigin;
	oms.MotorToStart.AxisList[0]=X;
	oms.MotorToStart.AxisList[1]=Z;
	oms.MotorToStart.AxisCount=2;
	CopyBytes((u8*)&_iniMoves.WorkComplMoves_Left[0],0,(u8*)&oms,omsLeng);//��һ������
	
	_iniMoves.WorkComplMoves_Left[1].MotionType=WaitTakeWork;//�ڶ�������
	
	memset((u8*)&oms,0,omsLeng);
	oms.MotionType=AxisOrigin;
	oms.MotorToStart.AxisList[0]=Y1;
	oms.MotorToStart.AxisCount=1;
	CopyBytes((u8*)&_iniMoves.WorkComplMoves_Left[2],0,(u8*)&oms,omsLeng);//����������
	
	_iniMoves.WorkComplMoves_Left[3].MotionType=NReset;//���ĸ�����
	_iniMoves.WorkComplMoveCount_Left=4;
	/*********************************/

	/*************��ƽ̨����*************/
	memset(_iniMoves.WorkBeforMoves_Right,0,sizeof(_iniMoves.WorkBeforMoves_Right));
	//����˿ǰ���������ã�Ĭ���޶���
	_iniMoves.WorkBeforMoveCount_Right=0;	
	//������˿�����������
	memset((u8*)&oms,0,omsLeng);
	oms.MotionType=AxisOrigin;
	oms.MotorToStart.AxisList[0]=X;
	oms.MotorToStart.AxisList[1]=Z;
	oms.MotorToStart.AxisCount=2;
	CopyBytes((u8*)&_iniMoves.WorkComplMoves_Right[0],0,(u8*)&oms,omsLeng);//��һ������
	
	_iniMoves.WorkComplMoves_Right[1].MotionType=WaitTakeWork;//�ڶ�������
	
	memset((u8*)&oms,0,omsLeng);
	oms.MotionType=AxisOrigin;
	oms.MotorToStart.AxisList[0]=Y2;
	oms.MotorToStart.AxisCount=1;
	CopyBytes((u8*)&_iniMoves.WorkComplMoves_Right[2],0,(u8*)&oms,omsLeng);//����������
	
	_iniMoves.WorkComplMoves_Right[3].MotionType=NReset;//���ĸ�����
	_iniMoves.WorkComplMoveCount_Right=4;
	/*********************************/
	
	//��λ�������ã����÷�����ƽ̨ 
	memset((u8*)&oms,0,omsLeng);
	oms.MotionType=AxisReset;
	oms.ResetPara.AxisList[0]=Z;
	oms.ResetPara.AxisCount=1;
	CopyBytes((u8*)&_iniMoves.ResetMoveMoves[0],0,(u8*)&oms,omsLeng);
	
	memset((u8*)&oms,0,omsLeng);
	oms.MotionType=AxisReset;
	oms.ResetPara.AxisList[0]=X;
	oms.ResetPara.AxisList[1]=Y1;
	oms.ResetPara.AxisList[2]=Y2;
	oms.ResetPara.AxisCount=3;
	CopyBytes((u8*)&_iniMoves.ResetMoveMoves[1],0,(u8*)&oms,omsLeng);
	_iniMoves.ResetMoveCount=2;
	
	//������¼����ã�Ĭ���޶���
	memset(_iniMoves.InportEvents,0,sizeof(_iniMoves.InportEvents));	
	_iniMoves.InportEventCount=0;	
}

//��ʼ�����������ļ�
void IniMoveProcFile(void)
{
	IniDefaultMove();
	u32 size=sizeof(Moves);
	memset((u8*)&Moves,0,size);
	if(CreateFileOnDisk(_moveFileName,false)!=FR_EXIST)//����ļ�ԭ��������
    { 		
		//WriteLog("�����ļ������ڣ�д��Ĭ��!");
		_isIniMotionFile=true;
		WriteBytesToFile(_moveFileName,(u8*) &_iniMoves,0,size);  
    }
	ReadBytesFormFile(_moveFileName,(u8*) &Moves,0,size);
	if(!StrCMP_n(_iniMoves.ParaVer,Moves.ParaVer,7))//�ж��Ƿ�Ҫ��������,Ҳ���������µ�������
	{
		//WriteLog("�����ļ��汾��ͬ��д��Ĭ��!");
		_isIniMotionFile=true;
		WriteBytesToFile(_moveFileName,(u8*) &_iniMoves,0,size);
		ReadBytesFormFile(_moveFileName,(u8*) &Moves,0,size);      
	}
}

//��ʾ�����ļ���ʼ����Ϣ����д��־
void ShowMotionIniMsg(void)
{
	if(_isIniMotionFile)
	{
		ShowWorkMsg("д����Ĭ�ϵĶ����ļ�!");
		//WriteLog("д����Ĭ�ϵĶ����ļ�!");
		_isIniMotionFile=false;
	}
}

//��ȡ�����ļ�
void ReadMoveFile(void)
{
	ReadBytesFormFile(_moveFileName,(u8*) &Moves,0,sizeof(Moves));
}

//���������ļ�
void SaveMoveFile(void)
{
	WriteBytesToFile(_moveFileName,(u8*) &Moves,0,sizeof(Moves));
}

//�ָ�����Ĭ�ϵĶ�������
void WritDefaultMoveProc(void)
{
	IniDefaultMove();
	WriteBytesToFile(_moveFileName,(u8*) &_iniMoves,0,sizeof(_iniMoves));
	ReadBytesFormFile(_moveFileName,(u8*) &Moves,0,sizeof(Moves));	
}


