#include "MotionProc.h"
#include "AppTasks.h"

typedef struct
{
	//动作类型。0：输出口动作，1：电机运动,2:电机复位，3：N次自动复位，4:等待取件，5:延时，6：电机到起始点,7:轴移动到参数设定的坐标
	u8 MoveType;
	char MoveStr[2];//动作字符串
}MoveTypeStrStruct;//动作类型结构体

const u8 _moveTypeCount=10;//动作类型个数
MoveTypeStrStruct _moveStrs[_moveTypeCount]={{OutPortMotion,"OT"},{AxisMove,"AM"},{AxisReset,"AR"},{NReset,"NR"},
											{WaitTakeWork,"WG"},{DelayMotion,"DY"},{AxisOrigin,"AO"},{AxisParaCoord,"AP"},{WaitInport,"WI"},{InportEvent,"IT"}};
OneMoveStruct _analyoms;

char _axissStr[20];//多轴号的字符串,用逗号隔开
char* GetMulitAxisStr(AxisIDEnum* axiss,u8 count)
{
	memset(_axissStr,0,sizeof(_axissStr));
	for(u8 i=0;i<count;i++)
	{
		u8 leng=strlen(_axissStr);
		sprintf(_axissStr+leng,"%s,",GetAxisStrFromID(axiss[i]));
	}
	u8 leng=strlen(_axissStr);
	_axissStr[leng-1]=0x00;
	return _axissStr;
}

char _moveStr[40];
//根据一个动作结构生成一条解释性字符串
char* GetOneMoveStr(OneMoveStruct oms)
{
	memset(_moveStr,0,sizeof(_moveStr));
	if(oms.MotionType==OutPortMotion)//输出口动作
	{
		char onoff[6]="高电平";
		if(!oms.OutputPara.IsHV)
			strcpy(onoff,"低电平");
		sprintf(_moveStr,"输出口%d,%s,延时%gs",(oms.OutputPara.PortIndex+1),onoff,oms.OutputPara.MoveDelay);			
	}
	else if(oms.MotionType==AxisMove)//电机动作
	{
		u8 count=oms.MotorPara.AxisCount;
		sprintf(_moveStr,"轴(%s),移动到(%s)",GetMulitAxisStr(oms.MotorPara.AxisList,count),
			ConvertFloatArrToStr(oms.MotorPara.DestCoord,count));	
	}
	else if(oms.MotionType==AxisReset)//电机复位
	{			
		sprintf(_moveStr,"轴(%s),复位",GetMulitAxisStr(oms.ResetPara.AxisList, oms.ResetPara.AxisCount));
	}
	else if(oms.MotionType==NReset)//N次自动复位
	{
		sprintf(_moveStr,"%s","平台N次自动复位");
	}
	else if(oms.MotionType==WaitTakeWork)//等待取件
	{
		sprintf(_moveStr,"%s","前放后取时,等待取工件");
	}
	else if(oms.MotionType==DelayMotion)//延时
	{
		sprintf(_moveStr,"延时%g秒",oms.DelayPara.DelayValue);
	}
	else if(oms.MotionType==AxisOrigin)//电机复位
	{			
		sprintf(_moveStr,"轴(%s),移动到起始点",GetMulitAxisStr(oms.MotorToStart.AxisList, oms.MotorToStart.AxisCount));
	}
	else if(oms.MotionType==AxisParaCoord)//轴移动到参数设定的坐标
	{			
//		u8 count=oms.AxisToParaCoord.MotorCount;
//		sprintf(_moveStr,"轴(%s),移动到参数设定点(%s)",GetMulitAxisStr(oms.AxisToParaCoord.Motors,count),
//			ConvertFloatArrToStr(oms.AxisToParaCoord.DestCoord,count));	
		if(oms.AxisToParaCoord.AxisList[0]!=Y1 && oms.AxisToParaCoord.AxisList[0]!=Y2)//暂时只支持Y1及Y2
			return NULL;
		float yMoveCoord=Parameter.Y1MoveCoord;
		if(oms.AxisToParaCoord.AxisList[0]==Y2)
			yMoveCoord=Parameter.Y2MoveCoord;
		u8 count=1;//oms.AxisToParaCoord.MotorCount;
		sprintf(_moveStr,"轴(%s),移动到设定点(%g)",GetMulitAxisStr(oms.AxisToParaCoord.AxisList,count),yMoveCoord);	
	}
	else if(oms.MotionType==WaitInport)
	{
		char HorL[]="低";
		if(oms.WaitInportPara.InputState)
			strcpy(HorL,"高");
		sprintf(_moveStr,"等待输入口%d为%s,最长[%gs]",oms.WaitInportPara.PortIndex+1,HorL,oms.WaitInportPara.WaitDelay);
	}
	else if(oms.MotionType==InportEvent)
	{
		char HorL[]="低";
		if(oms.InportEvent.InputState)
			strcpy(HorL,"高");
		sprintf(_moveStr,"输入口%d为%s,引发事件[%s]",oms.InportEvent.PortIndex+1,HorL,oms.InportEvent.Msg);
	}
	return _moveStr;
}

