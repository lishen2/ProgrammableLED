#include <stdlib.h>
#include "stm32f10x.h"
#include "usb_regs.h"
#include "nusb_ops.h"
#include "nusb_intf.h"
#include "nusb_hw.h"
#include "nusb_desc.h"
#include "nusb_conf.h"

static void _Init(void);
static void _Reset(void);
static void NOP_Process(void);
static u32 _GetDeviceDescriptorLength(void);
static u8* _GetDeviceDescriptor(void);
static u32 _GetConfigDescriptorLength(void);
static u8* _GetConfigDescriptor(void);
static u32 _GetStringDescriptorLength(u8 index);
static u8* _GetStringescriptor(u8 index);

NUSB_DEVICE_OPS g_devOps = 
{
	_Init,
	_Reset,
	_GetDeviceDescriptorLength,
	_GetDeviceDescriptor,
	_GetConfigDescriptorLength,
	_GetConfigDescriptor,
	_GetStringDescriptorLength,
	_GetStringescriptor,
	{
	    NOP_Process,   /* EP1_IN_Callback */
	    NOP_Process,   /* EP2_IN_Callback */
	    NOP_Process,   /* EP3_IN_Callback */
	    NOP_Process,   /* EP4_IN_Callback */
	    NOP_Process,   /* EP5_IN_Callback */
	    NOP_Process,   /* EP6_IN_Callback */
	    NOP_Process,   /* EP7_IN_Callback */
  	},
	{
		NOP_Process,	/* EP1_IN_Callback */
		NOP_Process,	/* EP2_IN_Callback */
		NOP_Process,	/* EP3_IN_Callback */
		NOP_Process,	/* EP4_IN_Callback */
		NOP_Process,	/* EP5_IN_Callback */
		NOP_Process,	/* EP6_IN_Callback */
		NOP_Process,	/* EP7_IN_Callback */
	},
};

NUSB_DEVICE_CONFIGURATION g_devConf = 
{
	NUSB_MAX_ENDP_NUM,
    NUSB_IMR_MSK,
    NUSB_ENDP0_BUFFER_SIZE,
};

static void _Init(void)
{

	NUSB_UpdateSerialNum();
	NUSB_HwConfig();

	return;
}

static void _Reset(void)
{	
	SetBTABLE(NUSB_BTABLE_ADDRESS);
	
	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_NAK);
	SetEPTxAddr(ENDP0, NUSB_ENDP0_TXADDR);
	SetEPRxAddr(ENDP0, NUSB_ENDP0_RXADDR);
	SetEPRxCount(ENDP0, NUSB_ENDP0_BUFFER_SIZE);
	Clear_Status_Out(ENDP0);
	SetEPRxStatus(ENDP0, EP_RX_VALID);
	
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

	return;
}

static u32 _GetDeviceDescriptorLength(void)
{
    return sizeof(MASS_DeviceDescriptor);
}

static u8* _GetDeviceDescriptor(void)
{
    return MASS_DeviceDescriptor;
}

static u32 _GetConfigDescriptorLength(void)
{
    return sizeof(MASS_ConfigDescriptor);
}

static u8* _GetConfigDescriptor(void)
{
    return MASS_ConfigDescriptor;
}

static u32 _GetStringDescriptorLength(u8 index)
{
    index -= 1;
    if (index < sizeof(MASS_StringArray))
    {
        return MASS_StringArray[index][0];
    }
    else 
    {
        return 0;
    }
}

static u8* _GetStringescriptor(u8 index)
{
    index -= 1;
    if (index < sizeof(MASS_StringArray))
    {
        return MASS_StringArray[index];
    }
    else 
    {
        return NULL;
    }
}

static void NOP_Process(void)
{}




