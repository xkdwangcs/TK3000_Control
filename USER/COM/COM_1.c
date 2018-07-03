#include "stm32f4xx.h"
#include "COM_1.h"

u8 _DMASendBuff[_comBuffLeng];//DMA发送缓存
u8 _DMARecvBuff[_comBuffLeng];//DMA接收缓存
DMA_InitTypeDef DMA_InitStructureSend_COM1;
u16 _sendStartIndex=0;//发送起始位置地址
u16 _waitSendCount=0;//等待发送的个数
bool _isSendOK=true;//一次发送是否完毕

//中断配置，在nvic.c中已定义，此处作为规范
void USART1_NVIC_Configuration(void)
{
	/*NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

	//串口中断优先级配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//使能串口1的中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//DMA发送中断设置，串口1 TX
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);*/      

	//DMA接收完成中断设置，使能DMA1_Channel5  
	/*NVIC_InitStructure.NVIC_IRQChannel = RecvDMAIRQ1;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure); */ 
}

void USART1_Configuration(void)
{
	//第1步：打开GPIO和USART部件的时钟。使能串口1使用的GPIO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//使能串口1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	//第2步：将USART1的GPIO配置为推挽复用模式。IO口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	//串口1 TX管脚配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//第3步：将USARTR1的GPIO配置为浮空输入模式。串口1 RX管脚配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//串口参数配置
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;                                    //设置波特率为115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //设置数据位为8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //设置停止位为1位
	USART_InitStructure.USART_Parity = USART_Parity_No;                             //无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //没有硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //发送与接收
	USART_Init(COMName1, &USART_InitStructure);
}

void USART1_DMA_Configuration(void)
{         
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 							  // 开启DMA2时钟    
    
	//DMA发送通道：DMA2_通道4_数据流7
	DMA_Cmd(SendDMAStream1, DISABLE);                                                 // 关DMA通道,此句可以不用
	DMA_DeInit(SendDMAStream1);                                                       // 恢复缺省值
	DMA_InitStructureSend_COM1.DMA_Channel = SendDMAChannel1;       			          
	DMA_InitStructureSend_COM1.DMA_PeripheralBaseAddr = (u32)(&COMName1->DR);         // 设置串口发送数据寄存器
	DMA_InitStructureSend_COM1.DMA_Memory0BaseAddr = (u32)_DMASendBuff;          	  // 设置发送缓冲区首地址
	DMA_InitStructureSend_COM1.DMA_DIR = DMA_DIR_MemoryToPeripheral;                  // 设置外设位目标，内存缓冲区 -> 外设寄存器
	DMA_InitStructureSend_COM1.DMA_BufferSize = _comBuffLeng;                	  	  // 需要发送的字节数，这里其实可以设置为0，因为在实际要发送的时候，会重新设置次值
	DMA_InitStructureSend_COM1.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         // 外设地址不做增加调整，调整不调整是DMA自动实现的
	DMA_InitStructureSend_COM1.DMA_MemoryInc = DMA_MemoryInc_Enable;                  // 内存缓冲区地址增加调整
	DMA_InitStructureSend_COM1.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 外设数据宽度8位，1个字节
	DMA_InitStructureSend_COM1.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          // 内存数据宽度8位，1个字节
	DMA_InitStructureSend_COM1.DMA_Mode = DMA_Mode_Normal;							  // 循环传输模式
	DMA_InitStructureSend_COM1.DMA_Priority = DMA_Priority_VeryHigh;                  // 优先级设置
    DMA_InitStructureSend_COM1.DMA_FIFOMode = DMA_FIFOMode_Disable;                   // 指定如果FIFO模式或直接模式将用于指定的流 ： 不使能FIFO模式    
    DMA_InitStructureSend_COM1.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;        // 指定了FIFO阈值水平    
    DMA_InitStructureSend_COM1.DMA_MemoryBurst = DMA_MemoryBurst_Single;              // 指定的Burst转移配置内存传输     
    DMA_InitStructureSend_COM1.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;      // 指定的Burst转移配置外围转移        
	DMA_Init(SendDMAStream1, &DMA_InitStructureSend_COM1);               		      // 写入配置
    //DMA_ClearFlag(SendDMAFlag1);                                 				      // 清除DMA所有标志,此句可以不用
	DMA_ITConfig(SendDMAStream1, DMA_IT_TC, ENABLE);         					      // 开启发送DMA通道中断
    
    //DMA接收通道：DMA2_通道4_数据流2 
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel = RecvDMAChannel1;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&COMName1->DR);		// 设置串口接收数据寄存器
	DMA_InitStructure.DMA_Memory0BaseAddr =  (u32)_DMARecvBuff;        		// 设置接收缓冲区首地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                 // 设置外设为数据源，外设寄存器 -> 内存缓冲区
	DMA_InitStructure.DMA_BufferSize = _comBuffLeng;                  		// 需要最大可能接收到的字节数
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不做增加调整，调整不调整是DMA自动实现的
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存缓冲区地址增加调整
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度8位，1个字节
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度8位，1个字节
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                         // 循环传输模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // 优先级设置  
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // 指定如果FIFO模式或直接模式将用于指定的流 ： 不使能FIFO模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;       // 指定了FIFO阈值水平 
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             // 指定的Burst转移配置内存传输   
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     // 指定的Burst转移配置外围转移  
	DMA_Init(RecvDMAStream1, &DMA_InitStructure);               		    // 写入配置
    //DMA_ClearFlag(RecvDMAFlag1);                                 		    // 清除DMA所有标志
	DMA_Cmd(RecvDMAStream1, ENABLE);                            		    // 开启接收DMA通道，等待接收数据		
}

