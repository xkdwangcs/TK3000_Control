/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    21-October-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "gpio.h"
#include "Protocol.h"
#include "COM_1.h"
#include "WIFI.h"
#include "MainForm.h"
#include "WorkControl.h"
#include "CurveBase.h"
#include "AppTasks.h"
#include "WorkDataFile.h"
#include "Delay.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

void USART1_IRQHandler(void)
{
	u16 i=0;
	if(USART_GetITStatus(COMName1, USART_IT_IDLE) != RESET)  // 空闲中断
	{		
		DMA_Cmd(RecvDMAStream1, DISABLE);       // 关闭DMA ，防止干扰	
		//读SR后读DR,清USART_IT_IDLE标志  
		i = COMName1->SR; 
		i = COMName1->DR;  	
		//USART_ClearFlag(COMName1, USART_IT_IDLE);//此函数的功能跟下面这个是一样的
		USART_ClearITPendingBit(COMName1, USART_IT_IDLE); //清除中断标志位，根据标志位来执行相应的中断函数。     		
		DMA_ClearFlag(RecvDMAStream1,RecvDMAFlag1);
		DMA_Cmd(RecvDMAStream1, ENABLE); 
		DisPackage();		
	} 
}

//串口1的DMA发送中断
void DMA2_Stream7_IRQHandler(void)
{    
	//if (DMA_GetITStatus(SendDMAStream1,SendDMAFlag1) != RESET)   //这条语句不行
    if (DMA_GetITStatus(SendDMAStream1,DMA_IT_TCIF7) != RESET)
	{
		DMA_ClearFlag(SendDMAStream1,SendDMAFlag1);
		DMA_Cmd(SendDMAStream1, DISABLE);	
		
		if(_waitSendCount>0)
		{
			StartSendToDMA_COM1();
		}
		else
		{
			_isSendOK=true;
		}
	}
}     

void USART2_IRQHandler(void)
{    
	u16 i=0;
	if(USART_GetITStatus(COMName2, USART_IT_IDLE) != RESET)  // 空闲中断
	{		
		DMA_Cmd(RecvDMAStream2, DISABLE);       // 关闭DMA,防止干扰	
		//读SR后读DR,清USART_IT_IDLE标志  
		i = COMName2->SR; 
		i = COMName2->DR;  	
		USART_ClearITPendingBit(COMName2, USART_IT_IDLE); //清除中断标志位，根据标志位来执行相应的中断函数。     		
		DMA_ClearFlag(RecvDMAStream2,RecvDMAFlag2);
		DMA_Cmd(RecvDMAStream2, ENABLE); 
		DisPackage();		
	} 
}

//串口2的DMA发送中断
void DMA1_Stream6_IRQHandler(void)
{
	if (DMA_GetITStatus(SendDMAStream2,DMA_IT_TCIF6) != RESET)
	{
		DMA_ClearFlag(SendDMAStream2,SendDMAFlag2);
		DMA_Cmd(SendDMAStream2, DISABLE);				
		if(_waitSendCount>0)
		{
			StartSendToDMA_COM2();
		}
		else
		{
			_isSendOK=true;
		}
	}
}

//串口3接收中断函数
void USART3_IRQHandler(void)
{
	u16 i=0;
	if(USART_GetITStatus(COMName3, USART_IT_IDLE) != RESET)  // 空闲中断
	{				
		DMA_Cmd(RecvDMAStream3, DISABLE);       // 关闭DMA ，防止干扰	
		//读SR后读DR,清USART_IT_IDLE标志  
		i = COMName3->SR; 
		i = COMName3->DR;
		USART_ClearITPendingBit(COMName3, USART_IT_IDLE); //清除中断标志位，根据标志位来执行相应的中断函数。     		
		DMA_ClearFlag(RecvDMAStream3,RecvDMAFlag3);
		DMA_Cmd(RecvDMAStream3, ENABLE); 
		DisPackage();		
	} 
}

