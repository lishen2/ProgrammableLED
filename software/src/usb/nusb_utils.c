#include "stm32f10x.h"
#include "nusb_conf.h"
#include "nusb_utils.h"
#include "usb_regs.h"

void SetDeviceAddress(u8 Val)
{
	u32 i;
	
	/* set address in every used endpoint */
	for (i = 0; i < NUSB_MAX_ENDP_NUM; i++)
	{
		_SetEPAddress((u8)i, (u8)i);
	} /* for */
	_SetDADDR(Val | DADDR_EF); /* set device address and enable function */ 

	return;
}