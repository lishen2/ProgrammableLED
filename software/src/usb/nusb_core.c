#include <stdlib.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "usb_def.h"
#include "usb_regs.h"
#include "usb_mem.h"
#include "nusb_intf.h"
#include "nusb_utils.h"
#include "nusb_core.h"
#include "nusb_pwr.h"

#define Usb_rLength Usb_wLength
#define Usb_rOffset Usb_wOffset

#define USBwValue USBwValues.w
#define USBwValue0 USBwValues.bw.bb0
#define USBwValue1 USBwValues.bw.bb1
#define USBwIndex USBwIndexs.w
#define USBwIndex0 USBwIndexs.bw.bb0
#define USBwIndex1 USBwIndexs.bw.bb1
#define USBwLength USBwLengths.w
#define USBwLength0 USBwLengths.bw.bb0
#define USBwLength1 USBwLengths.bw.bb1

//current request
static NUSB_REQUEST g_Request;

//setup status, used only in setup procedure
NUSB_SETUP_STATUS     g_SetupStatus;

/* Saved status of Rx and Tx of EP0 */
vu16 g_SaveRxStatus;
vu16 g_SaveTxStatus;

#define SetEP0TxStatusLater(ST) g_SaveTxStatus = ST
#define SetEP0RxStatusLater(ST) g_SaveRxStatus = ST

//device status
//NUSB_DEVICE_STATE g_DevStatus = NUSB_UNCONNECTED;
/*
NUSB_DEVICE_STATE NUSB_GetStatus(void)
{
	return g_DevStatus;
}
*/
void NUSB_init(void)
{
	g_devOps.Init();

    NUSB_PowerOn();
    
//	g_DevStatus = NUSB_UNCONNECTED;
	return;
}

void NUSB_EP0_InProcess(void)
{
    switch(g_SetupStatus.ControlState)
    {
        /* read ack from host, for now no more data to send.
           so stall the tx direction */
        case NUSB_STAT_SEND_DESCRIPTOR_WAIT_STATUS_IN:
        {
            SetEP0TxStatusLater(EP_TX_STALL);
            break;
        }
    }

    return;
}

void NUSB_EP0_OutProcess(void)
{
    printf("EP0 Out\r\n");
}

void _sendData(u8* buf, u32 length)
{
    UserToPMABufferCopy(buf, GetEPTxAddr(ENDP0), length);
    SetEPTxCount(ENDP0, length);
    SetEPTxStatus(ENDP0, EP_TX_VALID);
    SetEP0TxStatusLater(EP_TX_VALID);
    return;
}

static void _parserSetupMsg(NUSB_REQUEST* request)
{
    union
    {
        uint8_t* b;
        uint16_t* w;
    } pBuf;
    uint16_t offset = 1;

    pBuf.b = PMAAddr + (u8 *)(_GetEPRxAddr(ENDP0) * 2); /* *2 for 32 bits addr */

    request->USBbmRequestType = *pBuf.b++; /* bmRequestType */
    request->USBbRequest = *pBuf.b++; /* bRequest */
    pBuf.w += offset;  /* word not accessed because of 32 bits addressing */
    request->USBwValue = ByteSwap(*pBuf.w++); /* wValue */
    pBuf.w += offset;  /* word not accessed because of 32 bits addressing */
    request->USBwIndex  = ByteSwap(*pBuf.w++); /* wIndex */
    pBuf.w += offset;  /* word not accessed because of 32 bits addressing */
    request->USBwLength = *pBuf.w; /* wLength */

    return;
}

static NUSB_RESULT _req_getDescripter(NUSB_REQUEST* request)
{
    NUSB_RESULT resault;
    u32 length;
    u8* descBuf;

    if (request->USBwValue1 == DEVICE_DESCRIPTOR)
    {
        length = g_devOps.GetDeviceDescriptorLength();
        descBuf = g_devOps.GetDeviceDescriptor();
		printf("Get device descript request\r\n");
    }
    else if (request->USBwValue1 == CONFIG_DESCRIPTOR)
    {
        length = g_devOps.GetConfigDescriptorLength();
        descBuf = g_devOps.GetConfigDescriptor();
		printf("Get configuration descript request\r\n");
    }
    else if (request->USBwValue1 == STRING_DESCRIPTOR)
    {
        length = g_devOps.GetStringDescriptorLength(request->USBwValue0);
        descBuf = g_devOps.GetStringDescriptor(request->USBwValue0);
		printf("Get string descript request\r\n");
    } else 
	{
    	length = 0;
    	descBuf = NULL;		
	}

	if (NULL != descBuf && 0 != length){
		_sendData(descBuf, length);
		resault = NUSB_SUCCESS;
        g_SetupStatus.ControlState = NUSB_STAT_SEND_DESCRIPTOR_WAIT_STATUS_IN;
        printf("Send descriptor [%u].\r\n", length);
	}
	else
	{
		resault = NUSB_ERROR;
	}
    
    return resault;
}

static NUSB_RESULT _req_setAddress(NUSB_REQUEST* request)
{
    
    printf("Set Address.\r\n");
    return NUSB_SUCCESS;
}

static NUSB_RESULT _standardRequest(NUSB_REQUEST* request)
{
    NUSB_RESULT resault = NUSB_ERROR;
    
    switch (request->USBbRequest)
    {
        case GET_STATUS:
        {
            break;
        }
        case CLEAR_FEATURE:
        {
            break;
        }
        case SET_FEATURE:
        {
            break;
        }
        case SET_ADDRESS:
        {
            if (DEVICE_RECIPIENT == (request->USBbmRequestType & RECIPIENT))
            {
                resault = _req_setAddress(request);    
            }            
            break;
        }
        case GET_DESCRIPTOR:
        {
            if (DEVICE_RECIPIENT == (request->USBbmRequestType & RECIPIENT))
            {
                resault = _req_getDescripter(request);    
            }
            break;
        }
        case SET_DESCRIPTOR:
        {
            break;
        }            
        case GET_CONFIGURATION:
        {
            break;
        }            
        case SET_CONFIGURATION:
        {
            break;
        }            
        case GET_INTERFACE:
        {
            break;
        }            
        case SET_INTERFACE:
        {
            break;
        }            
        default:
        {
            resault = NUSB_UNSUPPORT;
            break;
        }        
    } //switch

    return resault;
}

static NUSB_RESULT _classRequest(void)
{
	return NUSB_SUCCESS;
}

void NUSB_EP0_SetupProcess(void)
{
    u8 bmRequestType;
    NUSB_RESULT resault;

    _parserSetupMsg(&g_Request);

    bmRequestType = g_Request.USBbmRequestType & REQUEST_TYPE;
    switch(bmRequestType)
    {
        case STANDARD_REQUEST:
        {
            resault = _standardRequest(&g_Request);
            break;
        }
        case CLASS_REQUEST:
        {
            resault = _classRequest();
            break;
        }
        case VENDOR_REQUEST:
        default:
        {
            resault = NUSB_UNSUPPORT;
            break;
        }
    }//switch

    //if somethine gose wrong
    if (NUSB_SUCCESS != resault)
    {   
        SetEP0RxStatusLater(EP_RX_STALL);
        SetEP0TxStatusLater(EP_TX_STALL);
    } 
    else 
    {
        SetEP0RxStatusLater(EP_RX_VALID);
    }

    return;
}

