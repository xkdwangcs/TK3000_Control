/*
*********************************************************************************************************
*
*	模块名称 : USB中断服务程序
*	文件名称 : usb_it.c
*	版    本 : V1.0
*	说    明 : 本文件存放USB中断服务程序。只需将该文件加入工程即可，无需再到 stm32f4xx_it.c 中添加这些ISR程序
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-02-01 armfly  正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "usb_core.h"
#include "usbd_core.h"
#include "usb_conf.h"
#include "usb_hcd_int.h"
#include "usb_bsp.h"

extern USB_OTG_CORE_HANDLE  USB_OTG_Core;

/* Private function prototypes -----------------------------------------------*/
//extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
	extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
	extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif

//USB OTG 中断服务函数
//处理所有USB中断
//void OTG_FS_IRQHandler(void)
//{
//	USBD_OTG_ISR_Handler (&USB_OTG_dev);
//}

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
	/**
	  * @brief  This function handles EP1_IN Handler.
	  * @param  None
	  * @retval None
	  */
	void OTG_HS_EP1_IN_IRQHandler(void)
	{
		USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_dev);
	}

	/**
	  * @brief  This function handles EP1_OUT Handler.
	  * @param  None
	  * @retval None
	  */
	void OTG_HS_EP1_OUT_IRQHandler(void)
	{
		USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_dev);
	}
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
