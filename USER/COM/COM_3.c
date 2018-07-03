#include "COM_3.h"
#include "MemoryAlloc.h"
#include "AppTasks.h"


u8 _DMASendBuff_COM3[_com3BuffLeng];// __at(LcdSendBuffAddr);//DMA发送缓存
u8 _DMARecvBuff_COM3[_com3BuffLeng];// __at(LcdRecvBuffAddr);//DMA接收缓存，接收不能放扩展内存，否则点动有时不停，内存速度问题

DMA_InitTypeDef DMA_InitStructureSend_COM3;
u16 _sendStartIndex_COM3=0;//发送起始位置地址
u16 _waitSendCount_COM3=0;//等待发送的个数
bool _isSendOK_COM3=true;//一次发送是否完毕
u16 _recvStartIndex_COM3=0;//接收的起始位置


//中断配置，在nvic.c中已定义，此处作为规范
void USART3_NVIC_Configuration(void)
{    
		/*NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

		//串口中断优先级配置
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	//使能串口3的中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		//DMA发送完成中断设置，使能DMA1_Channel2，发送优先级要高于接收中断否则触摸会异常
		NVIC_InitStructure.NVIC_IRQChannel = SendDMAIRQ3;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
		NVIC_Init(&NVIC_InitStructure);  */

		//DMA接收完成中断设置，使能DMA1_Channel3  
//		NVIC_InitStructure.NVIC_IRQChannel = RecvDMAIRQ3;  
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
//		NVIC_Init(&NVIC_InitStructure);  
}

void USART3_Configuration(void)
{
		//第1步：打开GPIO和USART部件的时钟。使能串口3使用的GPIO时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		//使能串口3时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
		//第2步：将USART3的GPIO配置为推挽复用模式。IO口设置
		GPIO_InitTypeDef GPIO_InitStructure;
		//串口3 TX管脚配置
		//USART3的TX和RX引脚默认的引出脚位是PB10和PB11，根据配置位的设置，可以重映射到PC10和PC11，还可以重映射到PD8和PD9。
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  		GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		//第3步：将USARTR3的GPIO配置为浮空输入模式。串口3 RX管脚配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		//串口参数配置
		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate = 115200; /*设置波特率为115200*/
		USART_InitStructure.USART_WordLength = USART_WordLength_8b; /*设置数据位为8位*/
		USART_InitStructure.USART_StopBits = USART_StopBits_1; /*设置停止位为1位*/
		USART_InitStructure.USART_Parity = USART_Parity_No; /*无奇偶校验*/
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; /*没有硬件流控*/
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; /*发送与接收*/
		USART_Init(COMName3, &USART_InitStructure);
}

void USART3_DMA_Configuration(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 							  // 开启DMA1时钟    
    
	//DMA发送通道：DMA2_通道4_数据流7
	DMA_Cmd(SendDMAStream3, DISABLE);                                                 // 关DMA通道,此句可以不用
	DMA_DeInit(SendDMAStream3);                                                       // 恢复缺省值
	DMA_InitStructureSend_COM3.DMA_Channel = SendDMAChannel3;       			          
	DMA_InitStructureSend_COM3.DMA_PeripheralBaseAddr = (u32)(&COMName3->DR);         // 设置串口发送数据寄存器
	DMA_InitStructureSend_COM3.DMA_Memory0BaseAddr = (u32)_DMASendBuff_COM3;          // 设置发送缓冲区首地址
	DMA_InitStructureSend_COM3.DMA_DIR = DMA_DIR_MemoryToPeripheral;                  // 设置外设位目标，内存缓冲区 -> 外设寄存器
	DMA_InitStructureSend_COM3.DMA_BufferSize = _com3BuffLeng;                	  // 需要发送的字节数，这里其实可以设置为0，因为在实际要发送的时候，会重新设置次值
	DMA_InitStructureSend_COM3.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         // 外设地址不做增加调整，调整不调整是DMA自动实现的
	DMA_InitStructureSend_COM3.DMA_MemoryInc = DMA_MemoryInc_Enable;                  // 内存缓冲区地址增加调整
	DMA_InitStructureSend_COM3.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 外设数据宽度8位，1个字节
	DMA_InitStructureSend_COM3.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          // 内存数据宽度8位，1个字节
	DMA_InitStructureSend_COM3.DMA_Mode = DMA_Mode_Normal;							  // 循环传输模式
	DMA_InitStructureSend_COM3.DMA_Priority = DMA_Priority_VeryHigh;                  // 优先级设置
    DMA_InitStructureSend_COM3.DMA_FIFOMode = DMA_FIFOMode_Disable;                   // 指定如果FIFO模式或直接模式将用于指定的流 ： 不使能FIFO模式    
    DMA_InitStructureSend_COM3.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;        // 指定了FIFO阈值水平    
    DMA_InitStructureSend_COM3.DMA_MemoryBurst = DMA_MemoryBurst_Single;              // 指定的Burst转移配置内存传输     
    DMA_InitStructureSend_COM3.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;      // 指定的Burst转移配置外围转移        
	DMA_Init(SendDMAStream3, &DMA_InitStructureSend_COM3);               		      // 写入配置
    //DMA_ClearFlag(SendDMAFlag3);                                 				      // 清除DMA所有标志,此句可以不用
	DMA_ITConfig(SendDMAStream3, DMA_IT_TC, ENABLE);         					      // 开启发送DMA通道中断
    
    //DMA接收通道：DMA2_通道4_数据流2 
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel = RecvDMAChannel3;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&COMName3->DR);		// 设置串口接收数据寄存器
	DMA_InitStructure.DMA_Memory0BaseAddr =  (u32)_DMARecvBuff_COM3;        // 设置接收缓冲区首地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                 // 设置外设为数据源，外设寄存器 -> 内存缓冲区
	DMA_InitStructure.DMA_BufferSize = _com3BuffLeng;                  // 需要最大可能接收到的字节数
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
	DMA_Init(RecvDMAStream3, &DMA_InitStructure);               		    // 写入配置
  //DMA_ClearFlag(RecvDMAFlag3);                                 		    // 清除DMA所有标志
	DMA_Cmd(RecvDMAStream3, ENABLE);                            		    // 开启接收DMA通道，等待接收数据	
}

