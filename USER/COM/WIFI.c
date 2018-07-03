#include "WIFI.h"
#include "Delay.h"

struct WIFI_Fram _wifi_Fram={0};

// ����WF-ESP8266ģ�鷢��ATָ�� ����Ҫ�ж�ִ���Ƿ�ɹ�����ʱ����
void Send_ESP_ATCMD(char* cmd,u32 waittime)
{
	int leng=strlen(cmd);
	char para[leng+3];
	strcpy(para, cmd);
	para[leng]=0x0D;
	para[leng+1]=0x0A;//2016.02.18�ӣ�ԭ��û�е������ò��ɹ�����������Щ���ҵ�ģ�鲻�ô��ֽڰ�
	para[leng+2]='\0';
	_wifi_Fram.Length = 0;               //���¿�ʼ�����µ����ݰ�
	_wifi_Fram.FinishFlag = 0;
	SendChars_COM2(para);
	//free(para);
	Delay_ms ( waittime ); //��ʱ
}

//
bool IsHaveWIFI(void)
{
	Send_ESP_ATCMD("AT",100);
}

//����WF-ESP8266ģ��
void ESP8266_Rst ()
{
	Send_ESP_ATCMD("AT+RST",1000);
}

//����ģ���Ӧ��ģʽ
void ESP8266_AP_STA()
{
	Send_ESP_ATCMD("AT+CWMODE=3",2000);//APģʽ����˵���ȶ���3Ҳ����
	Send_ESP_ATCMD("AT+RST",1000);
}

//WF-ESP8266ģ�鿪��������ģʽ
bool ESP8266_StartServer ()
{
	//Send_ESP_ATCMD("AT",100);
	//Send_ESP_ATCMD("AT+CIOBAUD=57600",100);//�����ԣ�12F��߹�����57600��������,115200ʱ�����쳣
	//ESP8266_AP_STA();	
	Send_ESP_ATCMD("AT+CIPMUX=1",100);//����Ϊ��·ģʽ���������±���Ϊ��·
	Send_ESP_ATCMD("AT+CIPSERVER=1,333",100);//����Ϊ���������˿�Ĭ��Ϊ333
	//	
	//Send_ESP_ATCMD("AT+CIPSTO=2800",100);//���÷�������ʱʱ0-28800:���8Сʱ
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
* ��������ESP8266_ReceiveString
* ����  ��WF-ESP8266ģ������ַ���
* ����  ��isTC�������Ƿ���ʹ����͸��ģʽ
* ����  : ���յ����ַ����׵�ַ
* ����  �����ⲿ����
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

