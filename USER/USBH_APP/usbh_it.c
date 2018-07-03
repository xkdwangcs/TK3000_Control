#include "usb_bsp.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"
#include "usbh_usr.h"
 

void OTG_FS_IRQHandler(void)
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

