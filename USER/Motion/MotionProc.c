#include "MotionProc.h"
#include "AppTasks.h"

typedef struct
{
	//�������͡�0������ڶ�����1������˶�,2:�����λ��3��N���Զ���λ��4:�ȴ�ȡ����5:��ʱ��6���������ʼ��,7:���ƶ��������趨������
	u8 MoveType;
	char MoveStr[2];//�����ַ���
}MoveTypeStrStruct;//�������ͽṹ��

const u8 _moveTypeCount=10;//�������͸���
MoveTypeStrStruct _moveStrs[_moveTypeCount]={{OutPortMotion,"OT"},{AxisMove,"AM"},{AxisReset,"AR"},{NReset,"NR"},
											{WaitTakeWork,"WG"},{DelayMotion,"DY"},{AxisOrigin,"AO"},{AxisParaCoord,"AP"},{WaitInport,"WI"},{InportEvent,"IT"}};
OneMoveStruct _analyoms;

char _axissStr[20];//����ŵ��ַ���,�ö��Ÿ���
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
//����һ�������ṹ����һ���������ַ���
char* GetOneMoveStr(OneMoveStruct oms)
{
	memset(_moveStr,0,sizeof(_moveStr));
	if(oms.MotionType==OutPortMotion)//����ڶ���
	{
		char onoff[6]="�ߵ�ƽ";
		if(!oms.OutputPara.IsHV)
			strcpy(onoff,"�͵�ƽ");
		sprintf(_moveStr,"�����%d,%s,��ʱ%gs",(oms.OutputPara.PortIndex+1),onoff,oms.OutputPara.MoveDelay);			
	}
	else if(oms.MotionType==AxisMove)//�������
	{
		u8 count=oms.MotorPara.AxisCount;
		sprintf(_moveStr,"��(%s),�ƶ���(%s)",GetMulitAxisStr(oms.MotorPara.AxisList,count),
			ConvertFloatArrToStr(oms.MotorPara.DestCoord,count));	
	}
	else if(oms.MotionType==AxisReset)//�����λ
	{			
		sprintf(_moveStr,"��(%s),��λ",GetMulitAxisStr(oms.ResetPara.AxisList, oms.ResetPara.AxisCount));
	}
	else if(oms.MotionType==NReset)//N���Զ���λ
	{
		sprintf(_moveStr,"%s","ƽ̨N���Զ���λ");
	}
	else if(oms.MotionType==WaitTakeWork)//�ȴ�ȡ��
	{
		sprintf(_moveStr,"%s","ǰ�ź�ȡʱ,�ȴ�ȡ����");
	}
	else if(oms.MotionType==DelayMotion)//��ʱ
	{
		sprintf(_moveStr,"��ʱ%g��",oms.DelayPara.DelayValue);
	}
	else if(oms.MotionType==AxisOrigin)//�����λ
	{			
		sprintf(_moveStr,"��(%s),�ƶ�����ʼ��",GetMulitAxisStr(oms.MotorToStart.AxisList, oms.MotorToStart.AxisCount));
	}
	else if(oms.MotionType==AxisParaCoord)//���ƶ��������趨������
	{			
//		u8 count=oms.AxisToParaCoord.MotorCount;
//		sprintf(_moveStr,"��(%s),�ƶ��������趨��(%s)",GetMulitAxisStr(oms.AxisToParaCoord.Motors,count),
//			ConvertFloatArrToStr(oms.AxisToParaCoord.DestCoord,count));	
		if(oms.AxisToParaCoord.AxisList[0]!=Y1 && oms.AxisToParaCoord.AxisList[0]!=Y2)//��ʱֻ֧��Y1��Y2
			return NULL;
		float yMoveCoord=Parameter.Y1MoveCoord;
		if(oms.AxisToParaCoord.AxisList[0]==Y2)
			yMoveCoord=Parameter.Y2MoveCoord;
		u8 count=1;//oms.AxisToParaCoord.MotorCount;
		sprintf(_moveStr,"��(%s),�ƶ����趨��(%g)",GetMulitAxisStr(oms.AxisToParaCoord.AxisList,count),yMoveCoord);	
	}
	else if(oms.MotionType==WaitInport)
	{
		char HorL[]="��";
		if(oms.WaitInportPara.InputState)
			strcpy(HorL,"��");
		sprintf(_moveStr,"�ȴ������%dΪ%s,�[%gs]",oms.WaitInportPara.PortIndex+1,HorL,oms.WaitInportPara.WaitDelay);
	}
	else if(oms.MotionType==InportEvent)
	{
		char HorL[]="��";
		if(oms.InportEvent.InputState)
			strcpy(HorL,"��");
		sprintf(_moveStr,"�����%dΪ%s,�����¼�[%s]",oms.InportEvent.PortIndex+1,HorL,oms.InportEvent.Msg);
	}
	return _moveStr;
}