//分析一个字符串，转换成一个动作结构体。字符串如果有错误返回空
OneMoveStruct* AnalyStrToMoveStruct(char* moveStr,OneStrParameterHandler showErrMsgHandel)
{
	u8 leng=strlen(moveStr);
	bool isShowErr=showErrMsgHandel!=NULL;
	if(moveStr==NULL)
	{
		if(isShowErr)showErrMsgHandel("*动作字符不能为空");
		return NULL;
	}
	if(leng<2)
	{
		if(isShowErr)showErrMsgHandel("*动作字符长度错误");
		return NULL;
	}
	char* paraErr="*动作参数错误";
	char currMove[2]={0};
	char moveParaStr[100]={0}; 
	CopyBytes((u8*)currMove,0,(u8*)moveStr,2);
	if(StrCMP_n(currMove,_moveStrs[0].MoveStr,2))//输出口动作
	{		
		_analyoms.MotionType=_moveStrs[0].MoveType;
		if(moveStr[2]=='(' && moveStr[leng-1]==')')
		{
			CopyBytes1((u8*)moveParaStr,0,(u8*)moveStr,3,leng-4);
			u8 paraCount=0;
			LIST* ps= SplitString(moveParaStr,",",&paraCount);
			if(paraCount!=3)
			{
				if(isShowErr)showErrMsgHandel(paraErr);
				return NULL;
			}
			_analyoms.OutputPara.PortIndex=(ConvertAsciiTou32(ps[0].item)-1);
			if(_analyoms.OutputPara.PortIndex<0 || _analyoms.OutputPara.PortIndex>OutPortNum)
			{
				if(isShowErr)showErrMsgHandel("输出端口号超出范围！");
				return NULL;
			}
			_analyoms.OutputPara.IsHV=StrCMP_n(ps[1].item,"H",1) || StrCMP_n(ps[1].item,"ON",2);//为了兼容旧版参数ON/OFF
			_analyoms.OutputPara.MoveDelay=ConvertAsciiToFloat(ps[2].item);
			return &_analyoms;
		}
		else
		{
			if(isShowErr)showErrMsgHandel(paraErr);
			return NULL;
		}
		return &_analyoms;
	}
	else if(StrCMP_n(currMove,_moveStrs[1].MoveStr,2))//电机运动到指定坐标
	{
		_analyoms.MotionType=_moveStrs[1].MoveType;
		if(moveStr[2]=='(' && moveStr[leng-1]==')')
		{
			CopyBytes1((u8*)moveParaStr,0,(u8*)moveStr,2,leng-2);
			u8 paraCount=0;
			LIST* ps= SplitString(moveParaStr,"-",&paraCount);
			if(paraCount!=2)
			{
				if(isShowErr)showErrMsgHandel(paraErr);
				return NULL;
			}
			char motorStr[20]={0};
			u8 tleng=strlen(ps[0].item);
			CopyBytes1((u8*)motorStr,0,(u8*)ps[0].item,1,tleng-2);
			char coordStr[80]={0};
			tleng=strlen(ps[1].item);
			CopyBytes1((u8*)coordStr,0,(u8*)ps[1].item,1,tleng-2);			
			ps= SplitString(motorStr,",",&paraCount);
			_analyoms.MotorPara.AxisCount=paraCount;
			for(u8 i=0;i<paraCount;i++)
			{
				_analyoms.MotorPara.AxisList[i]=GetAxisIDFormStr(ps[i].item);
			}
			ps= SplitString(coordStr,",",&paraCount);
			for(u8 i=0;i<paraCount;i++)
			{
				float tf=ConvertAsciiToFloat(ps[i].item);
				if(tf<0 || tf>Parameter.AxisLength[_analyoms.MotorPara.AxisList[i]])
				{
					if(isShowErr)showErrMsgHandel("设定的坐标值超出范围");
					return NULL;
				}
				_analyoms.MotorPara.DestCoord[i]=tf;
			}
			return &_analyoms;
		}
		else
		{
			if(isShowErr)showErrMsgHandel(paraErr);
			return NULL;
		}
		return &_analyoms;
	}
	else if(StrCMP_n(currMove,_moveStrs[2].MoveStr,2))//电机复位
	{
		_analyoms.MotionType=_moveStrs[2].MoveType;
		if(moveStr[2]=='(' && moveStr[leng-1]==')')
		{
			CopyBytes1((u8*)moveParaStr,0,(u8*)moveStr,3,leng-4);
			u8 paraCount=0;
			LIST* ps= SplitString(moveParaStr,",",&paraCount);
			_analyoms.ResetPara.AxisCount=paraCount;
			for(u8 i=0;i<paraCount;i++)
			{
				_analyoms.ResetPara.AxisList[i]=GetAxisIDFormStr(ps[i].item);
			}
			return &_analyoms;
		}
		else
		{
			if(isShowErr)showErrMsgHandel(paraErr);
			return NULL;
		}
	}
	else if(StrCMP_n(currMove,_moveStrs[3].MoveStr,2))//N次自动复位
	{
		_analyoms.MotionType=_moveStrs[3].MoveType;
		return &_analyoms;
	}
	else if(StrCMP_n(currMove,_moveStrs[4].MoveStr,2))//等待取件
	{
		_analyoms.MotionType=_moveStrs[4].MoveType;
		return &_analyoms;
	}
	else if(StrCMP_n(currMove,_moveStrs[5].MoveStr,2))//延时
	{
		_analyoms.MotionType=_moveStrs[5].MoveType;
		if(moveStr[2]=='(' && moveStr[leng-1]==')')
		{
			CopyBytes1((u8*)moveParaStr,0,(u8*)moveStr,3,leng-4);
			_analyoms.DelayPara.DelayValue=ConvertAsciiToFloat(moveParaStr);
			return &_analyoms;
		}
		else
		{
			if(isShowErr)showErrMsgHandel(paraErr);
			return NULL;
		}
	}
	else if(StrCMP_n(currMove,_moveStrs[6].MoveStr,2))//电机回到起始点
	{
		_analyoms.MotionType=_moveStrs[6].MoveType;
		if(moveStr[2]=='(' && moveStr[leng-1]==')')
		{
			CopyBytes1((u8*)moveParaStr,0,(u8*)moveStr,3,leng-4);
			u8 paraCount=0;
			LIST* ps= SplitString(moveParaStr,",",&paraCount);
			_analyoms.MotorToStart.AxisCount=paraCount;
			for(u8 i=0;i<paraCount;i++)
			{
				_analyoms.MotorToStart.AxisList[i]=GetAxisIDFormStr(ps[i].item);
			}
			return &_analyoms;
		}
		else
		{
			if(isShowErr)showErrMsgHandel(paraErr);
			return NULL;
		}
	}
	else if(StrCMP_n(currMove,_moveStrs[7].MoveStr,2))//轴移动到参数设置的坐标
	{
		_analyoms.MotionType=_moveStrs[7].MoveType;
		if(moveStr[2]=='(' && moveStr[leng-1]==')')
		{
			CopyBytes1((u8*)moveParaStr,0,(u8*)moveStr,3,leng-4);
			u8 paraCount=0;
			LIST* ps= SplitString(moveParaStr,",",&paraCount);
			_analyoms.AxisToParaCoord.AxisCount=paraCount;
			for(u8 i=0;i<paraCount;i++)
			{
				_analyoms.AxisToParaCoord.AxisList[i]=GetAxisIDFormStr(ps[i].item);
			}
			AxisIDEnum mid = _analyoms.AxisToParaCoord.AxisList[0];
			char* errstr="*暂时只支持Y轴的动作设定";
			if(mid!=Y1 && mid!=Y2)
			{
				if(isShowErr)
					showErrMsgHandel(errstr);
				return NULL;
			}
			if(_analyoms.AxisToParaCoord.AxisCount>1 && isShowErr)
			{
				showErrMsgHandel(errstr);
			}
			return &_analyoms;
		}
		else
		{
			if(isShowErr)showErrMsgHandel(paraErr);
			return NULL;
		}
	}
	else if(StrCMP_n(currMove,_moveStrs[8].MoveStr,2))//等待输入口信号
	{		
		_analyoms.MotionType=_moveStrs[8].MoveType;
		if(moveStr[2]=='(' && moveStr[leng-1]==')')
		{
			CopyBytes1((u8*)moveParaStr,0,(u8*)moveStr,3,leng-4);
			u8 paraCount=0;
			LIST* ps= SplitString(moveParaStr,",",&paraCount);
			if(paraCount!=3)
			{
				if(isShowErr)showErrMsgHandel(paraErr);
				return NULL;
			}
			_analyoms.WaitInportPara.PortIndex=(ConvertAsciiTou32(ps[0].item)-1);
			if(_analyoms.WaitInportPara.PortIndex<0 || _analyoms.WaitInportPara.PortIndex>InPortNum)
			{
				if(isShowErr)showErrMsgHandel("输入端口号超出范围！");
				return NULL;
			}
			_analyoms.WaitInportPara.InputState=!StrCMP_n(ps[1].item,"L",1);//L,H
			_analyoms.WaitInportPara.WaitDelay=ConvertAsciiToFloat(ps[2].item);
			return &_analyoms;
		}
		else
		{
			if(isShowErr)showErrMsgHandel(paraErr);
			return NULL;
		}
		return &_analyoms;
	}
	else if(StrCMP_n(currMove,_moveStrs[9].MoveStr,2))//输入口事件
	{		
		_analyoms.MotionType=_moveStrs[9].MoveType;
		if(moveStr[2]=='(' && moveStr[leng-1]==')')
		{
			CopyBytes1((u8*)moveParaStr,0,(u8*)moveStr,3,leng-4);
			u8 paraCount=0;
			LIST* ps= SplitString(moveParaStr,",",&paraCount);
			if(paraCount!=3)
			{
				if(isShowErr)showErrMsgHandel(paraErr);
				return NULL;
			}
			_analyoms.InportEvent.PortIndex=(ConvertAsciiTou32(ps[0].item)-1);
			if(_analyoms.InportEvent.PortIndex<0 || _analyoms.InportEvent.PortIndex>InPortNum)
			{
				if(isShowErr)showErrMsgHandel("输入端口号超出范围！");
				return NULL;
			}
			_analyoms.InportEvent.InputState=!StrCMP_n(ps[1].item,"L",1);//L,H
			strcpy(_analyoms.InportEvent.Msg,ps[2].item);
			return &_analyoms;
		}
		else
		{
			if(isShowErr)showErrMsgHandel(paraErr);
			return NULL;
		}
		return &_analyoms;
	}
	else
	{
		if(isShowErr)showErrMsgHandel("*动作关键字错误");
		return NULL;
	}
}

