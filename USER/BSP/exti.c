#include "exti.h"
/*
PG10--X限位中断
PG11--Y1限位中断
PG12--Z限位中断
PG13--Y2限位中断
*/
void Exti_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStruture;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE, ENABLE);
	 /* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	//M1限位中断
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, GPIO_PinSource10);
	EXTI_InitStruture.EXTI_Line = EXTI_Line10;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);	
	//M2限位中断
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, GPIO_PinSource11);
	EXTI_InitStruture.EXTI_Line = EXTI_Line11;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//M3限位中断
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, GPIO_PinSource12);
	EXTI_InitStruture.EXTI_Line = EXTI_Line12;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//M4限位中断
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, GPIO_PinSource13);
	EXTI_InitStruture.EXTI_Line = EXTI_Line13;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//M5限位中断
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, GPIO_PinSource0);
	EXTI_InitStruture.EXTI_Line = EXTI_Line0;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//M6限位中断
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, GPIO_PinSource1);
	EXTI_InitStruture.EXTI_Line = EXTI_Line1;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//吹螺丝中推料检测
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
	//按键中断	Y左启动
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, GPIO_PinSource15);
	EXTI_InitStruture.EXTI_Line = EXTI_Line15;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);	
	//按键中断	Y2启动
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, GPIO_PinSource3);
	EXTI_InitStruture.EXTI_Line = EXTI_Line3;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//按键中断	左工件检测启动
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, GPIO_PinSource5);
	EXTI_InitStruture.EXTI_Line = EXTI_Line5;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//按键中断	右工件检测启动
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, GPIO_PinSource8);
	EXTI_InitStruture.EXTI_Line = EXTI_Line8;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//按键中断	扭力信号
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, GPIO_PinSource9);
	EXTI_InitStruture.EXTI_Line = EXTI_Line9;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//按键中断	重打
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, GPIO_PinSource2);
	EXTI_InitStruture.EXTI_Line = EXTI_Line2;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
	//按键中断	停止
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, GPIO_PinSource4);
	EXTI_InitStruture.EXTI_Line = EXTI_Line4;
	EXTI_InitStruture.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruture.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruture.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruture);
}

