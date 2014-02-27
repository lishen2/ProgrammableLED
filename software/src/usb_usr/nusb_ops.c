#include <stdlib.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "usb_regs.h"
#include "usb_def.h"
#include "nusb_ops.h"
#include "nusb_intf.h"
#include "nusb_hw.h"
#include "nusb_desc.h"
#include "nusb_conf.h"
#include "nusb_scsi.h"

static u8 g_MaxLun = NUSB_MAX_LUN;

#define NUSB_CLASS_GET_MAX_LUN                0xFE
#define NUSB_CLASS_MASS_STORAGE_RESET         0xFF

static void _Init(void);
static void _Reset(void);
static void NOP_Process(void);
static u32 _GetDeviceDescriptorLength(void);
static u8* _GetDeviceDescriptor(void);
static u32 _GetConfigDescriptorLength(void);
static u8* _GetConfigDescriptor(void);
static u32 _GetStringDescriptorLength(u8 index);
static u8* _GetStringescriptor(u8 index);
static void _SetConfiguration(void);
static NUSB_RESULT _classSetup(NUSB_REQUEST* request);

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
	_SetConfiguration,
	_classSetup,
	{
	    NUSB_OnSendFinish,   /* EP1_IN_Callback */
	    NOP_Process,   /* EP2_IN_Callback */
	    NOP_Process,   /* EP3_IN_Callback */
	    NOP_Process,   /* EP4_IN_Callback */
	    NOP_Process,   /* EP5_IN_Callback */
	    NOP_Process,   /* EP6_IN_Callback */
	    NOP_Process,   /* EP7_IN_Callback */
  	},
	{
		NOP_Process,	/* EP1_OUT_Callback */
		NUSB_OnReceive,	/* EP2_OUT_Callback */
		NOP_Process,	/* EP3_OUT_Callback */
		NOP_Process,	/* EP4_OUT_Callback */
		NOP_Process,	/* EP5_OUT_Callback */
		NOP_Process,	/* EP6_OUT_Callback */
		NOP_Process,	/* EP7_OUT_Callback */
	},
};

NUSB_DEVICE_CONFIGURATION g_devConf = 
{
    NUSB_IMR_MSK,
    NUSB_ENDP0_BUFFER_SIZE,
	NUSB_TOTAL_CONFIGURATION,
	NUSB_DEVICE_FEATURE_NO_FEATURE
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
	Clear_Status_Out(ENDP0);
	
	SetEPTxAddr(ENDP0, NUSB_ENDP0_TXADDR);
	SetEPTxCount(ENDP0, 0);
	SetEPRxAddr(ENDP0, NUSB_ENDP0_RXADDR);
	SetEPRxCount(ENDP0, NUSB_ENDP0_BUFFER_SIZE);

	SetEPTxStatus(ENDP0, EP_TX_NAK);
	SetEPRxStatus(ENDP0, EP_RX_VALID);

	_SetEPAddress(ENDP0, ENDP0);	

	/* Initialize Endpoint 1 */
	SetEPType(ENDP1, EP_BULK);

	SetEPTxAddr(ENDP1, NUSB_ENDP1_TXADDR);
	SetEPTxStatus(ENDP1, 0);

	SetEPTxStatus(ENDP1, EP_TX_NAK);
	SetEPRxStatus(ENDP1, EP_RX_DIS);

	_SetEPAddress(ENDP1, ENDP1);
	
	/* Initialize Endpoint 2 */
	SetEPType(ENDP2, EP_BULK);

	SetEPRxAddr(ENDP2, NUSB_ENDP2_RXADDR);
	SetEPRxCount(ENDP2, NUSB_ENDP2_BUFFER_SIZE);
    
	SetEPTxStatus(ENDP2, EP_TX_DIS);
	SetEPRxStatus(ENDP2, EP_RX_VALID);

	_SetEPAddress(ENDP2, ENDP2);

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
    if (index < sizeof(MASS_StringArray))
    {
        return MASS_StringArray[index];
    }
    else 
    {
        return NULL;
    }
}

static void _SetConfiguration(void)
{
	ClearDTOG_TX(ENDP1);
    ClearDTOG_RX(ENDP2);
	SetEPRxStatus(ENDP2, EP_RX_VALID);

	NUSB_ResetStatus();

	return;
}

static NUSB_RESULT _classSetup(NUSB_REQUEST* request)
{
	NUSB_RESULT resault = NUSB_ERROR;

	switch(request->USBbRequest)
	{
		case NUSB_CLASS_GET_MAX_LUN:
		{
			if ((INTERFACE_RECIPIENT == (request->USBbmRequestType & RECIPIENT))
				&& (request->USBwValues.w == 0)
      			&& (request->USBwIndexs.w == 0) 
				&& (request->USBwLengths.w == 0x01))
			{
				printf("GET_MAX_LUN\r\n");
				NUSB_EP0SendData(&g_MaxLun, sizeof(g_MaxLun));
				resault	= NUSB_SUCCESS;
			}

			break;
		}
		case NUSB_CLASS_MASS_STORAGE_RESET:
		{
			if ((INTERFACE_RECIPIENT == (request->USBbmRequestType & RECIPIENT))
				&& (request->USBwValues.w == 0)
      			&& (request->USBwIndexs.w == 0) 
				&& (request->USBwLengths.w == 0x00))
			{
				printf("MS_RESET\r\n");
				ClearDTOG_TX(ENDP1);
    			ClearDTOG_RX(ENDP2);
				NUSB_ResetStatus();
				SetEPRxStatus(ENDP2, EP_RX_VALID);
				NUSB_EP0SendData(NULL, 0);
				resault	= NUSB_SUCCESS;
			}
			
			break;		
		}
		default:
		{
			resault = NUSB_UNSUPPORT;
			break;
		}	
	}

	return resault;
}

static void NOP_Process(void)
{}