//串口3的DMA发送中断,传输完成后中断。DMA_ITConfig(USART3SendDMAChannel, DMA_IT_TC, ENABLE); DMA_IT_TC:完成中断
void DMA1_Stream3_IRQHandler(void)
{
	if (DMA_GetITStatus(SendDMAStream3,DMA_IT_TCIF3) != RESET)
	{
		DMA_ClearFlag(SendDMAStream3,SendDMAFlag3);
		DMA_Cmd(SendDMAStream3, DISABLE);				
		if(_waitSendCount_COM3>0)
		{
			StartSendToDMA_COM3();
		}
		else
		{
			_isSendOK_COM3=true;
		}
	}
}
 
//0号中断--M5左限位 A 		
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		OriginLimitFunc(M5);
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}
//1号中断--M6左限位 W
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		OriginLimitFunc(M6);
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}
//重打
void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2) != RESET)
	{
		if (GetInPortState(RedoPort)==0)
		{
			Delay_ms(200);
			if (GetInPortState(RedoPort)==0)
			{
				RepeatWork();
			}
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}
//右启动
void EXTI3_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		if (GetInPortState(RightStartPort)==0)
		{
			if(Parameter.IsFJQD && !_isWaitSemaphore)//放件启动不允许从按钮启动,故障后可以
				return;
			StartWorkTask(RightPlatform,Key);
		}	
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}
//急停 
void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
//		if(_currFormIndex==23)
//		{
//			EXTI_ClearITPendingBit(EXTI_Line4);
//			return;
//		}
		Delay_ms(100);
		if (GetInPortState(StopPort))
		{
			ScramStop(KeyStop);
			while(true)
			{
				if(!GetInPortState(StopPort))
				{
					Delay_ms(500);
					if(!GetInPortState(StopPort))
					{
						//ShowWelcomeForm();
						Delay_ms(100);
						SystemReset();
						break;
					}
				}
			}
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}

