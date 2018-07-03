#include "MotionProcFile.h"
#include "string.h"
#include "FileOperate.h"
#include "PublicFunction.h"
#include "MemoryAlloc.h"
#include "LogFile.h"
#include "MainForm.h"

//暂时不采取字符串的方式来配置
//char* _workBeforStr="Y=150,O11_ON=200,O11_OFF=100,OUT_ON=300";
//MovesStruct Moves;//内存不够，放到CCM内存中
//动作配置文件
char* _moveFileName="0:/MOT/MotionProc.mot";
MovesStruct Moves __at(MovesBuffAddr);
MovesStruct _iniMoves __at(MovesBKBuffAddr);
bool _isIniMotionFile=false;//是否初始化了动作文件

//初始化默认动作文件值
void IniDefaultMove(void)
{
	strcpy(_iniMoves.MoveName,"默认的流程文件");
	strcpy(_iniMoves.ParaVer,"ver1.02");
	/*************左平台配置*************/
	memset(_iniMoves.WorkBeforMoves_Left,0,sizeof(_iniMoves.WorkBeforMoves_Left));
	//打螺丝前的流程配置，默认无动作
	_iniMoves.WorkBeforMoveCount_Left=0;	//无动作
	
	//打完螺丝后的流程配置
	OneMoveStruct oms;
	u16 omsLeng=sizeof(oms);
	oms.MotionType=AxisOrigin;
	oms.MotorToStart.AxisList[0]=X;
	oms.MotorToStart.AxisList[1]=Z;
	oms.MotorToStart.AxisCount=2;
	CopyBytes((u8*)&_iniMoves.WorkComplMoves_Left[0],0,(u8*)&oms,omsLeng);//第一个动作
	
	_iniMoves.WorkComplMoves_Left[1].MotionType=WaitTakeWork;//第二个动作
	
	memset((u8*)&oms,0,omsLeng);
	oms.MotionType=AxisOrigin;
	oms.MotorToStart.AxisList[0]=Y1;
	oms.MotorToStart.AxisCount=1;
	CopyBytes((u8*)&_iniMoves.WorkComplMoves_Left[2],0,(u8*)&oms,omsLeng);//第三个动作
	
	_iniMoves.WorkComplMoves_Left[3].MotionType=NReset;//第四个动作
	_iniMoves.WorkComplMoveCount_Left=4;
	/*********************************/

	/*************右平台配置*************/
	memset(_iniMoves.WorkBeforMoves_Right,0,sizeof(_iniMoves.WorkBeforMoves_Right));
	//打螺丝前的流程配置，默认无动作
	_iniMoves.WorkBeforMoveCount_Right=0;	
	//打完螺丝后的流程配置
	memset((u8*)&oms,0,omsLeng);
	oms.MotionType=AxisOrigin;
	oms.MotorToStart.AxisList[0]=X;
	oms.MotorToStart.AxisList[1]=Z;
	oms.MotorToStart.AxisCount=2;
	CopyBytes((u8*)&_iniMoves.WorkComplMoves_Right[0],0,(u8*)&oms,omsLeng);//第一个动作
	
	_iniMoves.WorkComplMoves_Right[1].MotionType=WaitTakeWork;//第二个动作
	
	memset((u8*)&oms,0,omsLeng);
	oms.MotionType=AxisOrigin;
	oms.MotorToStart.AxisList[0]=Y2;
	oms.MotorToStart.AxisCount=1;
	CopyBytes((u8*)&_iniMoves.WorkComplMoves_Right[2],0,(u8*)&oms,omsLeng);//第三个动作
	
	_iniMoves.WorkComplMoves_Right[3].MotionType=NReset;//第四个动作
	_iniMoves.WorkComplMoveCount_Right=4;
	/*********************************/
	
	//复位流程配置，不用分左右平台 
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
	
	//输入口事件配置，默认无动作
	memset(_iniMoves.InportEvents,0,sizeof(_iniMoves.InportEvents));	
	_iniMoves.InportEventCount=0;	
}

//初始化动作配置文件
void IniMoveProcFile(void)
{
	IniDefaultMove();
	u32 size=sizeof(Moves);
	memset((u8*)&Moves,0,size);
	if(CreateFileOnDisk(_moveFileName,false)!=FR_EXIST)//如果文件原来不存在
    { 		
		//WriteLog("动作文件不存在，写入默认!");
		_isIniMotionFile=true;
		WriteBytesToFile(_moveFileName,(u8*) &_iniMoves,0,size);  
    }
	ReadBytesFormFile(_moveFileName,(u8*) &Moves,0,size);
	if(!StrCMP_n(_iniMoves.ParaVer,Moves.ParaVer,7))//判断是否要升级参数,也可能是向下调整参数
	{
		//WriteLog("动作文件版本不同，写入默认!");
		_isIniMotionFile=true;
		WriteBytesToFile(_moveFileName,(u8*) &_iniMoves,0,size);
		ReadBytesFormFile(_moveFileName,(u8*) &Moves,0,size);      
	}
}

//提示动作文件初始化信息，并写日志
void ShowMotionIniMsg(void)
{
	if(_isIniMotionFile)
	{
		ShowWorkMsg("写入了默认的动作文件!");
		//WriteLog("写入了默认的动作文件!");
		_isIniMotionFile=false;
	}
}

//读取动作文件
void ReadMoveFile(void)
{
	ReadBytesFormFile(_moveFileName,(u8*) &Moves,0,sizeof(Moves));
}

//保存流程文件
void SaveMoveFile(void)
{
	WriteBytesToFile(_moveFileName,(u8*) &Moves,0,sizeof(Moves));
}

//恢复出厂默认的动作流程
void WritDefaultMoveProc(void)
{
	IniDefaultMove();
	WriteBytesToFile(_moveFileName,(u8*) &_iniMoves,0,sizeof(_iniMoves));
	ReadBytesFormFile(_moveFileName,(u8*) &Moves,0,sizeof(Moves));	
}


