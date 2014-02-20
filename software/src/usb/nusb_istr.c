#include <stdio.h>
#include "stm32f10x.h"
#include "nusb_intf.h"
#include "usb_regs.h"
#include "nusb_conf.h"

vu16 g_wIstr;

void NUSB_LP_INT(void)
{
	g_wIstr = _GetISTR();

	#if (NUSB_IMR_MSK & ISTR_CTR)
	if (g_wIstr & ISTR_CTR)
	{
		_SetISTR((uint16_t)CLR_CTR);
		printf("CTR occur\r\n");
	}
	#endif  
	#if (NUSB_IMR_MSK & ISTR_RESET)
	if (g_wIstr & ISTR_RESET)
	{
		_SetISTR((uint16_t)CLR_RESET);
		g_devOps.Reset();
		printf("RESET occur\r\n");
	}
	#endif
	#if (NUSB_IMR_MSK & ISTR_WKUP)
	if (g_wIstr & ISTR_WKUP)
	{
		_SetISTR((uint16_t)CLR_WKUP);
		//Resume(RESUME_EXTERNAL);
		printf("WAKEUP occur\r\n");
	}
	#endif
	#if (NUSB_IMR_MSK & ISTR_SUSP)
	if (g_wIstr & ISTR_SUSP)
	{
		//TODO, why first suspend then clear SUSP flag
		//Suspend();
		_SetISTR((uint16_t)CLR_SUSP);
		printf("SUSPEND occur\r\n");
	}
	#endif

	return;
}


