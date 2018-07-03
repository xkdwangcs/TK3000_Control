#ifndef _LCDPROTOCOL_H
#define _LCDPROTOCOL_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"
//#include "COM_1.h"

#define _com3BuffLeng 2048 	  //���ڻ��棨�������͡����ա�Э�����������ֽ���,64�ֽڶ����ԣ�ֻҪ���������ĳ��ȼ��ɡ��������Ч�ʻ��Щ

#define COMName3				USART3
#define SendDMAChannel3         DMA_Channel_4
#define SendDMAStream3          DMA1_Stream3
#define SendDMAFlag3            DMA_FLAG_TCIF3
#define SendDMAIRQ3             DMA1_Stream3_IRQn 

#define RecvDMAChannel3         DMA_Channel_4
#define RecvDMAStream3          DMA1_Stream1
#define RecvDMAFlag3            DMA_FLAG_TCIF1
#define RecvDMAIRQ3             DMA1_Stream1_IRQn
	 
extern u8 _DMASendBuff_COM3[_com3BuffLeng];//DMA���ͻ���
extern u8 _DMARecvBuff_COM3[_com3BuffLeng];//DMA���ջ���
extern u16 _sendStartIndex_COM3;//������ʼλ�õ�ַ
extern u16 _waitSendCount_COM3;//�ȴ����͵ĸ���
extern bool _isSendOK_COM3;//һ�η����Ƿ����

extern u8 _lcdSendCmdBuf[_com3BuffLeng];//��������棬�����ַ��
extern u8 _lcdRecvCmdBuf[_com3BuffLeng];//��������棬�����ַ��

//��ʼ������3,LCD
void InitUSART_COM3(void);
//����DMA����
void StartSendToDMA_COM3(void);
//�򴮿�3��������
void SendBytes_COM3(u8* datas, u16 count);


#ifdef __cplusplus
}
#endif

#endif
