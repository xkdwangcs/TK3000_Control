#include "gpio.h"
#include "Delay.h"

//输出端口列表
IOPortStruct OutPortList[OutPortNum];
//输入端口列表
IOPortStruct InPortList[InPortNum];
//电机端口列表
MotorPortStruct MotorPortList[MotorPortNum];
//电批控制端口
IOPortStruct ScrewDriverPort;
//真空电磁阀控制端口
IOPortStruct VacuumValvePort;
//报警声输出端口
IOPortStruct AlarmSundPort;
//报警灯输出端口
IOPortStruct AlarmLampPort;
//锁付气缸控制端口
IOPortStruct LockQGPort;
//推料控制端口
//IOPortStruct PushMaterPort;
//吹料控制端口
IOPortStruct BlowMaterPort;

//复位按钮
//IOPortStruct ResetPort;
//左启动按钮
IOPortStruct LeftStartPort;
//右启动按钮
IOPortStruct RightStartPort;
//停止按钮
IOPortStruct StopPort;
//重打按钮，可以去掉
IOPortStruct RedoPort;
//镙丝准备好信号输入口
IOPortStruct ScrewRedyPort;
//镙丝吸取好信号输入口
IOPortStruct ScrewInhalePort;
//扭力信号输入口
IOPortStruct TorquePort;
//左平台工件检测信号输入口
IOPortStruct LeftWorkpiecePort;
//右平台工件检测信号输入口
IOPortStruct RightWorkpiecePort;

//其它输出端口配置及初始化
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

	//初始化
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

	//测试用LED3	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

//输入端口配置
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
	InPortList[4].Pin=GPIO_Pin_15;	//G15	左启动
	InPortList[5].Pin=GPIO_Pin_3;	//B3	右启动
	InPortList[6].Pin=GPIO_Pin_4;	//B4	
	InPortList[7].Pin=GPIO_Pin_5;	//B5	左工件检测信号
	InPortList[8].Pin=GPIO_Pin_8;	//B8	右工件检测信号
	InPortList[9].Pin=GPIO_Pin_9;	//B9	扭力信号
	InPortList[10].Pin=GPIO_Pin_3;	//E3	
	InPortList[11].Pin=GPIO_Pin_2;	//E2	重打 
	InPortList[12].Pin=GPIO_Pin_4;	//E4	急停
	InPortList[13].Pin=GPIO_Pin_5;	//E5	镙丝盒准备好信号	
	InPortList[14].Pin=GPIO_Pin_6;	//E6	镙丝吸取好信号/ 吹螺丝检测
	InPortList[15].Pin=GPIO_Pin_6;	//F6	
	InPortList[16].Pin=GPIO_Pin_7;	//F7	
	InPortList[17].Pin=GPIO_Pin_8;	//F8
	InPortList[18].Pin=GPIO_Pin_9;	//F9		
	InPortList[19].Pin=GPIO_Pin_10;	//F10		
	InPortList[20].Pin=GPIO_Pin_0;	//C0	LIMIT NEG M5 A
	InPortList[21].Pin=GPIO_Pin_1;	//C1	LIMIT NEG M6 W
	InPortList[22].Pin=GPIO_Pin_2;	//C2
	InPortList[23].Pin=GPIO_Pin_3;	//C3

	//初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	for(u8 i=0;i<InPortNum;i++)
	{
		GPIO_InitStructure.GPIO_Pin = InPortList[i].Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(InPortList[i].Port, &GPIO_InitStructure);
	}	
	//左启动按钮
	LeftStartPort= InPortList[4];
	//右启动按钮
	RightStartPort= InPortList[5];
	//左平台工件检测信号输入口
	LeftWorkpiecePort=InPortList[7];
	//右平台工件检测信号输入口
	RightWorkpiecePort=InPortList[8];
	//扭力信号
	TorquePort = InPortList[9];
	//重打按钮
	RedoPort=InPortList[11]; //原口10，重打端口错误，但线程还有问题
	//停止按钮
	StopPort= InPortList[12];
	//供料器准备信号输入口
	ScrewRedyPort=InPortList[13];
	//真空信号输入口
	ScrewInhalePort=InPortList[14];
}

