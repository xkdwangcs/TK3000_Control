#ifndef _LCDPROTOCOL_H
#define _LCDPROTOCOL_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
//#include "COM_1.h"

#define _com3BuffLeng 2048 	  //串口缓存（包括发送、接收、协议解析）最大字节数,64字节都可以，只要大于最长命令的长度即可。但缓存大效率会高些

#define COMName3				USART3
#define SendDMAChannel3         DMA_Channel_4
#define SendDMAStream3          DMA1_Stream3
#define SendDMAFlag3            DMA_FLAG_TCIF3
#define SendDMAIRQ3             DMA1_Stream3_IRQn 

#define RecvDMAChannel3         DMA_Channel_4
#define RecvDMAStream3          DMA1_Stream1
#define RecvDMAFlag3            DMA_FLAG_TCIF1
#define RecvDMAIRQ3             DMA1_Stream1_IRQn
	 
extern u8 _DMASendBuff_COM3[_com3BuffLeng];//DMA发送缓存
extern u8 _DMARecvBuff_COM3[_com3BuffLeng];//DMA接收缓存
extern u16 _sendStartIndex_COM3;//发送起始位置地址
extern u16 _waitSendCount_COM3;//等待发送的个数
extern bool _isSendOK_COM3;//一次发送是否完毕

extern u8 _lcdSendCmdBuf[_com3BuffLeng];//发送命令缓存，计算地址用
extern u8 _lcdRecvCmdBuf[_com3BuffLeng];//接收命令缓存，计算地址用

//初始化串口3,LCD
void InitUSART_COM3(void);
//启动DMA发送
void StartSendToDMA_COM3(void);
//向串口3发送数据
void SendBytes_COM3(u8* datas, u16 count);


#ifdef __cplusplus
}
#endif

#endif
