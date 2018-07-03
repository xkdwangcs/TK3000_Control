#ifndef __WIFI_CONFIG_H
#define	__WIFI_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx.h"
#include "stdbool.h"

#define COMName2				    USART2
#define SendDMAChannel2             DMA_Channel_4
#define SendDMAStream2              DMA1_Stream6
#define SendDMAFlag2                DMA_FLAG_TCIF6
#define SendDMAIRQ2                 DMA1_Stream6_IRQn 

#define RecvDMAChannel2             DMA_Channel_4
#define RecvDMAStream2              DMA1_Stream5
#define RecvDMAFlag2     		    DMA_FLAG_TCIF5
#define RecvDMAIRQ2      		    DMA1_Stream5_IRQn


	//����DMA����
	void StartSendToDMA_COM2(void);
	//��ʼ������2������WIFI
	void InitUSART_COM2(void);
	//�򴮿ڷ����ַ�����,Ҫ��"\0"��β
	void SendChars_COM2(char* data);
	//�򴮿ڷ���һ���ֽ�����
	void SendBytes_COM2(u8* data,u16 count);

#ifdef __cplusplus
}
#endif

#endif    /* __WIFI_CONFIG_H */