void InitUSART_COM3(void)
{
		USART3_Configuration();
		USART3_NVIC_Configuration();
		USART3_DMA_Configuration();

	//  一种模式：DMA发送，串口中断接收
    //	USART_DMACmd(COMName3,USART_DMAReq_Tx,ENABLE); 		//开启串口DMA发送
    //	USART_ITConfig(COMName3, USART_IT_RXNE, ENABLE); 	//使用中断方式接收。使能串口接收中断
    //	USART_Cmd(COMName3, ENABLE);											//开启串口

	//  二种模式：DMA发送，DMA接收
    //  DMA_ITConfig(RecvDMAStream3, DMA_IT_TC, ENABLE); //DMA传输完成中断
		USART_ITConfig(COMName3, USART_IT_IDLE, ENABLE);  //开启串口3空闲IDEL 中断
		USART_DMACmd(COMName3, USART_DMAReq_Tx, ENABLE);  //开启串口DMA发送
		USART_DMACmd(COMName3, USART_DMAReq_Rx, ENABLE);  //开启串口DMA接收
		USART_Cmd(COMName3, ENABLE);											//开启串口
}

void StartSendToDMA_COM3(void)
{
	//_isSendOK_COM3=false;
	DMA_InitStructureSend_COM3.DMA_Memory0BaseAddr = (u32)&_DMASendBuff_COM3[_sendStartIndex_COM3];  // 设置发送缓冲区首地址
	DMA_Init(SendDMAStream3, &DMA_InitStructureSend_COM3);             	// 写入配置
	DMA_SetCurrDataCounter(SendDMAStream3,_waitSendCount_COM3); 						//设置要发送的字节数	
	_sendStartIndex_COM3=_sendStartIndex_COM3+_waitSendCount_COM3;
    _waitSendCount_COM3=0;		
	DMA_Cmd(SendDMAStream3, ENABLE); //开始DMA发送
	_isSendOK_COM3=false;
}

void SendBytes_COM3(u8* datas, u16 count)
{
     if(count>=_com3BuffLeng)
         count=count;
     if((_sendStartIndex_COM3+_waitSendCount_COM3+count)> _com3BuffLeng-1)
	 {	
		u32 i=0;
		while(!_isSendOK_COM3)//等待队列中一所有发送完毕
		{
			//DO:按几次重打按键（不是屏幕上的按钮），就会在此死循环
			//先这样解决，根本原因待查。2017.08.26
			i++;			
			if(i>0xFFFFFF) 
				break;
		}
		_sendStartIndex_COM3=0;
     }
	u16 currAddrBase=_sendStartIndex_COM3+_waitSendCount_COM3;//当前要存数的基址
	for(u16 i=0;i<count;i++)
	{
		_DMASendBuff_COM3[currAddrBase+i]=datas[i];
	}	
	_waitSendCount_COM3=_waitSendCount_COM3+count;
	if(_isSendOK_COM3)
	{
		StartSendToDMA_COM3();
	}
}
