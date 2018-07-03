#include "CMD.h"
#include "string.h"
#include <stdio.h>
#include "stdlib.h"
#include "CoordFile.h"


//��ǰʹ�õĴ��ں�
USART_TypeDef *currUSART = USART1;
char* _showFormCmd="ShowForm";
//��ǰ�豸״̬����Ϊ�������ͼ����ز�ѯ��
char _currStatus[100];

//���ó���ǰ״̬��statusID��״̬�ţ�statusDescribe��״̬�����ı�
//Ҫע����ǣ����ں�����ʾҪ��UTF-8,���Դ����������Ϣһ��Ҫ��UTF-8.������ʾ����
void SetCurrStatus(u16 statusID,char* statusDescribe)
{
	memset(_currStatus,0,sizeof(_currStatus));
	LinkThreeStr(_currStatus,ConvertU32ToAscii(statusID),",",statusDescribe);
}

//RDD�������Ĵ���
void RDDProcess(void)
{
	if (StrCMP(_cmdRecvBuff.CmdName, "DEVMODE")) //��������ر���������
	{		
		SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)(Parameter.ProdModel),sizeof(Parameter.ProdModel));
		return;
	}
  if (StrCMP(_cmdRecvBuff.CmdName, "PARAFILE")) //��ȡ�����ļ�
	{		
		SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)&Parameter,sizeof(Parameter));
		return;
	}
	if (StrCMP(_cmdRecvBuff.CmdName, "CURRSTATUS")) //��ȡ��ǰ����״̬
	{		
		SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)_currStatus,sizeof(_currStatus));
		return;
	}
	
  
	//memset(_protocolBuff.CmdBuff,0,CMDBuffLeng);//һ���ѯָ��ֻ��ָ���������Է��ڴ˴����á�����в����Ͳ��ܷ��ڴ˴�
	//char* cmdAddr=&_protocolBuff.CmdBuff[PackHeadNum];
//	if (StrCMP(_protocolBuff.CmdName, "DEVMODEL")) //��������ر���������
//	{		
//		LinkThreeStr(cmdAddr,_protocolBuff.CmdName,"=",Parameter.ProdModel);
//	}
//	else if(StrCMP(_protocolBuff.CmdName, "MPMSG"))//������Ϣ
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
//	else if(StrCMP(_protocolBuff.CmdName, "CTMPMSG"))//������������Ϣ
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

//WRT����ظ�OK
void SendWRTOK(void)
{
	SendChars_Pack(_wrt,_cmdRecvBuff.CmdName,"OK");
}

//WRT�������Ĵ���
void WRTProcess(void)
{
    if (StrCMP(_cmdRecvBuff.CmdName, "AxisPTRun")) //�㶯����
    {		
      u8 count=0;
      LIST* ls=GetLIST((char*)_cmdRecvBuff.CmdData,&count);
      DirectEnum fx=ZFX;
      if(StrCMP_n(ls[1].item,"FFX",3))
        fx=FFX;
      AxisPTRun(GetAxisIDFormStr(ls[0].item),fx);
    }
    if (StrCMP(_cmdRecvBuff.CmdName, "AxisPTStop")) //�㶯ֹͣ
    {		
      AxisPTStop(GetAxisIDFormStr((char*)_cmdRecvBuff.CmdData));
    }
	/*if(StrCMP(_protocolBuff.CmdName, "MPMSG"))//�豸������Ϣ����
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
	else if(StrCMP(_protocolBuff.CmdName, "CTMPMSG"))//������������Ϣ����
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
	else if(StrCMP(_protocolBuff.CmdName, "TIMEJZ"))//ʱ��У׼
	{
		char* data=_protocolBuff.CmdBuff;
		u16 leng=strlen(data);
		bool bl=true;
		if(!(leng!=19 || data[4]!='-'|| data[10]!=' ' || data[16]!=':'))
			bl = TimeAdjust(data);		
		SendWRTOK();
	}*/
}

//OUT����ظ�OK
void SendOUTOK(char *cmdName)
{
	SendChars_Pack(_out,_cmdRecvBuff.CmdName,"OK");
}

//OUT�������Ĵ���
void OUTProcess(void)
{
	
}

//������ͨ��SRQ������ʾĳһ����
//void ShowForm_SRQ(char* formName)
//{
//  SendChars_Pack(_srq,"ShowForm",formName);
//}

//������ͨ��SRQ������ʾUSB���ܴ���
void ShowUSBFuncForm(void)
{
  SendChars_Pack(_srq,_showFormCmd,"USBFunc_Ctr");
}

//��ʾϵͳ��λ����,stopSource:ָֹͣԴ����Ļ�ϼ�ͣ���ǻ�̨�ϰ�ť��ͣ
void ShowSysResetForm(StopSource stopSource)
{
  char cmdBuff[20]={0};
  AppendStrAddSplit(cmdBuff, "SysResetForm,", ConvertU32ToAscii(stopSource));
  SendChars_Pack(_srq,_showFormCmd,cmdBuff);
}

