#include "SRAM_SFMC.h"

//初始化外部扩展存储器
void InitExtSRAM(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能GPIO时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
	             RCC_AHB1Periph_GPIOG, ENABLE);
	/* 使能 FSMC 时钟 */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
	
	/* GPIOD configuration */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);//D2
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);//D3
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);//OE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);//WE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);//CE，片选
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);//D13
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);//D14
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);//D15
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);//A16
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);//A17
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);//A18,未用
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);//D0
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);//D1

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_4  | GPIO_Pin_5  |
	                    GPIO_Pin_7  |GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
	                    GPIO_Pin_12 | /*GPIO_Pin_13 |*/ GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    	//复用输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100	//100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	/* GPIOE configuration */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);//LB
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);//UB
	//GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);//A19,未用
	//GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);//安富莱用于译码器
	//GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);//安富莱用于译码器
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);//D4
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);//D5
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);//D6
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);//D7
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);//D8
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);//D9
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);//D10
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);//D11
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);//D12

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  /*| GPIO_Pin_3 |
	                    GPIO_Pin_4  | GPIO_Pin_5  */| GPIO_Pin_7 |
	                    GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
	                    GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* GPIOF configuration */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);//A0
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);//A1
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);//A2
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);//A3
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);//A4
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);//A5
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);//A6
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);//A7
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);//A8
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);//A9

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3  |
	                    GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_12 | GPIO_Pin_13 |
	                    GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* GPIOG configuration */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FSMC);//A10
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);//A11
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);//A12
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);//A13
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);//A14
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);//A15
	//GPIO_PinAFConfig(GPIOG, GPIO_PinSource10 , GPIO_AF_FSMC);//安富莱用于译码器,片选主信号

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
	                    GPIO_Pin_4  | GPIO_Pin_5  /*|GPIO_Pin_10*/;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/*-- FSMC Configuration ------------------------------------------------------*/
	p.FSMC_AddressSetupTime = 2;//40; //0x00 //地址建立时间（ADDSET）为1个HCLK 1/36M=27ns ,设为1出错，2可以
	p.FSMC_AddressHoldTime = 0; 		//地址保持时间（ADDHLD）模式A未用到	
	p.FSMC_DataSetupTime = 1;//40;	//0x08	//数据保持时间（DATAST）为9个HCLK 6*9=54ns	 ，设为1也可以
	p.FSMC_BusTurnAroundDuration = 0;
	p.FSMC_CLKDivision = 0;
	p.FSMC_DataLatency = 0;
	p.FSMC_AccessMode = FSMC_AccessMode_A; //模式A 

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;  
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;	// FSMC_MemoryType_PSRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;//FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
	/*!< Enable FSMC Bank1_SRAM1 Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

//内存读写测试
bool ExtSRAMTest(void)
{
	u8 value=0;
	for(u32 i=0;i<ExtSRAM_SIZE;i++)
	{
		*(__IO uint8_t *) (ExtSRAM_ADDR+i)=value;
		if(value==0xFF)
		{
			value=0;
			continue;
		}
		value++;
	}
	
	value=0;
	for(u32 i=0;i<ExtSRAM_SIZE;i++)
	{
		u8 tv=*(__IO uint8_t *) (ExtSRAM_ADDR+i);
		if(value!=tv)
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

//#include "Timekeeper.h"
//typedef struct
//{
//	char Chars[32];                        
//	u32 data1;
//	float data2;
//}TestStruct;

//TestStruct TS __at(ExtSRAM_ADDR);
//char ffff[32] __at(ExtSRAM_ADDR+0x1000);
//u8 _speedTestBuff[ExtSRAM_SIZE] __at(ExtSRAM_ADDR);

/***********测试结果 XYQ 2016.09.21***************
写速度
缓存大小：整个扩展大小（512kB）
FSMC_DataSetupTime=2，为40ms  =>12.5MB/s
FSMC_DataSetupTime=4，为40ms  =>12.5MB/s
FSMC_DataSetupTime=20，为85ms =>5.88MB/s
***************************************************/

//测试扩展内存
void TestExtSRAM(void)
{
	/*memset(ffff, 0, sizeof(ffff));
	for(u8 i=0;i<32;i++)
	{
		ffff[i]=i;
	}
	
	memset(ffff, 0, sizeof(ffff));
	strcpy(ffff, "ABCDEFGDDDDRT");
	memset(&TS, 0, sizeof(TS));
	strcpy(TS.Chars, "自动锁螺丝机器人控制系统");
	TS.data1=125;
	TS.data2=22.8;*/
	
	/*StartTimekeeper();
	u32 startTime=GetTimekeeperValue();
	u8 value=0;
	for(u32 i=0;i<ExtSRAM_SIZE;i++)
	{
		_speedTestBuff[i]=value++;
		if(value==0xFF)
			value=0;
	}
	u32 timeLeng=GetTimekeeperValue()-startTime;
	StopTimekeeper();*/
}

