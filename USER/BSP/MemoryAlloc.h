#ifndef __MEMORYALLOC_H
#define __MEMORYALLOC_H

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
#include "SRAM_SFMC.h"
#include "MotionProcFile.h"
#include "CoordFile.h"
#include "SCurve_Table.h"
#include "PublicFunction.h"
#include "COM_1.h"
		 
	 
/*************BKPSRAM****************/	 
//使用的备份寄存器（最大4K）
#define FirmwareUpdateTagAddr 		BKPSRAM_BASE 	//固件下载、升级标志的地址，放在备份寄存器的第1个
#define PermissionAddr 				BKPSRAM_BASE+1 	//热启动时权限保存地址，在第2个
//#define FormatDiskAddr_OneSYS 	BKPSRAM_BASE+2 	//由引导第一次下载固件后要格式化磁盘，在第3个
#define InportEventCancelTag 		BKPSRAM_BASE+3 	//输入口事件是否取消保存地址，在第4个
#define OutputStateAddrBase 		BKPSRAM_BASE+32 //前32byte备用，输出口状态从此地址开始存放,一共使用OutPortNum个字节		 
/*************BKPSRAM END****************/


/***************CCMRAM******************/
#define CCMBaseAddr 				0x10000000 	//CCM内存的起始地址
#define CCMSize						0x10000 	//CCM内存大小：64KB	 

#define FlashBuffAddr   			CCMBaseAddr 
//#define FileBuffAddr    		FlashBuffAddr+FLASH_SECTOR_SIZE 
//#define MovesBuffAddr   		FileBuffAddr+FLASH_SECTOR_SIZE  	//动作文件缓存
//#define MovesBKBuffAddr 		MovesBuffAddr+sizeof(Moves)  		//备份动作文件缓存
#define MotionCurveBuff 			FlashBuffAddr+FLASH_SECTOR_SIZE //MovesBKBuffAddr+sizeof(Moves) 		//S曲线缓存地址,运动控制加减速缓存,放在此段内存不够
/***************CCMRAM END*****************/	 

/*****************ExtRAM****************/
#define WorkParaAddr 				  ExtSRAM_ADDR 								//工作参数起始地址
#define DefaultParaAddr 			WorkParaAddr+sizeof(Parameter) 				//默认参数起始地址
#define LeftCoordAddr 				DefaultParaAddr+ sizeof(Parameter)  		//左平台坐标列表起始地址
#define RightCoordAddr 				LeftCoordAddr+ sizeof(_coordListLeft)  		//右平台坐标列表起始地址
#define CoordFileAttriAddr  	RightCoordAddr+sizeof(_coordListLeft) 		//坐标文件属性列表起始地址
//#define MatrixCoordAddr			CoordFileAttriAddr+sizeof(_fileList)		//矩阵操作坐标缓存
//#define FileOPTempAddr			MatrixCoordAddr+sizeof(_matrixCoordList)	//文件操作删除插入临时缓存
#define FileOPTempAddr      	CoordFileAttriAddr+sizeof(_fileList)
#define DicBuffAddr					  FileOPTempAddr+sizeof(_tempArray)			//字典结构缓存地址
#define FileBuffAddr    			DicBuffAddr+sizeof(_dic) 
#define MovesBuffAddr   			FileBuffAddr+FLASH_SECTOR_SIZE  	//动作文件缓存
#define MovesBKBuffAddr 			MovesBuffAddr+sizeof(Moves)  		//备份动作文件缓存  
#define LcdSendBuffAddr 	    MovesBKBuffAddr+sizeof(Moves) //MotionCurveBuff+ sizeof(AccPerList)		//显示屏发送缓存起始地址
//#define LcdRecvBuffAddr 	    LcdSendBuffAddr+sizeof(_DMASendBuff_COM3)	//显示屏接收缓存起始地址,点动原因接收不能放扩展内存
//#define LcdSendCMDBuffAddr 	LcdSendBuffAddr+sizeof(_DMASendBuff_COM3)	//显示屏发送命令缓存起始地址

/**************ExtRAM END***************/	

#ifdef __cplusplus
}
#endif

#endif