void EXTI9_5_IRQHandler(void)
{	
//左工件检测信号
	if (EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		if (GetInPortState(LeftWorkpiecePort)==0 && Parameter.IsFJQD)
		{
			Delay_ms(200);
			if(!GetInPortState(LeftWorkpiecePort))
			{
				StartWorkTask(LeftPlatform,FJQD);
			}
		}		
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	//6号中断
	if (EXTI_GetITStatus(EXTI_Line6) != RESET)
	{
		//_screwIsOK = true;
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
	//7号中断
	if (EXTI_GetITStatus(EXTI_Line7) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	//右工件检测信号
	if (EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		if (GetInPortState(RightWorkpiecePort)==0 && Parameter.IsFJQD)
		{
			Delay_ms(200);
			if(!GetInPortState(RightWorkpiecePort))
			{
				StartWorkTask(RightPlatform,FJQD);
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	//扭力信号，关闭电批
	if (EXTI_GetITStatus(EXTI_Line9) != RESET)
	{
//		if (GetInPortState(TorquePort))
//		{
//			SetOutPortState(ScrewDriverPort,false);//关电批
//		}
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

void EXTI15_10_IRQHandler(void)
{
	//10号中断--M1左限位 X
	if (EXTI_GetITStatus(EXTI_Line10) != RESET)
	{
		OriginLimitFunc(M1);
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
	//11号中断--M2左限位 Y1
	if (EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
		OriginLimitFunc(M2);
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
	//12号中断--M3左限位 Z
	if (EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
		OriginLimitFunc(M3);
		EXTI_ClearITPendingBit(EXTI_Line12);
	}	
	//13号中断--M4轴左限位 Y2
	if (EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		OriginLimitFunc(M4);
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
/*	
	//14号中断--未使用
if (EXTI_GetITStatus(EXTI_Line14) != RESET)
	{

		EXTI_ClearITPendingBit(EXTI_Line14);
	}
*/
	//15号中断--左启动
	if (EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		if (GetInPortState(LeftStartPort)==0)
		{
			if(Parameter.IsFJQD && !_isWaitSemaphore)//放件启动不允许从按钮启动,故障后可以
				return;
			StartWorkTask(LeftPlatform,Key);
		}
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
	
}
#include "LineRunin.h"
//M1 PWM中断 X
void TIM2_IRQHandler(void)
{
	//if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		TIM2->CCR1=MotorList[M1].TimPeriod>>1;//占空比
		TIM2->ARR=MotorList[M1].TimPeriod-1;  //预装载
		PWM(M1);
	}
	//XYQ,2018.0324,要加此句，两次清除定时器标志，并且中断中调用PWM函数要放在判断之内
	//否则DISABLE后还会触发一次，每次会多一个脉冲
	//TIM2->CCR1=MotorList[M1].TimPeriod>>1;//占空比
	//TIM2->ARR=MotorList[M1].TimPeriod-1;  //预装载
	//PWM(M1);
}

//M2 PWM中断 Y1
void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		TIM5->CCR3=MotorList[M2].TimPeriod>>1;
		TIM5->ARR=MotorList[M2].TimPeriod-1; 
		PWM(M2);
	}
}

//M3 PWM中断 Z
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		 TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		 TIM3->CCR3=MotorList[M3].TimPeriod>>1;
		 TIM3->ARR=MotorList[M3].TimPeriod-1; 
		 PWM(M3);
	}	
}

//M4 PWM中断 Y2
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		TIM4->CCR1=MotorList[M4].TimPeriod>>1;
		TIM4->ARR=MotorList[M4].TimPeriod-1; 	
		PWM(M4);
	}	
}

//M5 PWM中断 A
void TIM1_UP_TIM10_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		TIM1->CCR1=MotorList[M5].TimPeriod>>1;
		TIM1->ARR=MotorList[M5].TimPeriod-1; 
		PWM(M5);
	}	
}

//M6 PWM中断 W
void TIM8_UP_TIM13_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		TIM8->CCR2=MotorList[M6].TimPeriod>>1;
		TIM8->ARR=MotorList[M6].TimPeriod-1; 
		PWM(M6);
	}
}

void TIM6_DAC_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET)
	{
		TIM_Cmd(TIM6, DISABLE);
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);	
//    //电阻触摸屏
//    if(g_tTP.Enable == 1)
//    {
//      TOUCH_Scan();	
//    }  
		TIM_Cmd(TIM6, ENABLE);
	}
}

//用作软定时器、计时器。在Timekeeper中实现了中断函数
//void TIM7_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
//	{	
//		TIM_ClearFlag(TIM7, TIM_FLAG_Update);
//	}
//}

//void RTC_IRQHandler(void)
//{
//	if (RTC_GetITStatus(RTC_IT_SEC) == SET)
//	{
//		/* Clear the RTC Second interrupt */
//		RTC_ClearITPendingBit(RTC_IT_SEC);
//		/* Wait until last write operation on RTC registers has finished */
//		RTC_WaitForLastTask();    
//		/* Reset RTC Counter when Time is 23:59:59 */
//		if (RTC_GetCounter() == 0x00015180)
//		{
//			RTC_SetCounter(0x0);
//			/* Wait until last write operation on RTC registers has finished */
//			RTC_WaitForLastTask();
//		}
//	}
//}


/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

//#include "main.h"

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	if(WorkData.DeviceIniBadTag)
	{
		WorkData.DevieIniFailNum++;
		WriteWorkData();
		SystemReset();
	}
    if(CoreDebug->DHCSR & 1)
    {
		//可以执行
//		*(__IO uint8_t *) (DeviceIniBadTagAddr)=1;
//		SystemReset();
		
//		if(!_isDeviceInitOk)
//		{
//			WorkData.DeviceIniBadTag=true;
//			WriteWorkData();
//			SystemReset();
//		}
       __breakpoint(0);
    }
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
        
    }
}
                                                                            
/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  //TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
