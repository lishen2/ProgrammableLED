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
#include "nusb_assert.h"

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

void NUSB_init(void)
{
	g_devOps.Init();

    NUSB_PowerOn();

	return;
}

void NUSB_EP0_InProcess(void)
{	
    switch(g_SetupStatus.ControlState)
    {
        /* read ack from host, for now no more data to send.
           so stall the tx direction */
        case NUSB_STAT_SEND_DESCRIPTOR_WAIT_ACK:
        {
            SetEP0TxStatusLater(EP_TX_STALL);
            break;
        }
		case NUSB_STAT_SET_ADDRESS_WAIT_ACK:
		{
			_SetDADDR(g_Request.USBwValue0 | DADDR_EF);
			printf(" Do SETADDR\r\n");
			break;
		}	
    }

    return;
}

void NUSB_EP0_OutProcess(void)
{
	return;
}

/* for now all sending data are smaller then EP0 TX buffer, so we allways send data in one go */
void NUSB_EP0SendData(u8* buf, u32 length)
{
	if (0 == length)
	{
		SetEPTxCount(ENDP0, 0);
	    SetEP0TxStatusLater(EP_TX_VALID);	
	}
	else 
	{
		NUSB_ASSERT(length <= g_devConf.EP0BufferSize);	
	
	    UserToPMABufferCopy(buf, GetEPTxAddr(ENDP0), length);
	    SetEPTxCount(ENDP0, length);
	    SetEP0TxStatusLater(EP_TX_VALID);	
	}

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

static NUSB_RESULT _req_getStatus(NUSB_REQUEST* request)
{
    NUSB_RESULT resault = NUSB_SUCCESS;
	u16 feature;
	u8 EPNum;

   /* GET STATUS for Device*/
    if (DEVICE_RECIPIENT == (request->USBbmRequestType & RECIPIENT)
        && (request->USBwIndex == 0))
    {
		feature = g_devConf.DeviceFeature;
    }
    /* GET STATUS for Interface*/
    else if (INTERFACE_RECIPIENT == (request->USBbmRequestType & RECIPIENT))
    {
		feature = 0;	
    }
    /* GET STATUS for EndPoint*/
    else if (ENDPOINT_RECIPIENT == (request->USBbmRequestType & RECIPIENT))
    {
		EPNum = request->USBwIndex0 & 0x0f;
		
		feature = 0;
		if (0 != request->USBwIndex0 & 0x80)
		{
			if (_GetTxStallStatus(EPNum)){
				feature = 0x01;	
			}
		}
		else
		{
			if (_GetRxStallStatus(EPNum)){
				feature = 0x01;	
			}
		}
	
    } else {
		resault = NUSB_ERROR;
	}

	if (NUSB_SUCCESS == resault)
	{
		NUSB_EP0SendData((u8*)&feature, sizeof(feature));
	}
	
	return resault;	
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
		printf("Dev\r\n");
    }
    else if (request->USBwValue1 == CONFIG_DESCRIPTOR)
    {
        length = g_devOps.GetConfigDescriptorLength();
        descBuf = g_devOps.GetConfigDescriptor();
		printf("Conf\r\n");
    }
    else if (request->USBwValue1 == STRING_DESCRIPTOR)
    {
        length = g_devOps.GetStringDescriptorLength(request->USBwValue0);
        descBuf = g_devOps.GetStringDescriptor(request->USBwValue0);
		printf("Str[%hhu]\r\n", request->USBwValue0);
    } else 
	{
		printf("UNKNOWN\r\n");
    	length = 0;
    	descBuf = NULL;		
	}

	if (NULL != descBuf && 0 != length){

		if (length > request->USBwLength){
			length = request->USBwLength;
		}

		NUSB_EP0SendData(descBuf, length);
		resault = NUSB_SUCCESS;
        g_SetupStatus.ControlState = NUSB_STAT_SEND_DESCRIPTOR_WAIT_ACK;
        printf(" SendDesc[%u]\r\n", length);
	}
	else
	{
		resault = NUSB_ERROR;
	}
    
    return resault;
}

static NUSB_RESULT _req_setAddress(NUSB_REQUEST* request)
{
	NUSB_RESULT resault;

	if ((request->USBwValue0 > 127) || (request->USBwValue1 != 0) || (request->USBwIndex != 0))
	/* Device Address should be 127 or less*/
	{
		g_SetupStatus.ControlState = NUSB_STALLED;
		resault = NUSB_ERROR;
		printf("FAILED\r\n");
	}
	// address saved in global variables, set address when recive ack from host 
	else
	{
		g_SetupStatus.ControlState = NUSB_STAT_SET_ADDRESS_WAIT_ACK;
		NUSB_EP0SendData(NULL, 0);
		resault = NUSB_SUCCESS;
		printf("SUCCESS\r\n");
	}
	
    return resault;
}

static NUSB_RESULT _req_setConfiguration(NUSB_REQUEST* request)
{
	if ((request->USBwValue0 <= g_devConf.TotalConfiguration) 
		&& (request->USBwValue1 == 0)
		&& (request->USBwIndex == 0)) /*call Back usb spec 2.0*/
	{
		g_SetupStatus.CurrentConfiguration = request->USBwValue0;
		g_devOps.SetConfiguration();

		NUSB_EP0SendData(NULL, 0);

		return NUSB_SUCCESS;
	}
	else
	{
		return NUSB_UNSUPPORT;
	}
}

static NUSB_RESULT _standardRequest(NUSB_REQUEST* request)
{
    NUSB_RESULT resault = NUSB_ERROR;
    
    switch (request->USBbRequest)
    {
        case GET_STATUS:
        {
			printf("GET_STATUS\r\n");
			resault = _req_getStatus(request);
            break;
        }
        case CLEAR_FEATURE:
        {
			printf("CLEAR_FEATURE\r\n");
            break;
        }
        case SET_FEATURE:
        {
			printf("SET_FEATURE\r\n");
            break;
        }
        case SET_ADDRESS:
        {
			printf("SET_ADDRESS-");
            if (DEVICE_RECIPIENT == (request->USBbmRequestType & RECIPIENT))
            {
                resault = _req_setAddress(request);    
            }            
            break;
        }
        case GET_DESCRIPTOR:
        {
			printf("GET_DESC-");
            if (DEVICE_RECIPIENT == (request->USBbmRequestType & RECIPIENT))
            {
                resault = _req_getDescripter(request);    
            }
            break;
        }
        case SET_DESCRIPTOR:
        {
			printf("SET_DESC\r\n");
            break;
        }            
        case GET_CONFIGURATION:
        {
			printf("GET_CONF\r\n");
            break;
        }            
        case SET_CONFIGURATION:
        {
			printf("SET_CONF\r\n");
			if (DEVICE_RECIPIENT == (request->USBbmRequestType & RECIPIENT))
            {
                resault = _req_setConfiguration(request);    
            }
            break;
        }            
        case GET_INTERFACE:
        {
			printf("GET_INTF\r\n");
            break;
        }            
        case SET_INTERFACE:
        {
			printf("SET_INTF\r\n");
            break;
        }            
        default:
        {
			printf("UN_REQ\r\n");
            resault = NUSB_UNSUPPORT;
            break;
        }        
    } //switch

    return resault;
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
			printf(" ST:");
            resault = _standardRequest(&g_Request);
            break;
        }
        case CLASS_REQUEST:
        {
			printf(" CLASS:");
            resault = g_devOps.ClassSetup(&g_Request);
            break;
        }
        case VENDOR_REQUEST:
        default:
        {
			printf(" UNKONWN\r\n");
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

