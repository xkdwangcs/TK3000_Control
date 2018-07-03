#ifndef _COM_H
#define _COM_H

#ifdef __cplusplus
 extern "C" {
#endif 
	 
#include "stm32f4xx.h"
#include "stdbool.h"

#define _comBuffLeng 512 	  //串口缓存（包括发送、接收、协议解析）最大字节数,64字节都可以，只要大于最长命令的长度即可。但缓存大效率会高些
extern u8 _DMASendBuff[];     //DMA发送缓存
extern u8 _DMARecvBuff[];     //DMA接收缓存
extern u16 _sendStartIndex;   //发送起始位置地址
extern u16 _waitSendCount;    //等待发送的个数
extern bool _isSendOK;	      //一次发送是否完毕
	 
#define COMName1				         USART1
#define SendDMAChannel1          DMA_Channel_4
#define SendDMAStream1           DMA2_Stream7
#define SendDMAFlag1             DMA_FLAG_TCIF7      
#define SendDMAIRQ1              DMA2_Stream7_IRQn 

#define RecvDMAChannel1          DMA_Channel_4
#define RecvDMAStream1           DMA2_Stream2
#define RecvDMAFlag1     	       DMA_FLAG_TCIF2
#define RecvDMAIRQ1      	       DMA2_Stream2_IRQn

//启动DMA发送
void StartSendToDMA_COM1(void);
//初始串口1
void InitUSART_COM1(void);
//向指定的串口发送一个字节数组
void SendBytes_COM1(u8* data,u16 count);

#ifdef __cplusplus
}
#endif

#endif