//����һ���ַ�����ת����һ�������ṹ�塣�ַ�������д��󷵻ؿ�
OneMoveStruct* AnalyStrToMoveStruct(char* moveStr,OneStrParameterHandler showErrMsgHandel)
{
	u8 leng=strlen(moveStr);
	bool isShowErr=showErrMsgHandel!=NULL;
	if(moveStr==NULL)
	{
		if(isShowErr)showErrMsgHandel("*�����ַ�����Ϊ��");
		return NULL;
	}
	if(leng<2)
	{
		if(isShowErr)showErrMsgHandel("*�����ַ����ȴ���");
		return NULL;
	}
	char* paraErr="*������������";
	char currMove[2]={0};
	char moveParaStr[100]={0}; 
	CopyBytes((u8*)currMove,0,(u8*)moveStr,2);
	if(StrCMP_n(currMove,_moveStrs[0].MoveStr,2))//����ڶ���
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
				if(isShowErr)showErrMsgHandel("����˿ںų�����Χ��");
				return NULL;
			}
			_analyoms.OutputPara.IsHV=StrCMP_n(ps[1].item,"H",1) || StrCMP_n(ps[1].item,"ON",2);//Ϊ�˼��ݾɰ����ON/OFF
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
	else if(StrCMP_n(currMove,_moveStrs[1].MoveStr,2))//����˶���ָ������
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
					if(isShowErr)showErrMsgHandel("�趨������ֵ������Χ");
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
	else if(StrCMP_n(currMove,_moveStrs[2].MoveStr,2))//�����λ
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
	else if(StrCMP_n(currMove,_moveStrs[3].MoveStr,2))//N���Զ���λ
	{
		_analyoms.MotionType=_moveStrs[3].MoveType;
		return &_analyoms;
	}
	else if(StrCMP_n(currMove,_moveStrs[4].MoveStr,2))//�ȴ�ȡ��
	{
		_analyoms.MotionType=_moveStrs[4].MoveType;
		return &_analyoms;
	}
	else if(StrCMP_n(currMove,_moveStrs[5].MoveStr,2))//��ʱ
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
	else if(StrCMP_n(currMove,_moveStrs[6].MoveStr,2))//����ص���ʼ��
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
	else if(StrCMP_n(currMove,_moveStrs[7].MoveStr,2))//���ƶ����������õ�����
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
			char* errstr="*��ʱֻ֧��Y��Ķ����趨";
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
	else if(StrCMP_n(currMove,_moveStrs[8].MoveStr,2))//�ȴ�������ź�
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
				if(isShowErr)showErrMsgHandel("����˿ںų�����Χ��");
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
	else if(StrCMP_n(currMove,_moveStrs[9].MoveStr,2))//������¼�
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
				if(isShowErr)showErrMsgHandel("����˿ںų�����Χ��");
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
		if(isShowErr)showErrMsgHandel("*�����ؼ��ִ���");
		return NULL;
	}
}

