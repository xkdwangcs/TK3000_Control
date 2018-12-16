#ifndef __ProtocolBASE_H
#define __ProtocolBASE_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stm32f4xx.h>
#include "string.h"
//#include "COM_1.h"
#include "COM_3.h"
	 
#define _srq "SRQ"
#define _rdd "RDD"
#define _wrt "WRT"
#define _cdr "CDR"
#define _cdw "CDW"
#define _err "ERR"
#define _out "OUT"


//命令符
typedef enum
{
	RMR,	//run motor 设置电机直接执行
	SMR, //SetMotor
	RUN, //根据set motor 命令,执行
	SPD,  //set speed max
	SPT,	//set point speed
	RPM,	//run point 轴+距离
	OUT_NULL = 0xFF
} MOT_ET;

typedef enum
{
	RS232,
	WIFI,
}CommChannelEnum;//通信通道类型

#pragma pack(1)  
typedef struct
{
	u8 Head;					//头字节
	u16 Length;					//整包的长度，包手包头、包尾
	u8 DestAddr;				//目的地址
	u8 SourAddr;				//源地址
  u8 PackNum;             //分包数，为0时表示没分包
	u8 PackID;					    //包号
	char FunCode[3];			//功能码
	char CmdName[10];			//命令名，固定长度，不足用0补充
	u8 CmdData[_com3BuffLeng];	//命令数据据，发送时包括了CRC、尾
  u16 DataLeng;            //数据域实际长度
} ProtocolBuffStruct;//通信缓存结构
#pragma pack()  

//命令接收缓存
extern ProtocolBuffStruct _cmdRecvBuff;
//命令发送缓存
extern ProtocolBuffStruct _cmdSendBuff;
extern char *_cmdERR;
extern char *_crcERR;
extern char *_tailERR;
extern char *_funcERR;

//计算CRC16
u16 CRC16ByPoly(u8 *data, u16 startIndex, u16 length);
//组包，返回组好包的包长
u16 Package(char* func,char* cmdName,u8* cmdData,u16 dataLeng);
//COM1的解包
void DisPackage(void);

//命令解析,recvPack：接收到；commChannel：当前接收的通信通道
void CMDAnalysis(void);
//作为应答的数据发送
void ResponseSendData(char *func, u16 cmdDataLeng);
//向指定的串口发送组包后的数据
void SendBytes_Pack(char* func,char* cmdName, u8* cmdData,u16 cmdLeng);
//向指定的串口发送组包后的数据
void SendChars_Pack(char* func,char* cmdName, char* cmdData);

#ifdef __cplusplus
}
#endif
#endif
