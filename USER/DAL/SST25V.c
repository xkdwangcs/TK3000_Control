//******************************************************************************
//*File name:           bsp_SST25VF016.c                                       *
//*Descriptions:        SPI下的SST25VF016B操作函数库                           *
//*Created date:        2011-03-29                                             *
//*Modified date:       2011-03-29                                             *
//*Version:             1.0                                                    *
//*note:                SST25VFXX 写操作必须先使能写状态，同时被写的数据位置需要
//*                     在擦除状态，而擦除是4K,32K,64K方式擦除，所以写数据需将要
//*                     该地址所在的4K数据读出，存放到一个数组，修改数组对应的数
//*                     据，然后擦除存储器内的该4K数据，将数组重新写入存储器
//******************************************************************************

#include "SST25V.h"
#include "PublicFunction.h"
#include "MemoryAlloc.h"

//Flash测试扇区
u16 _flashTestSection=FLASH_SECTOR_COUNT+1;

void SPI_FLASH_Init(void);
void SST25V_EnableWriteStatusRegister(void);
void SST25V_WriteStatusRegister(u8 Byte);
void SST25V_WaitForWriteEnd(void);
void SST25V_DBSY(void);
void SST25V_ByteWrite(unsigned char *Byte, u32 WriteAddr);
void SST25V_BufferWrite(char* pBuffer, u32 ReadAddr, u16 NumByteToRead);
void SSTF016B_WriteOneByte(unsigned long W_address, unsigned char byte);

/******************************************************************************
** 函数名称: SST25V_Init(void)
** 函数功能: SST25 初始化
** 入口参数: 无
** 出口参数: 无
** 备    注: 无
*******************************************************************************/
u8 FLASH_Init(void)
{
	SPI_FLASH_Init();
	SST25_CS_HIGH();
	SST25V_EnableWriteStatusRegister();
	SST25V_WriteStatusRegister(0x02);
	SST25V_DBSY();
    return 0;
}
/******************************************************************************
** 函数名称: SPI_FLASH_Init(void)
** 函数功能: 初始化SPI端口
** 入口参数: 无
** 出口参数: 无
** 备    注: 无
*******************************************************************************/
void SPI_FLASH_Init(void)
{
	//SPI相关端口配置
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE); //使能GPIOA\GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); //使能SPI1时钟     
	// SCLK  MISO  MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);   
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); //复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); //复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); //复用为 SPI1   

	//WP写保护可写，2017.03.17
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	
    //CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;       
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
    SPI_FLASH_CS_HIGH();
    
    //SPI参数设置
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                //设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		            //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		                    //串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                    //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                    //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;  //定义波特率预分频的值:波特率预分频值为4较快(要注意会不会写出错),256分频写入太慢了
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                        //CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);                                 //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器 
	SPI_Cmd(SPI1, ENABLE);                                              //使能SPI外设   
}