float _startCoords[AxisNum];//����ʼλ�õ������б�
//ִ��һ�鶯��
void RunOneGroupWork(OneMoveStruct* moves,u16 moveCount,LeftRightPlatformEnum platform,OneStrParameterHandler showMSG)
{
	for(u16 i=0;i<moveCount;i++)
	{
		OneMoveStruct oms=moves[i];
		if(showMSG!=NULL)
		{
			showMSG(GetOneMoveStr(oms));
		}
		if(oms.MotionType==OutPortMotion)//����ڶ���
		{			
			bool bl=GetOutPortState_Index(oms.OutputPara.PortIndex);
			if(bl!=oms.OutputPara.IsHV)
			{
				SetOutPortState_Index(oms.OutputPara.PortIndex,oms.OutputPara.IsHV);
				Delay_s_OS(oms.OutputPara.MoveDelay);
			}
		}
		else if(oms.MotionType==AxisMove)//�������
		{
			//MoveToCoord(oms.MotorPara.MotorIndex,oms.MotorPara.DestCoord,true);
			MultiAxisLinkageNoAction(oms.MotorPara.AxisList,oms.MotorPara.DestCoord,oms.MotorPara.AxisCount);
		}
		else if(oms.MotionType==AxisReset)//�����λ
		{			
			MultiAxisMoveToPositionZeroAndMove(oms.ResetPara.AxisList,oms.ResetPara.AxisCount);
		}
		else if(oms.MotionType==NReset)//N���Զ���λ
		{
			AutoReset();
		}
		else if(oms.MotionType==WaitTakeWork)//�ȴ�ȡ��
		{
			WaitGetWorkpiece(platform);
		}
		else if(oms.MotionType==DelayMotion)//��ʱ
		{
			Delay_s_OS(oms.DelayPara.DelayValue);//Ҫע�����ǵ��ò���ϵͳ����ʱ������ڷ������е��ý�����
		}
		else if(oms.MotionType==AxisOrigin)//�ص���ʼ��
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
		else if(oms.MotionType==AxisParaCoord)//���ƶ��������趨����
		{
			AxisIDEnum id=oms.AxisToParaCoord.AxisList[0];
			if(id!=Y1 && id!=Y2)
				return;
			float yMoveCoord=Parameter.Y1MoveCoord;
			if(id==Y2)
				yMoveCoord=Parameter.Y2MoveCoord;
			MoveToCoord(id,yMoveCoord,true);
		}
		else if(oms.MotionType==WaitInport)//�ȴ������
		{
			u16 count=oms.WaitInportPara.WaitDelay*100;//�ȴ����ٴ�10ms
			for(u16 i=0;i<count;i++)
			{
				if(GetInPortState_Index(oms.WaitInportPara.PortIndex)==oms.WaitInportPara.InputState)
					break;
				Delay_ms_OS(10);
			}
		}
		else if(oms.MotionType==InportEvent)//������¼����˴�����Ҫ���κ���
		{
		}
	}
}

//��ƽ̨����˿ǰ�Ķ�������
void WorkBeforMove_Left(void)
{
	RunOneGroupWork(Moves.WorkBeforMoves_Left,Moves.WorkBeforMoveCount_Left,LeftPlatform,NULL);
}

//��ƽ̨������ɺ�Ķ�������
void WorkCompletedMove_Left(void)
{
	RunOneGroupWork(Moves.WorkComplMoves_Left,Moves.WorkComplMoveCount_Left,LeftPlatform,NULL);
}

//��ƽ̨����˿ǰ�Ķ�������
void WorkBeforMove_Right(void)
{
	RunOneGroupWork(Moves.WorkBeforMoves_Right,Moves.WorkBeforMoveCount_Right,RightPlatform,NULL);
}

//��ƽ̨������ɺ�Ķ�������
void WorkCompletedMove_Right(void)
{
	RunOneGroupWork(Moves.WorkComplMoves_Right,Moves.WorkComplMoveCount_Right,RightPlatform,NULL);
}

//��λʱ�Ķ�������
void PositionResetMove(void)
{
	RunOneGroupWork(Moves.ResetMoveMoves,Moves.ResetMoveCount,LeftPlatform,NULL);
}
