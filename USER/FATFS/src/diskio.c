/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */
#include "SST25V.h"
#include "usbh_usr.h"
#include "FileOperate.h"

#define  USB_SECTOR_SIZE		512
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	switch (pdrv)
	{
		case SPI_FLASH :
			stat = 0;
			break;   
		case FS_USB :
            stat= 0;
			break;
		default:
			break;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;

	switch (pdrv)
	{
		case FS_USB :		/* STM32 USB Host 口外接U盘 */
			if(HCD_IsDeviceConnected(&USB_OTG_Core))//返回U盘状态
			{
				stat &= ~STA_NOINIT;
			}
			break;
        case SPI_FLASH:
             if(FLASH_Init()==RES_OK)
                 stat=RES_OK;
             break;
		default :
			break;
	}
	return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{          
    DRESULT res;  
	switch (pdrv)
	{  		
		case SPI_FLASH :
            for(u16 i=0;i<count;i++)
            {
                ReadOneSector(sector,buff);
                sector ++;
                buff += FLASH_SECTOR_SIZE;
            }	
            return RES_OK;
		case FS_USB :
			{
				//读U盘
				//buff:读数据缓存区
				//sector:扇区地址
				//count:扇区个数	
				BYTE status = USBH_MSC_OK;
				if (HCD_IsDeviceConnected(&USB_OTG_Core))
				{
					do
					{
						status = USBH_MSC_Read10(&USB_OTG_Core, buff,sector,USB_SECTOR_SIZE * count);
						USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);
						if (!HCD_IsDeviceConnected(&USB_OTG_Core))
						{
							break;
						}
					}
					while (status == USBH_MSC_BUSY );
				}
				if (status == USBH_MSC_OK)
				{
					res = RES_OK;
				}
				else
				{
					res = RES_ERROR;
				}
			}
			break;
		default:
			res = 1;
			break;
	}
	return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{    	
    
    DRESULT res; 
	switch (pdrv)
	{
		case SPI_FLASH :
			for(u16 i=0;i<count;i++)
            {	
                WriteOneSector(sector,(u8*)buff);
                sector ++;
                buff += FLASH_SECTOR_SIZE;
            }            
            return RES_OK; 
		case FS_USB :
			{
				//写U盘
				//buff:写数据缓存区
				//sector:扇区地址
				//count:扇区个数	
				BYTE status = USBH_MSC_OK;
				if (HCD_IsDeviceConnected(&USB_OTG_Core))
				{
					do
					{
						status = USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)buff,sector, USB_SECTOR_SIZE * count);
						USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);
						if(!HCD_IsDeviceConnected(&USB_OTG_Core))
						{
							break;
						}
					}
					while(status == USBH_MSC_BUSY );
				}
				if (status == USBH_MSC_OK)
				{
					res = RES_OK;
				}
				else
				{
					res = RES_ERROR;
				}
			}
			break;
		default:
			res = 1;
			break;
	}
	return res;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{  
	switch (pdrv) 
    { 	
        case SPI_FLASH :
            {
                DRESULT res = RES_OK; 
                DWORD nFrom,nTo;
                int i; 	
                switch(cmd)
                {
                    case CTRL_SYNC :
                        break;		
                    //扇区擦除
                    case CTRL_ERASE_SECTOR:
                        nFrom = *((DWORD*)buff);
                        nTo = *(((DWORD*)buff)+1);
                        for(i = nFrom;i <= nTo;i ++)
                            SST25V_SectorErase_4KByte(i);	
                        break;		
                    case GET_BLOCK_SIZE:
                        *(DWORD*)buff = FLASH_BLOCK_SIZE;
                        break;		
                    case GET_SECTOR_SIZE:
                        *(DWORD*)buff = FLASH_SECTOR_SIZE;
                        break;
                    
                    case GET_SECTOR_COUNT:
                        *(DWORD*)buff = FLASH_SECTOR_COUNT;
                        break;
                        
                    default:
                        res = RES_PARERR;
                        break;
                }
                return res;
            }

            case FS_USB :
            {
                DRESULT res = RES_OK;
				switch (cmd)
				{
					case CTRL_SYNC :		/* Make sure that no pending write process */
						res = RES_OK;
						break;
					case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
						*(DWORD*)buff = (DWORD) USBH_MSC_Param.MSCapacity;
						res = RES_OK;
						break;
					case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
						*(WORD*)buff = USB_SECTOR_SIZE;
						res = RES_OK;
						break;
					case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */\
						*(DWORD*)buff = USB_SECTOR_SIZE;
						res = RES_OK;
						break;
					default:
						res = RES_PARERR;
						break;
				}		
                return res;
            }

        }
        return RES_PARERR;
}

//获得系统时间，用于改写文件的创建和修改时间。
DWORD get_fattime(void)
{
    DataTimeStruct* dateTime=GetCurrDataTime();
	return	  ((DWORD)(dateTime->Year - 1980) << 25)	/* Year = 2013 */
			| ((DWORD)dateTime->Mon << 21)				/* Month = 1 */
			| ((DWORD)dateTime->Day << 16)				/* Day_m = 1*/
			| ((DWORD)dateTime->Hour << 11)				/* Hour = 0 */
			| ((DWORD)dateTime->Min << 5)				/* Min = 0 */
			| ((DWORD)dateTime->Sec >> 1);				/* Sec = 0 */
}

