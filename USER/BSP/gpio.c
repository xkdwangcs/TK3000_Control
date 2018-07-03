#include "gpio.h"
#include "Delay.h"

//����˿��б�
IOPortStruct OutPortList[OutPortNum];
//����˿��б�
IOPortStruct InPortList[InPortNum];
//����˿��б�
MotorPortStruct MotorPortList[MotorPortNum];
//�������ƶ˿�
IOPortStruct ScrewDriverPort;
//��յ�ŷ����ƶ˿�
IOPortStruct VacuumValvePort;
//����������˿�
IOPortStruct AlarmSundPort;
//����������˿�
IOPortStruct AlarmLampPort;
//�������׿��ƶ˿�
IOPortStruct LockQGPort;
//���Ͽ��ƶ˿�
//IOPortStruct PushMaterPort;
//���Ͽ��ƶ˿�
IOPortStruct BlowMaterPort;

//��λ��ť
//IOPortStruct ResetPort;
//��������ť
IOPortStruct LeftStartPort;
//��������ť
IOPortStruct RightStartPort;
//ֹͣ��ť
IOPortStruct StopPort;
//�ش�ť������ȥ��
IOPortStruct RedoPort;
//��˿׼�����ź������
IOPortStruct ScrewRedyPort;
//��˿��ȡ���ź������
IOPortStruct ScrewInhalePort;
//Ť���ź������
IOPortStruct TorquePort;
//��ƽ̨��������ź������
IOPortStruct LeftWorkpiecePort;
//��ƽ̨��������ź������
IOPortStruct RightWorkpiecePort;

