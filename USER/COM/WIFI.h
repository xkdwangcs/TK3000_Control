#ifndef __WIFI_H
#define __WIFI_H


#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include <stdbool.h>
#include "WIFIConfig.h"
#include "string.h"

#define Rec_MaxLeng     100   //最大接收缓存字节数,这个要去掉

	//* 数据类型 */
	typedef enum{
		STA,
		AP,
		STA_AP  
	} ENUM_Net_ModeTypeDef;


	typedef enum{
		enumTCP,
		enumUDP,
	} ENUM_NetPro_TypeDef;


	typedef enum{
		Multiple_ID_0 = 0,
		Multiple_ID_1 = 1,
		Multiple_ID_2 = 2,
		Multiple_ID_3 = 3,
		Multiple_ID_4 = 4,
		Single_ID_0 = 5,
	} ENUM_ID_NO_TypeDef;


	typedef enum{
		OPEN = 0,
		WEP = 1,
		WPA_PSK = 2,
		WPA2_PSK = 3,
		WPA_WPA2_PSK = 4,
	} ENUM_AP_PsdMode_TypeDef;

	//wifi接收到数据帧
	extern struct WIFI_Fram
	{
		char Data[Rec_MaxLeng];
		u16 Length;
		u8 FinishFlag;
	}_wifi_Fram;

	//重启WF-ESP8266模块
	void ESP8266_Rst (void);
	//打开模块为服务端模式
	bool ESP8266_StartServer (void);

	//向wifi发送数据
	void SendData_wifi_char(char* data);
	void SendData_wifi_byte(u8* data,u16 leng);

	//向WIFI发送组包后的数据
	void SendCharsPack_WIFI(char *func, char *cmdData);
	//向WIFI发送一个字节数组
	void SendBytesPack_WIFI(char *func,u8 *data,u16 leng);

#ifdef __cplusplus
}
#endif

#endif