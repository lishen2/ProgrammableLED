#include "stm32f10x.h"
#include "usb_regs.h"
#include "nusb_intf.h"
#include "nusb_utils.h"

void NUSB_SetDeviceAddress(u8 Val)
{
	u32 i;
	
	/* set address in every used endpoint */
	for (i = 0; i < g_devConf.MaxEPNum; i++)
	{
		_SetEPAddress((u8)i, (u8)i);
	} /* for */
	_SetDADDR(Val | DADDR_EF); /* set device address and enable function */ 

	return;
}