/******************************************************************************
** 函数名称: SPI_FLASH_SendByte(u8 byte)
** 函数功能: 发送1个字节数据，返回接收到的数据
** 入口参数: byte
** 出口参数: byte
** 备    注: 无
*******************************************************************************/
static u8 SPI_FLASH_SendByte(u8 byte)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		;
	SPI_I2S_SendData(SPI1, byte);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		;
	return SPI_I2S_ReceiveData(SPI1);
}
/******************************************************************************
** 函数名称: SPI_FLASH_ReadByte(void)
** 函数功能: 接收SPI数据
** 入口参数: 无
** 出口参数: byte 返回接收的数据
** 备    注: 无
*******************************************************************************/
static u8 SPI_FLASH_ReadByte(void)
{
	return (SPI_FLASH_SendByte(SST25_Dummy_Byte));
}
/************************************************************************
** 函数名称: SST25V_ReadStatusRegister(void)
** 函数功能: SST25VF016B 读取状态寄存器
** 入口参数: 无
** 出口参数: 状态寄存器数据
** 注	 意:
************************************************************************/
u8 SST25V_ReadStatusRegister(void)
{
	u8 StatusRegister = 0;
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_ReadStatusReg);
	StatusRegister = SPI_FLASH_ReadByte();
	SST25_CS_HIGH();
	return StatusRegister;
}
/************************************************************************
** 函数名称: SST25V_WriteEnable(void)
** 函数功能: SST25VF016B 写使能
** 入口参数: 无
** 出口参数: 无
** 注	 意:
************************************************************************/
void SST25V_WriteEnable(void)
{
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_WriteEnable);
	SST25_CS_HIGH();
}
/************************************************************************
** 函数名称: SST25V_WriteDisable(void)
** 函数功能: SST25VF016B 禁止写使能
** 入口参数: 无
** 出口参数: 无
** 注	 意:
************************************************************************/
void SST25V_WriteDisable(void)
{
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_WriteDisable);
	SST25_CS_HIGH();
}
/************************************************************************
** 函数名称: SST25V_EnableWriteStatusRegister(void)
** 函数功能: SST25VF016B 使能写状态寄存器
** 入口参数: 无
** 出口参数: 无
** 注	 意:
************************************************************************/
void SST25V_EnableWriteStatusRegister(void)
{
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_EnableWriteStatusReg);
	SST25_CS_HIGH();
}
/************************************************************************
** 函数名称: SST25V_WriteStatusRegister(u8 Byte)
** 函数功能: SST25VF016B 写状态寄存器指令
** 入口参数: u8 Byte :指令
** 出口参数: 无
** 注	 意:
************************************************************************/
void SST25V_WriteStatusRegister(u8 Byte)
{
	SST25V_EnableWriteStatusRegister();
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_WriteStatusReg);
	SPI_FLASH_SendByte(Byte);
	SST25_CS_HIGH();
}
/************************************************************************
** 函数名称: SST25V_WaitForWriteEnd(void)
** 函数功能: SST25VF016B 等待数据写完
** 入口参数: 无
** 出口参数: 无
** 注	 意:
************************************************************************/
void SST25V_WaitForWriteEnd(void)
{
	u8 FLASH_Status = 0;
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_ReadStatusReg);
	do
	{
		FLASH_Status = SPI_FLASH_SendByte(SST25_Dummy_Byte);

	}
	while ((FLASH_Status & SST25_WriteStatusReg) == SET);

	SST25_CS_HIGH();
}
/************************************************************************
** 函数名称: SST25V_ByteRead
** 函数功能: SST25VF016B的读取1个字节数据(低速)
** 入口参数:
**			 u32 ReadAddr：目标地址,
范围 0x0 - SST25_MAX_ADDR（SST25_MAX_ADDR = 0x1FFFFF）
** 出口参数: 返回读取的数据
** 备    注:
************************************************************************/
u8 SST25V_ByteRead(u32 ReadAddr)
{
	u8 Temp = 0;
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_ReadData);
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
	Temp = SPI_FLASH_ReadByte();
	SST25_CS_HIGH();
	return Temp;
}
/************************************************************************
** 函数名称: SST25V_BufferRead
** 函数功能: SST25VF016B的读取N个字节数据(低速)
** 入口参数:
**			 u32 ReadAddr：目标地址,
范围 0x0 - SST25_MAX_ADDR（SST25_MAX_ADDR = 0x1FFFFF）
**           pBuffer:  存放数据的数组
**           NumByteToRead : 数据个数
** 出口参数: 返回读取的数据
** 备    注:
************************************************************************/
void SST25V_BufferRead(char* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	SST25_CS_LOW();
	//	char * temp_abc = (char *)pBuffer;
	SPI_FLASH_SendByte(SST25_ReadData);
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
	while (NumByteToRead--)
	{
		*pBuffer = SPI_FLASH_ReadByte();
		pBuffer++;
	}
	SST25_CS_HIGH();
}
/************************************************************************
** 函数名称: SST25V_HighSpeedRead
** 函数功能: SST25VF016B的读函数，高速读取1字节
** 入口参数:
**			 u32 ReadAddr：目标地址,
**			 u8* pBuffer:接收缓存的指针
** 出口参数:
** 备    注:
************************************************************************/
u8 SST25V_HighSpeedRead(u32 ReadAddr)
{
	u32 Temp = 0;
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_FastReadData);
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
	SPI_FLASH_SendByte(SST25_Dummy_Byte);
	Temp = SPI_FLASH_ReadByte();
	SST25_CS_HIGH();
	return Temp;
}
/************************************************************************
** 函数名称: SST25V_HighSpeedBufferRead
** 函数功能: SST25VF016B的读函数，高速读取N字节
** 入口参数:
**			 u32 ReadAddr：目标地址,
范围 0x0 - SST25_MAX_ADDR（SST25_MAX_ADDR = 0x1FFFFF）
**      	 u16 NumByteToRead:	要读取的数据字节数
**			 u8* pBuffer:接收缓存的指针
** 出口参数:
** 备    注:
************************************************************************/
void SST25V_HighSpeedBufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_FastReadData);
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
	SPI_FLASH_SendByte(SST25_Dummy_Byte);

	while (NumByteToRead--)
	{
		*pBuffer = SPI_FLASH_ReadByte();
		pBuffer++;
	}
	SST25_CS_HIGH();
}
/************************************************************************
** 函数名称: SST25V_Wait_Busy_AAI(void)
** 函数功能: SST25VF016B 查询AAI忙？
** 入口参数: 无
** 出口参数: 无
** 注	 意:
************************************************************************/
void SST25V_Wait_Busy_AAI(void)
{
	while (SST25V_ReadStatusRegister() == 0x43) /* ???? */
		SST25V_ReadStatusRegister();
}
/************************************************************************
** 函数名称: SST25V_ByteWrite
** 函数功能: SST25VF016B的写函数，可写1个数据到指定地址
** 入口参数:
**			 u32 WriteAddr：目标地址,范围 0x0 - SST25_MAX_ADDR（SST25_MAX_ADDR = 0x1FFFFF）
**			 u8 Byte:发送数据
** 出口参数:
** 注	 意:
************************************************************************/
void SST25V_ByteWrite(unsigned char *Byte, u32 WriteAddr)
{
	SST25V_WriteEnable();
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_ByteProgram);
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
	SPI_FLASH_SendByte(WriteAddr & 0xFF);

	SPI_FLASH_SendByte(*Byte);
	SST25_CS_HIGH();
	SST25V_WaitForWriteEnd();
}
/************************************************************************
** 函数名称: SST25V_ByteWrite
** 函数功能: SST25VF016B的写函数，可写2个数据到指定地址 (AAI写) 带地址的写
**           即 第一次需写地址，以后就是2个2个数据的持续写
** 入口参数:
**			 u32 WriteAddr：目标地址,范围 0x0 - SST25_MAX_ADDR（SST25_MAX_ADDR = 0x1FFFFF）
**			 u8 Byte1, u8 Byte2:发送的2个数据
** 出口参数:
** 注	 意:
************************************************************************/
void AutoAddressIncrement_WordProgramA(u8 Byte1, u8 Byte2, u32 Addr)
{
	SST25V_WriteEnable();
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_AAI_WordProgram);
	SPI_FLASH_SendByte((Addr & 0xFF0000) >> 16);
	SPI_FLASH_SendByte((Addr & 0xFF00) >> 8);
	SPI_FLASH_SendByte(Addr & 0xFF);

	SPI_FLASH_SendByte(Byte1);
	SPI_FLASH_SendByte(Byte2);

	SST25_CS_HIGH();
	SST25V_Wait_Busy_AAI();
	SST25V_WaitForWriteEnd();
}
/************************************************************************
** 函数名称: SST25V_ByteWrite
** 函数功能: SST25VF016B的写函数，可写2个数据到指定地址 (AAI写) 不带地址的写
**           即 第一次需写地址，以后就是2个2个数据的持续写
** 入口参数:
**			 u32 WriteAddr：目标地址,范围 0x0 - SST25_MAX_ADDR（SST25_MAX_ADDR = 0x1FFFFF）
**			 u8 Byte1, u8 Byte2:发送的2个数据
**           u8 state :模式选择，AAI发送数据的最后2个数据之后需要发送一次WRDI
** 出口参数:
** 注	 意:
************************************************************************/
void AutoAddressIncrement_WordProgramB(u8 state, u8 Byte1, u8 Byte2)
{
	SST25V_WriteEnable();
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_AAI_WordProgram);

	SPI_FLASH_SendByte(Byte1);
	SPI_FLASH_SendByte(Byte2);

	SST25_CS_HIGH();
	SST25V_Wait_Busy_AAI();

	if (state == 1)
	{
		SST25V_WriteDisable();
	}
	SST25V_Wait_Busy_AAI();
}
/************************************************************************
** 函数名称: SST25V_SectorErase_4KByte
** 函数功能: SST25VF016B 擦除该地址所在的4K存储器的数据
** 入口参数: sectorIndex : 扇区地址
** 出口参数: 无
** 注	 意:
************************************************************************/
void SST25V_SectorErase_4KByte(u32 sectorIndex)
{
	SST25V_WriteEnable();
	SST25_CS_LOW();
    u32 Addr=sectorIndex*FLASH_SECTOR_SIZE;
	SPI_FLASH_SendByte(SST25_4KByte_BlockERASE);
	SPI_FLASH_SendByte((Addr & 0xFF0000) >> 16);
	SPI_FLASH_SendByte((Addr & 0xFF00) >> 8);
	SPI_FLASH_SendByte(Addr & 0xFF);

	SST25_CS_HIGH();
	SST25V_WaitForWriteEnd();
}

