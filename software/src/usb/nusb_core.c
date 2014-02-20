#include "stm32f10x.h"
#include "nusb_desc.h"
#include "nusb_intf.h"
#include "nusb_hw.h"
#include "nusb_core.h"

NUSB_DEVICE_STATE g_DevStatus = NUSB_UNCONNECTED;

NUSB_DEVICE_STATE NUSB_GetStatus(void)
{
	return g_DevStatus;
}

void NUSB_init(void)
{
	g_devOps.Init();

	g_DevStatus = NUSB_UNCONNECTED;

	return;
}




