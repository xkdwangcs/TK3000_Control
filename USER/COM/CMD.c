#include "CMD.h"
#include "string.h"
#include <stdio.h>
#include "stdlib.h"
#include "CoordFile.h"


//当前使用的串口号
USART_TypeDef *currUSART = USART1;
char* _showFormCmd="ShowForm";
//当前设备状态，作为主动上送及主控查询用
char _currStatus[100];

//设置程序当前状态。statusID：状态号，statusDescribe：状态描述文本
//要注意的是，由于汉字显示要用UTF-8,所以传入的文字信息一定要是UTF-8.否则显示不了
void SetCurrStatus(u16 statusID,char* statusDescribe)
{
	memset(_currStatus,0,sizeof(_currStatus));
	LinkThreeStr(_currStatus,ConvertU32ToAscii(statusID),",",statusDescribe);
}

//RDD相关命令的处理
void RDDProcess(void)
{
	if (StrCMP(_cmdRecvBuff.CmdName, "DEVMODE")) //此条命令必备，联机用
	{		
		SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)(Parameter.ProdModel),sizeof(Parameter.ProdModel));
		return;
	}
  if (StrCMP(_cmdRecvBuff.CmdName, "PARAFILE")) //读取参数文件
	{		
		SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)&Parameter,sizeof(Parameter));
		return;
	}
	if (StrCMP(_cmdRecvBuff.CmdName, "CURRSTATUS")) //读取当前程序状态
	{		
		SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)_currStatus,sizeof(_currStatus));
		return;
	}
	
  
	//memset(_protocolBuff.CmdBuff,0,CMDBuffLeng);//一般查询指令只有指令名，所以放在此处共用。如果有参数就不能放在此处
	//char* cmdAddr=&_protocolBuff.CmdBuff[PackHeadNum];
//	if (StrCMP(_protocolBuff.CmdName, "DEVMODEL")) //此条命令必备，联机用
//	{		
//		LinkThreeStr(cmdAddr,_protocolBuff.CmdName,"=",Parameter.ProdModel);
//	}
//	else if(StrCMP(_protocolBuff.CmdName, "MPMSG"))//铭牌信息
//	{
//		AppendStrAddSplit(cmdAddr, "MPMSG=ProdName|", Parameter.ProdName);
//		AppendStrAddSplit(cmdAddr, ",ProdModel|", Parameter.ProdModel);
//		AppendStrAddSplit(cmdAddr, ",ProduDate|", Parameter.ProduDate);
//		AppendStrAddSplit(cmdAddr, ",ProdID|", Parameter.ProdID);
//		AppendStrAddSplit(cmdAddr, ",Company|", Parameter.Company);
//		AppendStrAddSplit(cmdAddr, ",URL|", Parameter.URL);
//		AppendStrAddSplit(cmdAddr, ",Tel|", Parameter.Tel);
//		AppendStrAddSplit(cmdAddr, ",CompanyAddr|", Parameter.CompanyAddr);		
//	}
//	else if(StrCMP(_protocolBuff.CmdName, "CTMPMSG"))//控制器铭牌信息
//	{
//		AppendStrAddSplit(cmdAddr, "MPMSG=ProdName|", ControlMPMsg.ProdName);
//		AppendStrAddSplit(cmdAddr, ",ProdModel|", ControlMPMsg.ProdModel);
//		AppendStrAddSplit(cmdAddr, ",ProduDate|", ControlMPMsg.ProduDate);
//		AppendStrAddSplit(cmdAddr, ",ProdID|", ControlMPMsg.ProdID);
//		AppendStrAddSplit(cmdAddr, ",Company|", ControlMPMsg.Company);
//		AppendStrAddSplit(cmdAddr, ",URL|", ControlMPMsg.URL);
//		AppendStrAddSplit(cmdAddr, ",Tel|", ControlMPMsg.Tel);
//		AppendStrAddSplit(cmdAddr, ",CompanyAddr|", ControlMPMsg.CompanyAddr);		
//	}
//	ResponseSendData(_rdd, strlen(cmdAddr));
	return;
}

//WRT命令回复OK
void SendWRTOK(void)
{
	SendChars_Pack(_wrt,_cmdRecvBuff.CmdName,"OK");
}

