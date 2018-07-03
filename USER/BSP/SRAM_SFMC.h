#ifndef __SRAM_SFMC_H
#define __SRAM_SFMC_H

#ifdef __cplusplus
 extern "C" {
#endif 

/******************
* IS61LV25616
********************/
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
	 
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
	 
#define ExtSRAM_ADDR  	0x60000000 //扩展内存起始地址
#define ExtSRAM_SIZE		0x80000 	//扩展内存大小：16*256KbIt,512KB	
	 
//初始化外部扩展存储器
void InitExtSRAM(void);	

//测试扩展内存
bool ExtSRAMTest(void);	 
	 
#ifdef __cplusplus
}
#endif

#endif
