#include "WIFI.h"
#include "Delay.h"

struct WIFI_Fram _wifi_Fram={0};

// ：对WF-ESP8266模块发送AT指令 ，还要判断执行是否成功，暂时不做
void Send_ESP_ATCMD(char* cmd,u32 waittime)
{
	int leng=strlen(cmd);
	char para[leng+3];
	strcpy(para, cmd);
	para[leng]=0x0D;
	para[leng+1]=0x0A;//2016.02.18加，原来没有导致设置不成功，可能是有些厂家的模块不用此字节吧
	para[leng+2]='\0';
	_wifi_Fram.Length = 0;               //从新开始接收新的数据包
	_wifi_Fram.FinishFlag = 0;
	SendChars_COM2(para);
	//free(para);
	Delay_ms ( waittime ); //延时
}

//
bool IsHaveWIFI(void)
{
	Send_ESP_ATCMD("AT",100);
}

//重启WF-ESP8266模块
void ESP8266_Rst ()
{
	Send_ESP_ATCMD("AT+RST",1000);
}

//设置模块的应用模式
void ESP8266_AP_STA()
{
	Send_ESP_ATCMD("AT+CWMODE=3",2000);//AP模式，据说最稳定，3也可以
	Send_ESP_ATCMD("AT+RST",1000);
}

//WF-ESP8266模块开启服务器模式
bool ESP8266_StartServer ()
{
	//Send_ESP_ATCMD("AT",100);
	//Send_ESP_ATCMD("AT+CIOBAUD=57600",100);//经测试，12F最高工作在57600波特率下,115200时接收异常
	//ESP8266_AP_STA();	
	Send_ESP_ATCMD("AT+CIPMUX=1",100);//设置为多路模式，服务器下必须为多路
	Send_ESP_ATCMD("AT+CIPSERVER=1,333",100);//设置为服务器，端口默认为333
	//	
	//Send_ESP_ATCMD("AT+CIPSTO=2800",100);//设置服务器超时时0-28800:最大8小时
	return 1;
}

void SendCharsPack_WIFI(char *func, char *cmdData)
{
//	u16 leng=PackageChars(_oneSendPackBuff,func,cmdData);
//	char cStr [50];
//	sprintf (cStr, "AT+CIPSEND=0,%d", leng);
//	Send_ESP_ATCMD(cStr,100);
//	SendBytes_COM2(_oneSendPackBuff_COM2,leng);
}

void SendBytesPack_WIFI(char *func,u8 *cmdData,u16 cmdleng)
{
//	u16 leng =PackageBytes(_oneSendPackBuff_COM2,func,cmdData,cmdleng);
//	char cStr [50];
//	sprintf (cStr, "AT+CIPSEND=0,%d", leng);
//	Send_ESP_ATCMD(cStr,100);
//	SendBytes_COM2(_oneSendPackBuff_COM2,leng);
}


/*
* 函数名：ESP8266_ReceiveString
* 描述  ：WF-ESP8266模块接收字符串
* 输入  ：isTC，声明是否已使能了透传模式
* 返回  : 接收到的字符串首地址
* 调用  ：被外部调用
*/
char * ESP8266_ReceiveString (bool isTC )
{
	char * pRecStr = 0;	
	_wifi_Fram.Length = 0;
	_wifi_Fram.FinishFlag = 0;
	while ( ! _wifi_Fram.FinishFlag );
	_wifi_Fram.Data[_wifi_Fram.Length ] = '\0';

	if ( isTC )
	{
		if (strstr (_wifi_Fram.Data, ">" ) )
			pRecStr = _wifi_Fram.Data;
	}	
	else 
	{
		if ( strstr (_wifi_Fram.Data, "+IPD" ) )
			pRecStr = _wifi_Fram.Data;
	}
	return pRecStr;	
}