//��������˿����ü���ʼ��
void OutPortListConfig()
{
	for(u8 i=0;i<3;i++)
	{
		OutPortList[i].Port=GPIOD;
	}
	OutPortList[3].Port=GPIOC;	
	OutPortList[4].Port=GPIOA;
	OutPortList[5].Port=GPIOA;
	OutPortList[6].Port=GPIOC;	
	OutPortList[7].Port=GPIOC;	
	for(u8 i=8;i<10;i++)
	{
		OutPortList[i].Port=GPIOG;
	}
	OutPortList[10].Port=GPIOG;
	OutPortList[11].Port=GPIOD;
	OutPortList[12].Port=GPIOB;
	OutPortList[13].Port=GPIOB;
	OutPortList[14].Port=GPIOF;
	OutPortList[15].Port=GPIOC;
	
	OutPortList[0].Pin=GPIO_Pin_6;//D6
	OutPortList[1].Pin=GPIO_Pin_3;//D3
	OutPortList[2].Pin=GPIO_Pin_2;//D2
	OutPortList[3].Pin=GPIO_Pin_12;//C12
	OutPortList[4].Pin=GPIO_Pin_15;//A15
	OutPortList[5].Pin=GPIO_Pin_8;//A8
	OutPortList[6].Pin=GPIO_Pin_9;//C9
	OutPortList[7].Pin=GPIO_Pin_8;//C8
	OutPortList[8].Pin=GPIO_Pin_8;//G8
	OutPortList[9].Pin=GPIO_Pin_7;//G7
	OutPortList[10].Pin=GPIO_Pin_6;//G6
	OutPortList[11].Pin=GPIO_Pin_13;//D13
	OutPortList[12].Pin=GPIO_Pin_15;//B15
	OutPortList[13].Pin=GPIO_Pin_12;//B12
	OutPortList[14].Pin=GPIO_Pin_11;//F11
	OutPortList[15].Pin=GPIO_Pin_5;//C5

	//��ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;
	for(u8 i=0;i<OutPortNum;i++)
	{
		GPIO_InitStructure.GPIO_Pin = OutPortList[i].Pin;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
		GPIO_Init(OutPortList[i].Port, &GPIO_InitStructure);
		GPIO_ResetBits(OutPortList[i].Port,OutPortList[i].Pin);
	}

	//������LED3	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

//����˿�����
void InPortListConfig()
{
	for(u8 i=0;i<5;i++)
	{
		InPortList[i].Port=GPIOG;
	}
	for(u8 i=5;i<10;i++)
	{
		InPortList[i].Port=GPIOB;
	}
	for(u8 i=10;i<15;i++)
	{
		InPortList[i].Port=GPIOE;
	}
	for(u8 i=15;i<20;i++)
	{
		InPortList[i].Port=GPIOF;
	}
	for(u8 i=20;i<InPortNum;i++)
	{
		InPortList[i].Port=GPIOC;
	}

	InPortList[0].Pin=GPIO_Pin_10;	//G10	LIMIT NEG M1 X
	InPortList[1].Pin=GPIO_Pin_11;	//G11	LIMIT NEG M2 Y1
	InPortList[2].Pin=GPIO_Pin_12;	//G12	LIMIT NEG M3 Z
	InPortList[3].Pin=GPIO_Pin_13;	//G13	LIMIT NEG M4 Y2
	InPortList[4].Pin=GPIO_Pin_15;	//G15	������
	InPortList[5].Pin=GPIO_Pin_3;	//B3	������
	InPortList[6].Pin=GPIO_Pin_4;	//B4	
	InPortList[7].Pin=GPIO_Pin_5;	//B5	�󹤼�����ź�
	InPortList[8].Pin=GPIO_Pin_8;	//B8	�ҹ�������ź�
	InPortList[9].Pin=GPIO_Pin_9;	//B9	Ť���ź�
	InPortList[10].Pin=GPIO_Pin_3;	//E3	
	InPortList[11].Pin=GPIO_Pin_2;	//E2	�ش� 
	InPortList[12].Pin=GPIO_Pin_4;	//E4	��ͣ
	InPortList[13].Pin=GPIO_Pin_5;	//E5	��˿��׼�����ź�	
	InPortList[14].Pin=GPIO_Pin_6;	//E6	��˿��ȡ���ź�/ ����˿���
	InPortList[15].Pin=GPIO_Pin_6;	//F6	
	InPortList[16].Pin=GPIO_Pin_7;	//F7	
	InPortList[17].Pin=GPIO_Pin_8;	//F8
	InPortList[18].Pin=GPIO_Pin_9;	//F9		
	InPortList[19].Pin=GPIO_Pin_10;	//F10		
	InPortList[20].Pin=GPIO_Pin_0;	//C0	LIMIT NEG M5 A
	InPortList[21].Pin=GPIO_Pin_1;	//C1	LIMIT NEG M6 W
	InPortList[22].Pin=GPIO_Pin_2;	//C2
	InPortList[23].Pin=GPIO_Pin_3;	//C3

	//��ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;
	for(u8 i=0;i<InPortNum;i++)
	{
		GPIO_InitStructure.GPIO_Pin = InPortList[i].Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(InPortList[i].Port, &GPIO_InitStructure);
	}	
	//��������ť
	LeftStartPort= InPortList[4];
	//��������ť
	RightStartPort= InPortList[5];
	//��ƽ̨��������ź������
	LeftWorkpiecePort=InPortList[7];
	//��ƽ̨��������ź������
	RightWorkpiecePort=InPortList[8];
	//Ť���ź�
	TorquePort = InPortList[9];
	//�ش�ť
	RedoPort=InPortList[11]; //ԭ��10���ش�˿ڴ��󣬵��̻߳�������
	//ֹͣ��ť
	StopPort= InPortList[12];
	//������׼���ź������
	ScrewRedyPort=InPortList[13];
	//����ź������
	ScrewInhalePort=InPortList[14];
}

//��ʼ����˿�
void IniMotorPort(void)
{
	u16 Prescaler=14;//SystemCoreClock/2/T_FREQ-1;      //Ԥ��Ƶ��,F=168M/2/6000000-1=13
	u32 timCLK=SystemCoreClock/2;
	MotorPortStruct mps1={GPIOA,GPIO_Pin_0,GPIO_PinSource0,GPIO_AF_TIM2,GPIOA,GPIO_Pin_1,InPortList[0],Prescaler,timCLK};		//M1//X���PUL,DIR 
	MotorPortStruct mps2={GPIOA,GPIO_Pin_2,GPIO_PinSource2,GPIO_AF_TIM5,GPIOA,GPIO_Pin_3,InPortList[1],Prescaler,timCLK};		//M2//Y���PUL,DIR
	MotorPortStruct mps3={GPIOB,GPIO_Pin_0,GPIO_PinSource0,GPIO_AF_TIM3,GPIOB,GPIO_Pin_1,InPortList[2],Prescaler,timCLK};		//M3//Z���PUL,DIR
	MotorPortStruct mps4={GPIOB,GPIO_Pin_6,GPIO_PinSource6,GPIO_AF_TIM4,GPIOB,GPIO_Pin_7,InPortList[3],Prescaler,timCLK};		//M4//Y2���PUL,DIR
	MotorPortStruct mps5={GPIOB,GPIO_Pin_13,GPIO_PinSource13,GPIO_AF_TIM1,GPIOB,GPIO_Pin_14,InPortList[20],Prescaler*2,SystemCoreClock};	//M5//A���PUL,DIR   
	MotorPortStruct mps6={GPIOC,GPIO_Pin_7,GPIO_PinSource7,GPIO_AF_TIM8,GPIOC,GPIO_Pin_6,InPortList[21],Prescaler*2,SystemCoreClock};		//M6//W���PUL,DIR
	
	MotorPortList[M1]=mps1;	
	MotorPortList[M2]=mps2;
	MotorPortList[M3]=mps3;
	MotorPortList[M4]=mps4;
	MotorPortList[M5]=mps5;
	MotorPortList[M6]=mps6;
};

void PWMConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOF,ENABLE);
	for(u8 id=0; id<MotorPortNum; id++)
	{
		GPIO_InitStructure.GPIO_Pin = MotorPortList[id].PulsePin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(MotorPortList[id].PulsePort, &GPIO_InitStructure);
		
		GPIO_PinAFConfig(MotorPortList[id].PulsePort,MotorPortList[id].PulsePinSource,MotorPortList[id].GPIO_AF);
		GPIO_SetBits(MotorPortList[id].PulsePort, MotorPortList[id].PulsePin);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;;
		GPIO_InitStructure.GPIO_Pin = MotorPortList[id].DirectPin;
		GPIO_Init(MotorPortList[id].DirectPort, &GPIO_InitStructure);
		GPIO_SetBits(MotorPortList[id].DirectPort, MotorPortList[id].DirectPin);

		//�������ʼ�����ѵ��ã��˴�����Ҫ����
//		GPIO_InitStructure.GPIO_Pin=MotorPortList[id].LimitPort.Pin;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//		GPIO_Init(MotorPortList[id].LimitPort.Port, &GPIO_InitStructure);	
	}
}
//���롢����ڳ�ʼ��
void GPIO_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|
		RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOF,ENABLE);	
	OutPortListConfig();	//����˿����ü���ʼ��
	InPortListConfig();		//����˿�����
	IniMotorPort(); 		//����˿�����
	PWMConfig();
}

