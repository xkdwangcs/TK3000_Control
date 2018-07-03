#include "usb_bsp.h"
#include "BLL/AppTasks.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbd_core.h"
#include "UI/MainForm.h"

USBModeEnum _currUSBMode=HostMode;

/* Private function prototypes -----------------------------------------------*/
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12;   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG1_FS) ; 
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG1_FS) ;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ; 
}

void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
  NVIC_InitTypeDef NVIC_InitStructure;   
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
}

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{
	
}

void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{
	
}

//USBHost最后的状态
u8 _usbOTGState=0;
void OTG_FS_IRQHandler(void)
{
	if(_currUSBMode==HostMode)
	{
	   USBH_OTG_ISR_Handler(&USB_OTG_Core);
	   u8 currState= USB_OTG_Core.host.ConnSts;
	   if(currState==_usbOTGState)
		   return;
	   _usbOTGState=currState;
	   if(currState==1)  //检测到插入了U盘
	   {
		   SetUSBHostTaskEnable(true);
	   }
	   else  //拨出了U盘
	   {
		   //SetUSBHostTaskEnable(false);//不能在此关闭，否则不能多次连
		   if(_currPermission<AdminiPer)
			   SystemReset();
		   ShowMainForm(0);       
	   }
	}
	else if(_currUSBMode==DirverMode)
	{
		USBD_OTG_ISR_Handler (&USB_OTG_dev);
	}
}

void USB_OTG_BSP_uDelay (const uint32_t usec)
{  
    Delay_ms_OS(usec/1000);
}   

void USB_OTG_BSP_mDelay (const uint32_t msec)
{ 
    Delay_ms_OS(msec);  
}  
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