/************************************************************************
** 函数名称: SST25V_ChipErase(void)
** 函数功能: SST25VF016B 擦除存储器的全部数据
** 入口参数: 无
** 出口参数: 无
** 注	 意:
************************************************************************/
void SST25V_ChipErase(void)
{
	SST25V_WriteEnable();
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_ChipErase);
	SST25_CS_HIGH();
	SST25V_WaitForWriteEnd();
}
/************************************************************************
** 函数名称: SST25V_ReadJedecID(void)
** 函数功能: SST25VF016B 读取制造商和器件信息
** 入口参数: 无
** 出口参数: 信息数据
** 注	 意:
************************************************************************/
u32 SST25V_ReadJedecID(void)
{
	u32 JEDECID = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_JedecDeviceID);
	Temp0 = SPI_FLASH_ReadByte();
	Temp1 = SPI_FLASH_ReadByte();
	Temp2 = SPI_FLASH_ReadByte();
	SST25_CS_HIGH();
	JEDECID = (Temp0 << 16) | (Temp1 << 8) | Temp2;
	return JEDECID;
}
/************************************************************************
** 函数名称: SST25V_ReadManuID_DeviceID(u32 ReadManu_DeviceID_Addr)
** 函数功能: SST25VF016B 读取器件信息
** 入口参数: u32 ReadManu_DeviceID_Addr 器件地址
** 出口参数: u16  器件ID
** 注	 意:
************************************************************************/
u16 SST25V_ReadManuID_DeviceID(u32 ReadManu_DeviceID_Addr)
{
	u16 ManuID_DeviceID = 0;
	u8 ManufacturerID = 0, DeviceID = 0;
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_ManufactDeviceID);

	SPI_FLASH_SendByte((ReadManu_DeviceID_Addr & 0xFF0000) >> 16);
	SPI_FLASH_SendByte((ReadManu_DeviceID_Addr & 0xFF00) >> 8);
	SPI_FLASH_SendByte(ReadManu_DeviceID_Addr & 0xFF);

	if (ReadManu_DeviceID_Addr == 1)
	{
		DeviceID = SPI_FLASH_ReadByte();
		ManufacturerID = SPI_FLASH_ReadByte();
	}
	else
	{
		ManufacturerID = SPI_FLASH_ReadByte();
		DeviceID = SPI_FLASH_ReadByte();
	}

	ManuID_DeviceID = ((ManufacturerID << 8) | DeviceID);
	SST25_CS_HIGH();

	return ManuID_DeviceID;
}
/************************************************************************
** 函数名称: SST25V_EBSY()
** 函数功能: SST25VF016B 发送EBSY
** 入口参数: 无
** 出口参数: 无
** 注	 意:
************************************************************************/
void SST25V_EBSY(void)
{
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_EBSY);
	SST25_CS_HIGH();
}
/************************************************************************
** 函数名称: SST25V_EBSY()
** 函数功能: SST25VF016B 发送DBSY
** 入口参数: 无
** 出口参数: 无
** 注	 意:
************************************************************************/
void SST25V_DBSY(void)
{
	SST25_CS_LOW();
	SPI_FLASH_SendByte(SST25_DBSY);
	SST25_CS_HIGH();
}