//初始电机端口
void IniMotorPort(void)
{
	u16 Prescaler=14;//SystemCoreClock/2/T_FREQ-1;      //预分频器,F=168M/2/6000000-1=13
	u32 timCLK=SystemCoreClock/2;
	MotorPortStruct mps1={GPIOA,GPIO_Pin_0,GPIO_PinSource0,GPIO_AF_TIM2,GPIOA,GPIO_Pin_1,InPortList[0],Prescaler,timCLK};		//M1//X电机PUL,DIR 
	MotorPortStruct mps2={GPIOA,GPIO_Pin_2,GPIO_PinSource2,GPIO_AF_TIM5,GPIOA,GPIO_Pin_3,InPortList[1],Prescaler,timCLK};		//M2//Y电机PUL,DIR
	MotorPortStruct mps3={GPIOB,GPIO_Pin_0,GPIO_PinSource0,GPIO_AF_TIM3,GPIOB,GPIO_Pin_1,InPortList[2],Prescaler,timCLK};		//M3//Z电机PUL,DIR
	MotorPortStruct mps4={GPIOB,GPIO_Pin_6,GPIO_PinSource6,GPIO_AF_TIM4,GPIOB,GPIO_Pin_7,InPortList[3],Prescaler,timCLK};		//M4//Y2电机PUL,DIR
	MotorPortStruct mps5={GPIOB,GPIO_Pin_13,GPIO_PinSource13,GPIO_AF_TIM1,GPIOB,GPIO_Pin_14,InPortList[20],Prescaler*2,SystemCoreClock};	//M5//A电机PUL,DIR   
	MotorPortStruct mps6={GPIOC,GPIO_Pin_7,GPIO_PinSource7,GPIO_AF_TIM8,GPIOC,GPIO_Pin_6,InPortList[21],Prescaler*2,SystemCoreClock};		//M6//W电机PUL,DIR
	
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

		//在输入初始化中已调用，此处不需要调用
//		GPIO_InitStructure.GPIO_Pin=MotorPortList[id].LimitPort.Pin;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//		GPIO_Init(MotorPortList[id].LimitPort.Port, &GPIO_InitStructure);	
	}
}
//输入、输出口初始化
void GPIO_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|
		RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOF,ENABLE);	
	OutPortListConfig();	//输出端口配置及初始化
	InPortListConfig();		//输入端口配置
	IniMotorPort(); 		//电机端口配置
	PWMConfig();
}

//查询输出口状态
//OutPortList:输出端口，返回：true:高电平,false:低电平
bool GetOutPortState(IOPortStruct outPort)
{
	return GPIO_ReadOutputDataBit(outPort.Port, outPort.Pin);
}

//查询输入口状态
//inPort:输入端口，返回：true:高电平,false:低电平
bool GetInPortState(IOPortStruct inPort)
{
	return GPIO_ReadInputDataBit(inPort.Port, inPort.Pin);
}

//设置输出口状态，是否使能(高电平)
//OutPortList:输出端口，从0开始；isEnabled:是否为高电平
void SetOutPortState(IOPortStruct OutPortList,bool isEnabled)
{
	if(isEnabled)
		GPIO_SetBits(OutPortList.Port, OutPortList.Pin);
	else
		GPIO_ResetBits(OutPortList.Port, OutPortList.Pin);
}

//设置输出口状态，是否使能
//portIndex:端口序号，从0开始；isEnabled:是否为高电平
void SetOutPortState_Index(u8 portIndex,bool isEnabled)
{
	SetOutPortState(OutPortList[portIndex],isEnabled);
}

//查询输出口状态
//portIndex:端口序号，从0开始
bool GetOutPortState_Index(u8 portIndex)
{
	return GetOutPortState(OutPortList[portIndex]);
}

//查询输入端口状态
bool GetInPortState_Index(u8 portIndex)
{
	return GetInPortState(InPortList[portIndex]);
}

//板载指示灯闪烁
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
