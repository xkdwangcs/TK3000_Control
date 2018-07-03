#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f4xx.h"
#include "string.h"

//V1.1 20140706
//1,修改部分函数名字,防止和其他代码的名字冲突(比如LWIP).
//2,修改内存池为32字节对齐
//V1.2 20140716
//1,修改my_mem_malloc函数,对于大内存分配错误的bug.

//#define CCMBaseAddr 0x10000000 //CCM内存的起始地址
//#define CCMSize		0x10000 //CCM内存大小：64KB

#define ExtSRAM_ADDR  		0x60000000 //扩展内存起始地址
#define ExtSRAM_SIZE		0x80000 	//扩展内存大小：16*256KbIt,512KB	


//定义二个内存池
#define SRAMEX   0		//外部内存池
#define SRAMCCM  1		//CCM内存池(此部分SRAM仅仅CPU可以访问!!!)
#define SRAMBANK 	1	//定义支持的SRAM块数.	


//mem2内存参数设定.mem2的内存池处于外部SRAM里面
#define ERAM_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define ERAM_TABLE_SIZE			ExtSRAM_SIZE/ERAM_BLOCK_SIZE 	//内存表大小
		 
//mem3内存参数设定.mem3处于CCM,用于管理CCM(特别注意,这部分SRAM,仅CPU可以访问!!)
#define CCM_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define CCM_TABLE_SIZE			CCMSize/CCM_BLOCK_SIZE 			//内存表大小
		 


//内存管理控制器
struct _m_mallco_dev
{
	void (*init)(u8);					//初始化
	u8 (*perused)(u8);		  	    	//内存使用率
	u8 	*membase[SRAMBANK];				//内存池 管理SRAMBANK个区域的内存
	u16 *memmap[SRAMBANK]; 				//内存管理状态表
	u8  memrdy[SRAMBANK]; 				//内存管理是否就绪
};

extern struct _m_mallco_dev mallco_dev;	 //在mallco.c里面定义

////////////////////////////////////////////////////////////////////////////////

//内存管理初始化  
//memx:所属内存块
void IniMalloc(u8 memx);
void myfree(u8 memx,void *ptr);  			//内存释放(外部调用)
void *mymalloc(u8 memx,u32 size);			//内存分配(外部调用)
void *myrealloc(u8 memx,void *ptr,u32 size);//重新分配内存(外部调用)
#endif