//读取指定扇区基址+偏移的一段数据，可以是跨多扇区
//(CoordFileAttri file,u8* data,u32 startIndex,u32 count)
void ReadBytes(u16 sectorBaseAddr,u8* data,u32 startIndex,u32 count)
{
	if(count<=0)
		return;
	u32 readAddr=sectorBaseAddr*FLASH_SECTOR_SIZE+startIndex;
	memset(data,0,count);
	SST25V_HighSpeedBufferRead(data, readAddr, count);
	//SST25V_BufferRead((char*)data,readAddr,count);
}

//从一块指定的扇区读取FLASH_SECTOR_SIZE长度的数据,主要用于FATFS
//sectionIndex：从0开始的扇区索引;readDatas：要读取的数据内存空间,要确保内存空间是连续的
void ReadOneSector(u16 sectionIndex,u8* readDatas)
{
    ReadBytes(sectionIndex,readDatas,0, FLASH_SECTOR_SIZE);
}

//通过指定地址，可以读取多个扇区内的数据，不覆盖原有数据，读取到缓存指定的索引
void ReadBytesOffset(u32 flashAddr,u8* readData, u32 buffAddr,u32 dataCount)
{
    SST25V_HighSpeedBufferRead(&readData[buffAddr], flashAddr, dataCount);
}

