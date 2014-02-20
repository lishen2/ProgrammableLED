#include "stm32f10x.h"
#include "usb_regs.h"
#include "nusb_ops.h"
#include "nusb_intf.h"
#include "nusb_hw.h"
#include "nusb_desc.h"
#include "nusb_core.h"
#include "nusb_conf.h"
#include "nusb_utils.h"

static void _Init(void);
static void _Reset(void);

NUSB_DEVICE_OPS g_devOps = {
	_Init,
	_Reset
};

static void _Init(void)
{

	NUSB_UpdateSerialNum();

	NUSB_HwConfig();
	NUSB_PowerOn();

	return;
}

static void _Reset(void)
{	
	SetBTABLE(NUSB_BTABLE_ADDRESS);
	
	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_NAK);
	SetEPRxAddr(ENDP0, NUSB_ENDP0_RXADDR);
	SetEPRxCount(ENDP0, NUSB_ENDP0_BUFFER_SIZE);
	SetEPTxAddr(ENDP0, NUSB_ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxValid(ENDP0);
	
	/* Initialize Endpoint 1 */
	SetEPType(ENDP1, EP_BULK);
	SetEPTxAddr(ENDP1, NUSB_ENDP1_TXADDR);
	SetEPTxStatus(ENDP1, EP_TX_NAK);
	SetEPRxStatus(ENDP1, EP_RX_DIS);
	
	/* Initialize Endpoint 2 */
	SetEPType(ENDP2, EP_BULK);
	SetEPRxAddr(ENDP2, NUSB_ENDP2_RXADDR);
	SetEPRxCount(ENDP2, NUSB_ENDP2_BUFFER_SIZE);
	SetEPRxStatus(ENDP2, EP_RX_VALID);
	SetEPTxStatus(ENDP2, EP_TX_DIS);
	
	/* Set the device to response on default address */
	SetDeviceAddress(0);
	
	g_DevStatus = NUSB_ATTACHED;	

	return;
}




