#include "CMD.h"
#include "string.h"
#include <stdio.h>
#include "stdlib.h"
#include "CoordFile.h"
#include "Register.h"


//��ǰʹ�õĴ��ں�
USART_TypeDef *currUSART = USART1;
char* _showFormCmd="ShowForm";
//��ǰ�豸״̬����Ϊ�������ͼ����ز�ѯ��
char _statuStr[100];
LoopDataStruct _loopData; 

//���ó���ǰ״̬��statusID��״̬�ţ�statusDescribe��״̬�����ı�
//Ҫע����ǣ����ں�����ʾҪ��UTF-8,���Դ����������Ϣһ��Ҫ��UTF-8.������ʾ����
void SetCurrStatus(DeviceStatusEnum devStatus,char* statusDescribe)
{
	memset(_statuStr,0,sizeof(_statuStr));
	LinkThreeStr(_statuStr,ConvertU32ToAscii(devStatus),",",statusDescribe);
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
		SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)_statuStr,sizeof(_statuStr));
		return;
	}
	if (StrCMP(_cmdRecvBuff.CmdName, "SYSPARA")) //��ȡϵͳ����
	{		
		SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)&SysParameter,sizeof(SysParameter));
		return;
	}
    if(StrCMP(_cmdRecvBuff.CmdName, "LOOPDATA")) //��ȡѭ��������
    {
        _loopData.RealCoord=*GetRealTimeCoord();
        _loopData.DataTime=*GetCurrDataTime();
        SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)&_loopData,sizeof(_loopData));
		return;
    }
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
    if (StrCMP(_cmdRecvBuff.CmdName, "RegisterDevice")) //�豸ע��
    {		
      RegisterDevice((char*)_cmdRecvBuff.CmdData);
    }
}

//���������豸״̬
void DeviceStatusSRQ(DeviceStatusEnum devStatus,char* statusDescribe)
{
  SetCurrStatus(devStatus,statusDescribe);
  SendBytes_Pack(_srq,"CURRSTATUS",(u8*)_statuStr,sizeof(_statuStr));
}


