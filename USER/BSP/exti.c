#include "exti.h"
/*
PG10--X��λ�ж�
PG11--Y1��λ�ж�
PG12--Z��λ�ж�
PG13--Y2��λ�ж�
*/
void Exti_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStruture;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE, ENABLE);
	 /* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	//M1��λ�ж�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, GPIO_PinSource10);
	EXTI_InitStruture.EXTI_Line = EXTI_Line10;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);	
	//M2��λ�ж�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, GPIO_PinSource11);
	EXTI_InitStruture.EXTI_Line = EXTI_Line11;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//M3��λ�ж�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, GPIO_PinSource12);
	EXTI_InitStruture.EXTI_Line = EXTI_Line12;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//M4��λ�ж�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, GPIO_PinSource13);
	EXTI_InitStruture.EXTI_Line = EXTI_Line13;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//M5��λ�ж�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, GPIO_PinSource0);
	EXTI_InitStruture.EXTI_Line = EXTI_Line0;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//M6��λ�ж�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, GPIO_PinSource1);
	EXTI_InitStruture.EXTI_Line = EXTI_Line1;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//����˿�����ϼ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, GPIO_PinSource6);
	EXTI_InitStruture.EXTI_Line = EXTI_Line6;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, GPIO_PinSource7);
	EXTI_InitStruture.EXTI_Line = EXTI_Line7;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//�����ж�	Y������
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, GPIO_PinSource15);
	EXTI_InitStruture.EXTI_Line = EXTI_Line15;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);	
	//�����ж�	Y2����
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, GPIO_PinSource3);
	EXTI_InitStruture.EXTI_Line = EXTI_Line3;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//�����ж�	�󹤼��������
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, GPIO_PinSource5);
	EXTI_InitStruture.EXTI_Line = EXTI_Line5;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//�����ж�	�ҹ����������
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, GPIO_PinSource8);
	EXTI_InitStruture.EXTI_Line = EXTI_Line8;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//�����ж�	Ť���ź�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, GPIO_PinSource9);
	EXTI_InitStruture.EXTI_Line = EXTI_Line9;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//�����ж�	�ش�
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, GPIO_PinSource2);
	EXTI_InitStruture.EXTI_Line = EXTI_Line2;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//�����ж�	ֹͣ
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, GPIO_PinSource4);
	EXTI_InitStruture.EXTI_Line = EXTI_Line4;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
}