//向一块指定的扇区写入FLASH_SECTOR_SIZE长度数据
//sectionIndex：从0开始的扇区索引；writeDatas：要写入的数据块
void WriteOneSector(u16 sectionIndex,u8* writeDatas)
{
     //WriteBytes(sectionIndex,writeDatas, FLASH_SECTOR_SIZE);
	SST25V_SectorErase_4KByte(sectionIndex);     //擦除指定扇区
    u32 writeAddr=sectionIndex*FLASH_SECTOR_SIZE;
    for (u32 j = 0; j < FLASH_SECTOR_SIZE; j++)
	{
		SST25V_ByteWrite(&writeDatas[j], writeAddr++);
	}
}

u8 _flashBuff[FLASH_SECTOR_SIZE] __at(FlashBuffAddr);
//向指定的扇区基址开始写入指定长度的数据，可以一次写多个扇区。
//sectionIndex：从0开始的扇区索引；writeDatas：要写入的数据块；dataCount:写入的数据长度
void WriteBytes(u16 sectionIndex,u8* writeDatas,u32 dataCount)
{
	if(dataCount<=0)
		return;   
	u16 sectorNum=dataCount/FLASH_SECTOR_SIZE;
	for(u8 i=0;i<sectorNum;i++)
	{
		WriteOneSector(sectionIndex+i,writeDatas);
		writeDatas+=FLASH_SECTOR_SIZE;
	}
	sectionIndex+=sectorNum;
  	sectorNum=dataCount%FLASH_SECTOR_SIZE;
	if(sectorNum>0)
	{
		ReadOneSector(sectionIndex,_flashBuff);
		CopyBytes(_flashBuff,0,writeDatas,sectorNum);
		WriteOneSector(sectionIndex,_flashBuff);
	}		
}

//Flash测试
bool FlashTest(void)
{
	memset(_flashBuff,0,FLASH_SECTOR_SIZE);
	u8 value=0;
	for(u32 i=0;i<FLASH_SECTOR_SIZE;i++)
	{
		_flashBuff[i]=value;
		if(value==0xFF)
		{
			value=0;
			continue;
		}
		value++;
	}
	WriteOneSector(_flashTestSection,_flashBuff);
	memset(_flashBuff,0,FLASH_SECTOR_SIZE);
	ReadOneSector(_flashTestSection,_flashBuff);
	value=0;
	for(u32 i=0;i<FLASH_SECTOR_SIZE;i++)
	{		
		if(value!=_flashBuff[i])
			return false;
		if(value==0xFF)
		{
			value=0;
			continue;
		}
		value++;
	}
	return true;
}
