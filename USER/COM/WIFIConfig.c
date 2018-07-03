#include "WIFIConfig.h"
#include "string.h"
#include "COM_1.h"

DMA_InitTypeDef DMA_InitStructureSend_COM2;

void USART2_NVIC_Configuration(void)
{

}

void USART2_Configuration(void)
{
	//��1������GPIO��USART������ʱ�ӡ�ʹ�ܴ���2ʹ�õ�GPIOʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	//ʹ�ܴ���2ʱ��,USART2��APB1����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);
	//����USART2����ӳ������ 
//	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);  //40x USART2��Ҫ��ӳ��
	//��2������USART2��GPIO����Ϊ���츴��ģʽ��IO������
	GPIO_InitTypeDef GPIO_InitStructure;
	//����2 TX�ܽ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;

	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//��3������USARTR1��GPIO����Ϊ��������ģʽ������1 RX�ܽ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//���ڲ�������
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 57600; /*���ò�����Ϊ115200*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; /*��������λΪ8λ*/
	USART_InitStructure.USART_StopBits = USART_StopBits_1; /*����ֹͣλΪ1λ*/
	USART_InitStructure.USART_Parity = USART_Parity_No; /*����żУ��*/
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; /*û��Ӳ������*/
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; /*���������*/
	USART_Init(COMName2, &USART_InitStructure);
}

void USART2_DMA_Configuration(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 							// ����DMA1ʱ��    
    
	//DMA����ͨ����DMA1_ͨ��4_������6
	DMA_Cmd(SendDMAStream2, DISABLE);                                           // ��DMAͨ��,�˾���Բ���
	DMA_DeInit(SendDMAStream2);                                                 // �ָ�ȱʡֵ
	DMA_InitStructureSend_COM2.DMA_Channel = SendDMAChannel2;       			          
	DMA_InitStructureSend_COM2.DMA_PeripheralBaseAddr = (u32)(&COMName2->DR);         // ���ô��ڷ������ݼĴ���
	DMA_InitStructureSend_COM2.DMA_Memory0BaseAddr = (u32)_DMASendBuff;          // ���÷��ͻ������׵�ַ
	DMA_InitStructureSend_COM2.DMA_DIR = DMA_DIR_MemoryToPeripheral;                  // ��������λĿ�꣬�ڴ滺���� -> ����Ĵ���
	DMA_InitStructureSend_COM2.DMA_BufferSize = _comBuffLeng;                	  // ��Ҫ���͵��ֽ�����������ʵ��������Ϊ0����Ϊ��ʵ��Ҫ���͵�ʱ�򣬻��������ô�ֵ
	DMA_InitStructureSend_COM2.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         // �����ַ�������ӵ�����������������DMA�Զ�ʵ�ֵ�
	DMA_InitStructureSend_COM2.DMA_MemoryInc = DMA_MemoryInc_Enable;                  // �ڴ滺������ַ���ӵ���
	DMA_InitStructureSend_COM2.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // �������ݿ��8λ��1���ֽ�
	DMA_InitStructureSend_COM2.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          // �ڴ����ݿ��8λ��1���ֽ�
	DMA_InitStructureSend_COM2.DMA_Mode = DMA_Mode_Normal;							  // ѭ������ģʽ
	DMA_InitStructureSend_COM2.DMA_Priority = DMA_Priority_VeryHigh;                  // ���ȼ�����
    DMA_InitStructureSend_COM2.DMA_FIFOMode = DMA_FIFOMode_Disable;                   // ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ    
    DMA_InitStructureSend_COM2.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;        // ָ����FIFO��ֵˮƽ    
    DMA_InitStructureSend_COM2.DMA_MemoryBurst = DMA_MemoryBurst_Single;              // ָ����Burstת�������ڴ洫��     
    DMA_InitStructureSend_COM2.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;      // ָ����Burstת��������Χת��        
	DMA_Init(SendDMAStream2, &DMA_InitStructureSend_COM2);               		  // д������
    //DMA_ClearFlag(SendDMAFlag1);                                 				  // ���DMA���б�־,�˾���Բ���
	DMA_ITConfig(SendDMAStream2, DMA_IT_TC, ENABLE);         					  // ��������DMAͨ���ж�
    
    //DMA����ͨ����DMA1_ͨ��4_������5 
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel = RecvDMAChannel2;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&COMName2->DR);		// ���ô��ڽ������ݼĴ���
	DMA_InitStructure.DMA_Memory0BaseAddr =  (u32)_DMARecvBuff;        // ���ý��ջ������׵�ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                 // ��������Ϊ����Դ������Ĵ��� -> �ڴ滺����
	DMA_InitStructure.DMA_BufferSize = _comBuffLeng;                  // ��Ҫ�����ܽ��յ����ֽ���
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
	DMA_Init(RecvDMAStream2, &DMA_InitStructure);               		    // д������
    //DMA_ClearFlag(RecvDMAFlag1);                                 		// ���DMA���б�־
	DMA_Cmd(RecvDMAStream2, ENABLE);                            		    // ��������DMAͨ�����ȴ���������  	
}