//��ѯ�����״̬
//OutPortList:����˿ڣ����أ�true:�ߵ�ƽ,false:�͵�ƽ
bool GetOutPortState(IOPortStruct outPort)
{
	return GPIO_ReadOutputDataBit(outPort.Port, outPort.Pin);
}

//��ѯ�����״̬
//inPort:����˿ڣ����أ�true:�ߵ�ƽ,false:�͵�ƽ
bool GetInPortState(IOPortStruct inPort)
{
	return GPIO_ReadInputDataBit(inPort.Port, inPort.Pin);
}

//���������״̬���Ƿ�ʹ��(�ߵ�ƽ)
//OutPortList:����˿ڣ���0��ʼ��isEnabled:�Ƿ�Ϊ�ߵ�ƽ
void SetOutPortState(IOPortStruct OutPortList,bool isEnabled)
{
	if(isEnabled)
		GPIO_SetBits(OutPortList.Port, OutPortList.Pin);
	else
		GPIO_ResetBits(OutPortList.Port, OutPortList.Pin);
}

//���������״̬���Ƿ�ʹ��
//portIndex:�˿���ţ���0��ʼ��isEnabled:�Ƿ�Ϊ�ߵ�ƽ
void SetOutPortState_Index(u8 portIndex,bool isEnabled)
{
	SetOutPortState(OutPortList[portIndex],isEnabled);
}

//��ѯ�����״̬
//portIndex:�˿���ţ���0��ʼ
bool GetOutPortState_Index(u8 portIndex)
{
	return GetOutPortState(OutPortList[portIndex]);
}

//��ѯ����˿�״̬
bool GetInPortState_Index(u8 portIndex)
{
	return GetInPortState(InPortList[portIndex]);
}

//����ָʾ����˸
void LightAlarmPCB(u8 count)
{
	for(int i = 0;i<count;i++)	
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
		Delay_ms(200);
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		Delay_ms(200);
	}
}
