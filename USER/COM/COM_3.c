#include "COM_3.h"
#include "MemoryAlloc.h"
#include "AppTasks.h"


u8 _DMASendBuff_COM3[_com3BuffLeng];// __at(LcdSendBuffAddr);//DMA���ͻ���
u8 _DMARecvBuff_COM3[_com3BuffLeng];// __at(LcdRecvBuffAddr);//DMA���ջ��棬���ղ��ܷ���չ�ڴ棬����㶯��ʱ��ͣ���ڴ��ٶ�����

DMA_InitTypeDef DMA_InitStructureSend_COM3;
u16 _sendStartIndex_COM3=0;//������ʼλ�õ�ַ
u16 _waitSendCount_COM3=0;//�ȴ����͵ĸ���
bool _isSendOK_COM3=true;//һ�η����Ƿ����
u16 _recvStartIndex_COM3=0;//���յ���ʼλ��


//�ж����ã���nvic.c���Ѷ��壬�˴���Ϊ�淶
void USART3_NVIC_Configuration(void)
{    
		/*NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

		//�����ж����ȼ�����
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	//ʹ�ܴ���3���ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		//DMA��������ж����ã�ʹ��DMA1_Channel2���������ȼ�Ҫ���ڽ����жϷ��������쳣
		NVIC_InitStructure.NVIC_IRQChannel = SendDMAIRQ3;  
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
		NVIC_Init(&NVIC_InitStructure);  */

		//DMA��������ж����ã�ʹ��DMA1_Channel3  
//		NVIC_InitStructure.NVIC_IRQChannel = RecvDMAIRQ3;  
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
//		NVIC_Init(&NVIC_InitStructure);  
}

void USART3_Configuration(void)
{
		//��1������GPIO��USART������ʱ�ӡ�ʹ�ܴ���3ʹ�õ�GPIOʱ��
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		//ʹ�ܴ���3ʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
		//��2������USART3��GPIO����Ϊ���츴��ģʽ��IO������
		GPIO_InitTypeDef GPIO_InitStructure;
		//����3 TX�ܽ�����
		//USART3��TX��RX����Ĭ�ϵ�������λ��PB10��PB11����������λ�����ã�������ӳ�䵽PC10��PC11����������ӳ�䵽PD8��PD9��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  		GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		//��3������USARTR3��GPIO����Ϊ��������ģʽ������3 RX�ܽ�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		//���ڲ�������
		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate = 115200; /*���ò�����Ϊ115200*/
		USART_InitStructure.USART_WordLength = USART_WordLength_8b; /*��������λΪ8λ*/
		USART_InitStructure.USART_StopBits = USART_StopBits_1; /*����ֹͣλΪ1λ*/
		USART_InitStructure.USART_Parity = USART_Parity_No; /*����żУ��*/
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; /*û��Ӳ������*/
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; /*���������*/
		USART_Init(COMName3, &USART_InitStructure);
}