void InitUSART_COM2(void)
{
	USART2_Configuration();
	USART2_NVIC_Configuration();
	USART2_DMA_Configuration();

	//һ��ģʽ��DMA���ͣ������жϽ���
	//	USART_DMACmd(COMName2,USART_DMAReq_Tx,ENABLE); 		//��������DMA����
	//	USART_ITConfig(COMName2, USART_IT_RXNE, ENABLE); 	//ʹ���жϷ�ʽ���ա�ʹ�ܴ��ڽ����ж�
	//	USART_Cmd(COMName2, ENABLE);											//��������

	//����ģʽ��DMA���ͣ�DMA����
	//DMA_ITConfig(RecvDMAStream2, DMA_IT_TC, ENABLE); //DMA��������ж�
	USART_ITConfig(COMName2, USART_IT_IDLE, ENABLE);  //��������1����IDEL �ж�
	USART_DMACmd(COMName2, USART_DMAReq_Tx, ENABLE);  //��������DMA����
	USART_DMACmd(COMName2, USART_DMAReq_Rx, ENABLE);  //��������DMA����
	USART_Cmd(COMName2, ENABLE);											//��������
}

void StartSendToDMA_COM2(void)
{
	_isSendOK=false;
	DMA_InitStructureSend_COM2.DMA_Memory0BaseAddr = (u32)&_DMASendBuff[_sendStartIndex];  // ���÷��ͻ������׵�ַ
	DMA_Init(SendDMAStream2, &DMA_InitStructureSend_COM2);             	// д������
	DMA_SetCurrDataCounter(SendDMAStream2,_waitSendCount); 						//����Ҫ���͵��ֽ���	
	_sendStartIndex=_sendStartIndex+_waitSendCount;
	_waitSendCount=0;		
	DMA_Cmd(SendDMAStream2, ENABLE); //��ʼDMA����
}

void SendBytes_COM2(u8* datas, u16 count)
{
	if((_sendStartIndex+_waitSendCount+count)> _comBuffLeng-1)
	{	
		while(!_isSendOK)//�ȴ�������һ���з������
		{
		}
		_sendStartIndex=0;
	}
	u16 currAddrBase=_sendStartIndex+_waitSendCount;//��ǰҪ�����Ļ�ַ
	for(u16 i=0;i<count;i++)
	{
		_DMASendBuff[currAddrBase+i]=datas[i];
	}	
	_waitSendCount=_waitSendCount+count;
	if(_isSendOK)
	{
		StartSendToDMA_COM2();
	}
}

//�򴮿ڷ����ַ�����,Ҫ��"\0"��β
void SendChars_COM2(char* data)
{
	u16 count=strlen(data);
	SendBytes_COM2((u8*)data,count);
}


