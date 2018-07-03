#ifndef SST25V_H_
#define SST25V_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "string.h"
#include "stdbool.h"

/**
 4Kbytes为一个Sector
 16个扇区为1个Block
 SST25VF032B容量为4M字节,共有64个Block(块),1024个Sector（扇区）
 */
#define FLASH_SECTOR_SIZE	                4096 //一个扇区的字节数
#define FLASH_SECTOR_COUNT	                1014 //扇区总数,原本有1024个扇区，但后面10个扇区不作为FATFS,存储不修改的参数
#define FLASH_BLOCK_SIZE	                65536//块字节数
#define FLASH_PAGES_PER_SECTOR	FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE
     
#define SST25_ReadData                      0x03
#define SST25_FastReadData                  0x0B
#define SST25_4KByte_BlockERASE             0x20
#define SST25_32KByte_BlockErase            0x52
#define SST25_64KByte_BlockErase            0xD8
#define SST25_ChipErase                     0xC7

#define SST25_ByteProgram                   0x02
#define SST25_AAI_WordProgram               0xAD

#define SST25_ReadStatusReg                 0x05
#define SST25_EnableWriteStatusReg          0x50
#define SST25_WriteStatusReg                0x01
#define SST25_WriteEnable                   0x06
#define SST25_WriteDisable                  0x04
#define SST25_ManufactDeviceID              0x90
#define SST25_JedecDeviceID                 0x9F
#define SST25_EBSY                          0x70
#define SST25_DBSY                          0x80  
#define SST25_Dummy_Byte                    0xFF  //哑字节

#define SPI_FLASH_CS_LOW()                  GPIO_ResetBits(GPIOC, GPIO_Pin_4)
#define SPI_FLASH_CS_HIGH()                 GPIO_SetBits(GPIOC, GPIO_Pin_4)  
#define SST25_CS_LOW()                      SPI_FLASH_CS_LOW()
#define SST25_CS_HIGH()                     SPI_FLASH_CS_HIGH()

void SST25V_ChipErase(void);

//初始化flash
u8 FLASH_Init(void);
// SST25VF016B 擦除存储器的全部数据
void SST25V_ChipErase(void);
//SST25VF016B 擦除该地址所在的4K存储器的数据。 sectorIndex: 扇区地址
void SST25V_SectorErase_4KByte(u32 sectorIndex);
//读取指定扇区基址+偏移的一段数据，可以是跨多扇区
void ReadBytes(u16 sectorBaseAddr,u8* data,u32 startIndex,u32 count); 
//通过指定地址，可以读取多个扇区内的数据，不覆盖原有数据，读取到缓存指定的索引
void ReadBytesOffset(u32 flashAddr,u8* readData, u32 buffAddr,u32 dataCount);
//从一块指定的扇区读取FLASH_SECTOR_SIZE长度的数据
//sectionIndex：从0开始的扇区索引;readDatas：要读取的数据内存空间,要确保内存空间是连续的
void ReadOneSector(u16 sectionIndex,u8* readDatas);
//向一块指定的扇区写入FLASH_SECTOR_SIZE长度数据
//sectionIndex：从0开始的扇区索引；writeDatas：要写入的数据块
void WriteOneSector(u16 sectionIndex,u8* writeDatas);
//向指定的扇区基址开始写入指定长度的数据，可以一次写多个扇区。
//sectionIndex：从0开始的扇区索引；writeDatas：要写入的数据块；dataCount:写入的数据长度
void WriteBytes(u16 sectionIndex,u8* writeDatas,u32 dataCount);
//Flash测试
bool FlashTest(void);

#ifdef __cplusplus
}
#endif

#endif