void USART3_DMA_Configuration(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 							  // ����DMA1ʱ��    
    
	//DMA����ͨ����DMA2_ͨ��4_������7
	DMA_Cmd(SendDMAStream3, DISABLE);                                                 // ��DMAͨ��,�˾���Բ���
	DMA_DeInit(SendDMAStream3);                                                       // �ָ�ȱʡֵ
	DMA_InitStructureSend_COM3.DMA_Channel = SendDMAChannel3;       			          
	DMA_InitStructureSend_COM3.DMA_PeripheralBaseAddr = (u32)(&COMName3->DR);         // ���ô��ڷ������ݼĴ���
	DMA_InitStructureSend_COM3.DMA_Memory0BaseAddr = (u32)_DMASendBuff_COM3;          // ���÷��ͻ������׵�ַ
	DMA_InitStructureSend_COM3.DMA_DIR = DMA_DIR_MemoryToPeripheral;                  // ��������λĿ�꣬�ڴ滺���� -> ����Ĵ���
	DMA_InitStructureSend_COM3.DMA_BufferSize = _com3BuffLeng;                	  // ��Ҫ���͵��ֽ�����������ʵ��������Ϊ0����Ϊ��ʵ��Ҫ���͵�ʱ�򣬻��������ô�ֵ
	DMA_InitStructureSend_COM3.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
	DMA_InitStructureSend_COM3.DMA_MemoryInc = DMA_MemoryInc_Enable;                  // �ڴ滺������ַ���ӵ���
	DMA_InitStructureSend_COM3.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // �������ݿ��8λ��1���ֽ�
	DMA_InitStructureSend_COM3.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          // �ڴ����ݿ��8λ��1���ֽ�
	DMA_InitStructureSend_COM3.DMA_Mode = DMA_Mode_Normal;							  // ѭ������ģʽ
	DMA_InitStructureSend_COM3.DMA_Priority = DMA_Priority_VeryHigh;                  // ���ȼ�����
    DMA_InitStructureSend_COM3.DMA_FIFOMode = DMA_FIFOMode_Disable;                   // ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ    
    DMA_InitStructureSend_COM3.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;        // ָ����FIFO��ֵˮƽ    
    DMA_InitStructureSend_COM3.DMA_MemoryBurst = DMA_MemoryBurst_Single;              // ָ����Burstת�������ڴ洫��     
    DMA_InitStructureSend_COM3.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;      // ָ����Burstת��������Χת��        
	DMA_Init(SendDMAStream3, &DMA_InitStructureSend_COM3);               		      // д������
    //DMA_ClearFlag(SendDMAFlag3);                                 				      // ���DMA���б�־,�˾���Բ���
	DMA_ITConfig(SendDMAStream3, DMA_IT_TC, ENABLE);         					      // ��������DMAͨ���ж�
    
    //DMA����ͨ����DMA2_ͨ��4_������2 
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel = RecvDMAChannel3;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&COMName3->DR);		// ���ô��ڽ������ݼĴ���
	DMA_InitStructure.DMA_Memory0BaseAddr =  (u32)_DMARecvBuff_COM3;        // ���ý��ջ������׵�ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                 // ��������Ϊ����Դ������Ĵ��� -> �ڴ滺����
	DMA_InitStructure.DMA_BufferSize = _com3BuffLeng;                  // ��Ҫ�����ܽ��յ����ֽ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ滺������ַ���ӵ���
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��8λ��1���ֽ�
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ����ݿ��8λ��1���ֽ�
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                         // ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // ���ȼ�����  
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;       // ָ����FIFO��ֵˮƽ 
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             // ָ����Burstת�������ڴ洫��   
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     // ָ����Burstת��������Χת��  
	DMA_Init(RecvDMAStream3, &DMA_InitStructure);               		    // д������
  //DMA_ClearFlag(RecvDMAFlag3);                                 		    // ���DMA���б�־
	DMA_Cmd(RecvDMAStream3, ENABLE);                            		    // ��������DMAͨ�����ȴ���������	
}

void InitUSART_COM3(void)
{
		USART3_Configuration();
		USART3_NVIC_Configuration();
		USART3_DMA_Configuration();

	//  һ��ģʽ��DMA���ͣ������жϽ���
    //	USART_DMACmd(COMName3,USART_DMAReq_Tx,ENABLE); 		//��������DMA����
    //	USART_ITConfig(COMName3, USART_IT_RXNE, ENABLE); 	//ʹ���жϷ�ʽ���ա�ʹ�ܴ��ڽ����ж�
    //	USART_Cmd(COMName3, ENABLE);											//��������

	//  ����ģʽ��DMA���ͣ�DMA����
    //  DMA_ITConfig(RecvDMAStream3, DMA_IT_TC, ENABLE); //DMA��������ж�
		USART_ITConfig(COMName3, USART_IT_IDLE, ENABLE);  //��������3����IDEL �ж�
		USART_DMACmd(COMName3, USART_DMAReq_Tx, ENABLE);  //��������DMA����
		USART_DMACmd(COMName3, USART_DMAReq_Rx, ENABLE);  //��������DMA����
		USART_Cmd(COMName3, ENABLE);											//��������
}

void StartSendToDMA_COM3(void)
{
	//_isSendOK_COM3=false;
	DMA_InitStructureSend_COM3.DMA_Memory0BaseAddr = (u32)&_DMASendBuff_COM3[_sendStartIndex_COM3];  // ���÷��ͻ������׵�ַ
	DMA_Init(SendDMAStream3, &DMA_InitStructureSend_COM3);             	// д������
	DMA_SetCurrDataCounter(SendDMAStream3,_waitSendCount_COM3); 						//����Ҫ���͵��ֽ���	
	_sendStartIndex_COM3=_sendStartIndex_COM3+_waitSendCount_COM3;
    _waitSendCount_COM3=0;		
	DMA_Cmd(SendDMAStream3, ENABLE); //��ʼDMA����
	_isSendOK_COM3=false;
}

void SendBytes_COM3(u8* datas, u16 count)
{
     if(count>=_com3BuffLeng)
         count=count;
     if((_sendStartIndex_COM3+_waitSendCount_COM3+count)> _com3BuffLeng-1)
	 {	
		u32 i=0;
		while(!_isSendOK_COM3)//�ȴ�������һ���з������
		{
			//DO:�������ش򰴼���������Ļ�ϵİ�ť�����ͻ��ڴ���ѭ��
			//���������������ԭ����顣2017.08.26
			i++;			
			if(i>0xFFFFFF) 
				break;
		}
		_sendStartIndex_COM3=0;
     }
	u16 currAddrBase=_sendStartIndex_COM3+_waitSendCount_COM3;//��ǰҪ�����Ļ�ַ
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
