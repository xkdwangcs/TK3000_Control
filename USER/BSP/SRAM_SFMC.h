#ifndef __SRAM_SFMC_H
#define __SRAM_SFMC_H

#ifdef __cplusplus
 extern "C" {
#endif 

/******************
* IS61LV25616
********************/
//��IS61LV25616/IS62WV25616,��ַ�߷�ΧΪA0~A17 
//��IS61LV51216/IS62WV51216,��ַ�߷�ΧΪA0~A18
	 
#include "stm32f4xx.h"
#include "absacc.h"
#include "string.h"
#include "stdbool.h"

/*****************
	 Bank1_NORSRAM1:0x60000000
	 Bank1_NORSRAM2:0x64000000
	 Bank1_NORSRAM3:0x68000000
	 Bank1_NORSRAM4:0x6C000000	 
*****************/
	 
#define ExtSRAM_ADDR  	0x60000000 //��չ�ڴ���ʼ��ַ
#define ExtSRAM_SIZE		0x80000 	//��չ�ڴ��С��16*256KbIt,512KB	
	 
//��ʼ���ⲿ��չ�洢��
void InitExtSRAM(void);	

//������չ�ڴ�
bool ExtSRAMTest(void);	 
	 
#ifdef __cplusplus
}
#endif

#endif