float _startCoords[AxisNum];//到起始位置的坐标列表
//执行一组动作
void RunOneGroupWork(OneMoveStruct* moves,u16 moveCount,LeftRightPlatformEnum platform,OneStrParameterHandler showMSG)
{
	for(u16 i=0;i<moveCount;i++)
	{
		OneMoveStruct oms=moves[i];
		if(showMSG!=NULL)
		{
			showMSG(GetOneMoveStr(oms));
		}
		if(oms.MotionType==OutPortMotion)//输出口动作
		{			
			bool bl=GetOutPortState_Index(oms.OutputPara.PortIndex);
			if(bl!=oms.OutputPara.IsHV)
			{
				SetOutPortState_Index(oms.OutputPara.PortIndex,oms.OutputPara.IsHV);
				Delay_s_OS(oms.OutputPara.MoveDelay);
			}
		}
		else if(oms.MotionType==AxisMove)//电机动作
		{
			//MoveToCoord(oms.MotorPara.MotorIndex,oms.MotorPara.DestCoord,true);
			MultiAxisLinkageNoAction(oms.MotorPara.AxisList,oms.MotorPara.DestCoord,oms.MotorPara.AxisCount);
		}
		else if(oms.MotionType==AxisReset)//电机复位
		{			
			MultiAxisMoveToPositionZeroAndMove(oms.ResetPara.AxisList,oms.ResetPara.AxisCount);
		}
		else if(oms.MotionType==NReset)//N次自动复位
		{
			AutoReset();
		}
		else if(oms.MotionType==WaitTakeWork)//等待取件
		{
			WaitGetWorkpiece(platform);
		}
		else if(oms.MotionType==DelayMotion)//延时
		{
			Delay_s_OS(oms.DelayPara.DelayValue);//要注意这是调用操作系统的延时，如果在非任务中调用将出错
		}
		else if(oms.MotionType==AxisOrigin)//回到起始点
		{
			for(u8 i=0;i<oms.MotorToStart.AxisCount;i++)
			{
				AxisIDEnum id=oms.MotorToStart.AxisList[i];
				if(id==Z)
				{
					_startCoords[i]=Parameter.WorkHeight;
				}
				else
				{
					_startCoords[i]=GetScrewOneAxisCoord(id);//Parameter.GetScrewPara.Coord[id];
				}
			}			
			MultiAxisLinkageNoAction(oms.MotorToStart.AxisList,_startCoords,oms.MotorToStart.AxisCount);
		}
		else if(oms.MotionType==AxisParaCoord)//轴移动到参数设定坐标
		{
			AxisIDEnum id=oms.AxisToParaCoord.AxisList[0];
			if(id!=Y1 && id!=Y2)
				return;
			float yMoveCoord=Parameter.Y1MoveCoord;
			if(id==Y2)
				yMoveCoord=Parameter.Y2MoveCoord;
			MoveToCoord(id,yMoveCoord,true);
		}
		else if(oms.MotionType==WaitInport)//等待输入口
		{
			u16 count=oms.WaitInportPara.WaitDelay*100;//等待多少次10ms
			for(u16 i=0;i<count;i++)
			{
				if(GetInPortState_Index(oms.WaitInportPara.PortIndex)==oms.WaitInportPara.InputState)
					break;
				Delay_ms_OS(10);
			}
		}
		else if(oms.MotionType==InportEvent)//输入口事件，此处不需要做任何事
		{
		}
	}
}

//左平台打螺丝前的动作流程
void WorkBeforMove_Left(void)
{
	RunOneGroupWork(Moves.WorkBeforMoves_Left,Moves.WorkBeforMoveCount_Left,LeftPlatform,NULL);
}

//左平台工件完成后的动作流程
void WorkCompletedMove_Left(void)
{
	RunOneGroupWork(Moves.WorkComplMoves_Left,Moves.WorkComplMoveCount_Left,LeftPlatform,NULL);
}

//右平台打螺丝前的动作流程
void WorkBeforMove_Right(void)
{
	RunOneGroupWork(Moves.WorkBeforMoves_Right,Moves.WorkBeforMoveCount_Right,RightPlatform,NULL);
}

//右平台工件完成后的动作流程
void WorkCompletedMove_Right(void)
{
	RunOneGroupWork(Moves.WorkComplMoves_Right,Moves.WorkComplMoveCount_Right,RightPlatform,NULL);
}

//复位时的动作流程
void PositionResetMove(void)
{
	RunOneGroupWork(Moves.ResetMoveMoves,Moves.ResetMoveCount,LeftPlatform,NULL);
}