void InitUSART_COM1(void)
{
	USART1_Configuration();
	USART1_NVIC_Configuration();
	USART1_DMA_Configuration();

	//一种模式：DMA发送，串口中断接收
	//	USART_DMACmd(COMName1,USART_DMAReq_Tx,ENABLE); 		//开启串口DMA发送
	//	USART_ITConfig(COMName1, USART_IT_RXNE, ENABLE); 	//使用中断方式接收。使能串口接收中断
	//	USART_Cmd(COMName1, ENABLE);											//开启串口

	//二种模式：DMA发送，DMA接收
	//DMA_ITConfig(RecvDMAStream1, DMA_IT_TC, ENABLE); //DMA传输完成中断
	USART_ITConfig(COMName1, USART_IT_IDLE, ENABLE);  //开启串口1空闲IDEL 中断
	USART_DMACmd(COMName1, USART_DMAReq_Tx, ENABLE);  //开启串口DMA发送
	USART_DMACmd(COMName1, USART_DMAReq_Rx, ENABLE);  //开启串口DMA接收
	USART_Cmd(COMName1, ENABLE);											//开启串口
}

void StartSendToDMA_COM1(void)
{
	_isSendOK=false;
	DMA_InitStructureSend_COM1.DMA_Memory0BaseAddr = (u32)&_DMASendBuff[_sendStartIndex];  // 设置发送缓冲区首地址
	DMA_Init(SendDMAStream1, &DMA_InitStructureSend_COM1);             	// 写入配置
	DMA_SetCurrDataCounter(SendDMAStream1,_waitSendCount); 						//设置要发送的字节数	
	_sendStartIndex=_sendStartIndex+_waitSendCount;
	_waitSendCount=0;		
	DMA_Cmd(SendDMAStream1, ENABLE); //开始DMA发送
}      

void SendBytes_COM1(u8* datas, u16 count)
{
	if((_sendStartIndex+_waitSendCount+count)> _comBuffLeng-1)
	{	
		while(!_isSendOK)//等待队列中一所有发送完毕
		{   
		}
		_sendStartIndex=0;
	}
	u16 currAddrBase=_sendStartIndex+_waitSendCount;//当前要存数的基址
	for(u16 i=0;i<count;i++)
	{
		_DMASendBuff[currAddrBase+i]=datas[i];
	}	
	_waitSendCount=_waitSendCount+count;
	if(_isSendOK)
	{
		StartSendToDMA_COM1();
	}
}
