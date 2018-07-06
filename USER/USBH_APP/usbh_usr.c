/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "FileOperate.h"
#include "USBFunc.h"
#include "AppTasks.h"

USB_OTG_CORE_HANDLE   USB_OTG_Core;
USBH_HOST             USB_Host;
bool _isInset=false;  //U盘是否已经插入

//USB回调函数表，顺序执行
USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,							//初始化
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,					//检测到有USB设备插入
  USBH_USR_ResetDevice,						//重置设备
  USBH_USR_DeviceDisconnected,				//检测到有设备移除
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,				//检测设备速度，例如高速、全速、低速
  USBH_USR_Device_DescAvailable,			//设备信息
  USBH_USR_DeviceAddressAssigned,	   		//分配地址成功
  USBH_USR_Configuration_DescAvailable,	    //配置描述
  USBH_USR_Manufacturer_String,		  		//厂商信息（字符串）
  USBH_USR_Product_String,			  		//产品信息（字符串）
  USBH_USR_SerialNum_String,		   		//设备串号
  USBH_USR_EnumerationDone,			  		//枚举完成
  USBH_USR_UserInput,						//用户操作应用程序状态条目
  USBH_USR_MSC_Application,					//USB应用操作
  USBH_USR_DeviceNotSupported,				//设备不支持
  USBH_USR_UnrecoveredError 
};

//可以初始化一些东西
void USBH_USR_Init(void)
{

}

//检测到U盘插入
void USBH_USR_DeviceAttached(void)
{
	//printf("检测到USB设备插入!\r\n");
}

//无法恢复的错误!!
void USBH_USR_UnrecoveredError (void)
{
	//printf("无法恢复的错误!!!\r\n\r\n");
}

//检测到U盘拔出
void USBH_USR_DeviceDisconnected (void)
{
	//printf("USB设备拔出!\r\n");
	SetUSBHostTaskEnable(false);//在此可以关闭USBHost任务，节省任务资源
	_isInset=0;
}

//重置驱动
void USBH_USR_ResetDevice(void)
{
	/* callback for USB-Reset */
	//printf("USBH_USR_ResetDevice\r\n");
}

//检测到从机速度
//DeviceSpeed:从机速度(0,1,2 / 其他)
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	/*if(DeviceSpeed==HPRT0_PRTSPD_HIGH_SPEED)
	{
		printf("高速(HS)USB设备!\r\n");
 	}  
	else if(DeviceSpeed==HPRT0_PRTSPD_FULL_SPEED)
	{
		printf("全速(FS)USB设备!\r\n"); 
	}
	else if(DeviceSpeed==HPRT0_PRTSPD_LOW_SPEED)
	{
		printf("低速(LS)USB设备!\r\n");  
	}
	else
	{
		printf("设备错误!\r\n");  
	}  */
}

//检测到从机的描述符
//DeviceDesc:设备描述符指针
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
//	USBH_DevDesc_TypeDef *hs;
//	hs = DeviceDesc;
//	usb_printf("> VID : %04Xh\r\n" , (uint32_t)(*hs).idVendor);
//	usb_printf("> PID : %04Xh\r\n" , (uint32_t)(*hs).idProduct);
}

//从机地址分配成功
void USBH_USR_DeviceAddressAssigned(void)
{
  //printf("从机地址分配成功\r\n");
}


//配置描述符获有效
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	/*USBH_InterfaceDesc_TypeDef *id; 
	id = itfDesc;   
	if((*id).bInterfaceClass==0x08)
	{
		//printf("可移动存储器设备!\r\n"); 
	}
    else if((*id).bInterfaceClass==0x03)
	{
		//printf("HID 设备!\r\n"); 
	}*/    
}

//厂商（字符串）信息
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  //printf("Manufacturer : %s\r\n", (char *)ManufacturerString);
}

//产品信息（字符串）
void USBH_USR_Product_String(void *ProductString)
{
	//printf("Product : %s\r\n", (char *)ProductString);
}

//设备串号
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	//printf( "Serial Number : %s\r\n", (char *)SerialNumString); 
}

//设备USB枚举完成
void USBH_USR_EnumerationDone(void)
{
//	printf("设备USB枚举完成\r\n");	
//	USB_OTG_BSP_mDelay(500);
	//USBH_USR_MSC_Application();
}

//无法识别的USB设备
void USBH_USR_DeviceNotSupported(void)
{
	//printf("无法识别的USB设备\r\n");
}

//用户操作应用程序状态条目
USBH_USR_Status USBH_USR_UserInput(void)
{
	return USBH_USR_RESP_OK;
}

//USB接口电流过载
void USBH_USR_OverCurrentDetected (void)
{
	//printf("端口电流过大!!!\r\n");
}

void USBBackReset(u16 keyCode)
{
	SystemReset();
}

//应用操作
int USBH_USR_MSC_Application(void)
{
    if(!_isInset)
    {
      _isInset=true;
			PermissionTypeEnum destPer=AdminiPer;//需要管理员及以上权限
//		if(_currPermission<destPer)
//		{
//			ShowLoginForm(destPer,ShowUSBOprateForm,USBBackReset);
//			return 0;
//		}
      DeviceStatusSRQ(USBInsert,"检测到U盘已插入设备");
    }
	return 0;
}

//用户要求重新初始化设备
void USBH_USR_DeInit(void)
{
	//USBH_USR_ApplicationState = USH_USR_FS_INIT;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

