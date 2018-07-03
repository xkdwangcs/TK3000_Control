#ifndef __MEMORYALLOC_H
#define __MEMORYALLOC_H

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
#include "SRAM_SFMC.h"
#include "MotionProcFile.h"
#include "CoordFile.h"
#include "SCurve_Table.h"
#include "PublicFunction.h"
#include "COM_1.h"
		 
	 
/*************BKPSRAM****************/	 
//ʹ�õı��ݼĴ��������4K��
#define FirmwareUpdateTagAddr 		BKPSRAM_BASE 	//�̼����ء�������־�ĵ�ַ�����ڱ��ݼĴ����ĵ�1��
#define PermissionAddr 				BKPSRAM_BASE+1 	//������ʱȨ�ޱ����ַ���ڵ�2��
//#define FormatDiskAddr_OneSYS 	BKPSRAM_BASE+2 	//��������һ�����ع̼���Ҫ��ʽ�����̣��ڵ�3��
#define InportEventCancelTag 		BKPSRAM_BASE+3 	//������¼��Ƿ�ȡ�������ַ���ڵ�4��
#define OutputStateAddrBase 		BKPSRAM_BASE+32 //ǰ32byte���ã������״̬�Ӵ˵�ַ��ʼ���,һ��ʹ��OutPortNum���ֽ�		 
/*************BKPSRAM END****************/


/***************CCMRAM******************/
#define CCMBaseAddr 				0x10000000 	//CCM�ڴ����ʼ��ַ
#define CCMSize						0x10000 	//CCM�ڴ��С��64KB	 

#define FlashBuffAddr   			CCMBaseAddr 
//#define FileBuffAddr    		FlashBuffAddr+FLASH_SECTOR_SIZE 
//#define MovesBuffAddr   		FileBuffAddr+FLASH_SECTOR_SIZE  	//�����ļ�����
//#define MovesBKBuffAddr 		MovesBuffAddr+sizeof(Moves)  		//���ݶ����ļ�����
#define MotionCurveBuff 			FlashBuffAddr+FLASH_SECTOR_SIZE //MovesBKBuffAddr+sizeof(Moves) 		//S���߻����ַ,�˶����ƼӼ��ٻ���,���ڴ˶��ڴ治��
/***************CCMRAM END*****************/	 

/*****************ExtRAM****************/
#define WorkParaAddr 				  ExtSRAM_ADDR 								//����������ʼ��ַ
#define DefaultParaAddr 			WorkParaAddr+sizeof(Parameter) 				//Ĭ�ϲ�����ʼ��ַ
#define LeftCoordAddr 				DefaultParaAddr+ sizeof(Parameter)  		//��ƽ̨�����б���ʼ��ַ
#define RightCoordAddr 				LeftCoordAddr+ sizeof(_coordListLeft)  		//��ƽ̨�����б���ʼ��ַ
#define CoordFileAttriAddr  	RightCoordAddr+sizeof(_coordListLeft) 		//�����ļ������б���ʼ��ַ
//#define MatrixCoordAddr			CoordFileAttriAddr+sizeof(_fileList)		//����������껺��
//#define FileOPTempAddr			MatrixCoordAddr+sizeof(_matrixCoordList)	//�ļ�����ɾ��������ʱ����
#define FileOPTempAddr      	CoordFileAttriAddr+sizeof(_fileList)
#define DicBuffAddr					  FileOPTempAddr+sizeof(_tempArray)			//�ֵ�ṹ�����ַ
#define FileBuffAddr    			DicBuffAddr+sizeof(_dic) 
#define MovesBuffAddr   			FileBuffAddr+FLASH_SECTOR_SIZE  	//�����ļ�����
#define MovesBKBuffAddr 			MovesBuffAddr+sizeof(Moves)  		//���ݶ����ļ�����  
#define LcdSendBuffAddr 	    MovesBKBuffAddr+sizeof(Moves) //MotionCurveBuff+ sizeof(AccPerList)		//��ʾ�����ͻ�����ʼ��ַ
//#define LcdRecvBuffAddr 	    LcdSendBuffAddr+sizeof(_DMASendBuff_COM3)	//��ʾ�����ջ�����ʼ��ַ,�㶯ԭ����ղ��ܷ���չ�ڴ�
//#define LcdSendCMDBuffAddr 	LcdSendBuffAddr+sizeof(_DMASendBuff_COM3)	//��ʾ�������������ʼ��ַ

/**************ExtRAM END***************/	

#ifdef __cplusplus
}
#endif

#endif