//WRT相关命令的处理
void WRTProcess(void)
{
    if (StrCMP(_cmdRecvBuff.CmdName, "AxisPTRun")) //点动运行
    {		
      u8 count=0;
      LIST* ls=GetLIST((char*)_cmdRecvBuff.CmdData,&count);
      DirectEnum fx=ZFX;
      if(StrCMP_n(ls[1].item,"FFX",3))
        fx=FFX;
      AxisPTRun(GetAxisIDFormStr(ls[0].item),fx);
    }
    if (StrCMP(_cmdRecvBuff.CmdName, "AxisPTStop")) //点动停止
    {		
      AxisPTStop(GetAxisIDFormStr((char*)_cmdRecvBuff.CmdData));
    }
	/*if(StrCMP(_protocolBuff.CmdName, "MPMSG"))//设备铭牌信息设置
	{
		u8 count=0;
		Dictionary* dic=ConvertToDic(_protocolBuff.CmdBuff,&count);
		for(u8 i=0;i<count;i++)
		{
			if(StrCMP(dic[i].key,"ProdName"))
				strcpy(Parameter.ProdName,dic[i].value);
			else if(StrCMP(dic[i].key,"ProdModel"))
				strcpy(Parameter.ProdModel,dic[i].value);
			else if(StrCMP(dic[i].key,"ProduDate"))
				strcpy(Parameter.ProduDate,dic[i].value);
			else if(StrCMP(dic[i].key,"ProdID"))
				strcpy(Parameter.ProdID,dic[i].value);
			else if(StrCMP(dic[i].key,"Company"))
				strcpy(Parameter.Company,dic[i].value);
			else if(StrCMP(dic[i].key,"URL"))
				strcpy(Parameter.URL,dic[i].value);
			else if(StrCMP(dic[i].key,"Tel"))
				strcpy(Parameter.Tel,dic[i].value);
			else if(StrCMP(dic[i].key,"CompanyAddr"))
				strcpy(Parameter.CompanyAddr,dic[i].value);
		}
		WriteParameter(NULL);
		SendWRTOK();
	}
	else if(StrCMP(_protocolBuff.CmdName, "CTMPMSG"))//控制器铭牌信息设置
	{
		u8 count=0;
		Dictionary* dic=ConvertToDic(_protocolBuff.CmdBuff,&count);
		for(u8 i=0;i<count;i++)
		{
			if(StrCMP(dic[i].key,"ProdName"))
				strcpy(ControlMPMsg.ProdName,dic[i].value);
			else if(StrCMP(dic[i].key,"ProdModel"))
				strcpy(ControlMPMsg.ProdModel,dic[i].value);
			else if(StrCMP(dic[i].key,"ProduDate"))
				strcpy(ControlMPMsg.ProduDate,dic[i].value);
			else if(StrCMP(dic[i].key,"ProdID"))
				strcpy(ControlMPMsg.ProdID,dic[i].value);
			else if(StrCMP(dic[i].key,"Company"))
				strcpy(ControlMPMsg.Company,dic[i].value);
			else if(StrCMP(dic[i].key,"URL"))
				strcpy(ControlMPMsg.URL,dic[i].value);
			else if(StrCMP(dic[i].key,"Tel"))
				strcpy(ControlMPMsg.Tel,dic[i].value);
			else if(StrCMP(dic[i].key,"CompanyAddr"))
				strcpy(ControlMPMsg.CompanyAddr,dic[i].value);
		}
		ControlMPMsg.IsWrited=true;
		WriteControlMPMsg();
		SendWRTOK();
	}
	else if(StrCMP(_protocolBuff.CmdName, "TIMEJZ"))//时间校准
	{
		char* data=_protocolBuff.CmdBuff;
		u16 leng=strlen(data);
		bool bl=true;
		if(!(leng!=19 || data[4]!='-'|| data[10]!=' ' || data[16]!=':'))
			bl = TimeAdjust(data);		
		SendWRTOK();
	}*/
}

//OUT命令回复OK
void SendOUTOK(char *cmdName)
{
	SendChars_Pack(_out,_cmdRecvBuff.CmdName,"OK");
}

//OUT相关命令的处理
void OUTProcess(void)
{
	
}

//控制器通过SRQ请求显示某一窗体
//void ShowForm_SRQ(char* formName)
//{
//  SendChars_Pack(_srq,"ShowForm",formName);
//}

//控制器通过SRQ请求显示USB功能窗体
void ShowUSBFuncForm(void)
{
  SendChars_Pack(_srq,_showFormCmd,"USBFunc_Ctr");
}

//显示系统复位窗体,stopSource:指停止源是屏幕上急停还是机台上按钮急停
void ShowSysResetForm(StopSource stopSource)
{
  char cmdBuff[20]={0};
  AppendStrAddSplit(cmdBuff, "SysResetForm,", ConvertU32ToAscii(stopSource));
  SendChars_Pack(_srq,_showFormCmd,cmdBuff);
}

