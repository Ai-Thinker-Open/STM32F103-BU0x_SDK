#include "hal_usb.h"
#include "hw_config.h"

#include "usb_init.h"
#include "usb_mem.h"
#include "usb_conf.h"
#include "usb_regs.h"

void HalUsbInit (void)
{
    Set_System();

    Set_USBClock();

    USB_Interrupts_Config();

    USB_Init();
}

int HalUsbRead (uint8_t* buffter, uint32_t len)
{
    return USB_RxRead (buffter, len);
}

void HalUsbWrite (uint8_t* buffter, uint32_t len)
{
    USB_TxWrite (buffter, len);
}
