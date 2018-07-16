#include "CMD.h"
#include "string.h"
#include <stdio.h>
#include "stdlib.h"
#include "CoordFile.h"
#include "Register.h"


//当前使用的串口号
USART_TypeDef *currUSART = USART1;
char* _showFormCmd="ShowForm";
//当前设备状态，作为主动上送及主控查询用
char _statuStr[100];
LoopDataStruct _loopData; 

//设置程序当前状态。statusID：状态号，statusDescribe：状态描述文本
//要注意的是，由于汉字显示要用UTF-8,所以传入的文字信息一定要是UTF-8.否则显示不了
void SetCurrStatus(DeviceStatusEnum devStatus,char* statusDescribe)
{
	memset(_statuStr,0,sizeof(_statuStr));
	LinkThreeStr(_statuStr,ConvertU32ToAscii(devStatus),",",statusDescribe);
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
		SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)_statuStr,sizeof(_statuStr));
		return;
	}
	if (StrCMP(_cmdRecvBuff.CmdName, "SYSPARA")) //读取系统参数
	{		
		SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)&SysParameter,sizeof(SysParameter));
		return;
	}
    if(StrCMP(_cmdRecvBuff.CmdName, "LOOPDATA")) //读取循环的数据
    {
        _loopData.RealCoord=*GetRealTimeCoord();
        _loopData.DataTime=*GetCurrDataTime();
        SendBytes_Pack(_rdd,_cmdRecvBuff.CmdName,(u8*)&_loopData,sizeof(_loopData));
		return;
    }
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
    if (StrCMP(_cmdRecvBuff.CmdName, "RegisterDevice")) //设备注册
    {		
      RegisterDevice((char*)_cmdRecvBuff.CmdData);
    }
}

//主动上送设备状态
void DeviceStatusSRQ(DeviceStatusEnum devStatus,char* statusDescribe)
{
  SetCurrStatus(devStatus,statusDescribe);
  SendBytes_Pack(_srq,"CURRSTATUS",(u8*)_statuStr,sizeof(_statuStr));
}


