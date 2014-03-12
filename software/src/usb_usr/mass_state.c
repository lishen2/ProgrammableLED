#include <stdio.h>
#include "stm32f10x.h"
#include "usb_regs.h"
#include "nusb_trans.h"
#include "nusb_intf.h"
#include "mass_defs.h"
#include "mass_state.h"
#include "mass_conf.h"
#include "mass_scsi.h"

static u8 g_Status;
static MASS_Bulk_Only_CBW g_CBW;
static MASS_Bulk_Only_CSW g_CSW;

static void _goToErrorStatus(void)
{
    printf("Into error status.\r\n");

	g_Status = MASS_SCSI_ERROR;

	SetEPTxStatus(MASS_SEND_ENDPOINT,    EP_TX_STALL);
	SetEPRxStatus(MASS_RECEIVE_ENDPOINT, EP_RX_STALL);		

	return;	
}

static int _handleCBW(u32 length)
{
	if (SCSI_CBW_PACKET_LENGTH != length ||
		SCSI_CBW_SIGNATURE != g_CBW.dSignature ||
		g_CBW.bLUN > MASS_MAX_LUN ||
		g_CBW.bCBLength < 1 ||
		g_CBW.bCBLength > 16){

		_goToErrorStatus();

		return NUSB_ERROR;
	}

	g_CSW.dTag = g_CBW.dTag;
	g_CSW.dSignature = SCSI_CSW_SIGNATURE;
	g_CSW.dDataResidue = 0;

	switch (g_CBW.CB[0])
	{
		case SCSI_REQUEST_SENSE:
			printf("REQUEST_SENSE\r\n");
			MASS_CMD_RequestSense(&g_CBW, &g_CSW);
			break;
		case SCSI_INQUIRY:
			printf("SCSI_INQUIRY\r\n");
			MASS_CMD_Inquiry_Cmd(&g_CBW, &g_CSW);
			break;
		case SCSI_START_STOP_UNIT:
			printf("START_STOP_UNIT\r\n");
			//SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
			break;
		case SCSI_ALLOW_MEDIUM_REMOVAL:
			printf("ALLOW_MEDIUM_REMOVAL\r\n");
			//SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
			break;
		case SCSI_MODE_SENSE6:
			printf("MODE_SENSE6\r\n");
			//SCSI_ModeSense6_Cmd (CBW.bLUN);
			break;
		case SCSI_MODE_SENSE10:
			printf("MODE_SENSE10\r\n");
			//SCSI_ModeSense10_Cmd (CBW.bLUN);
			break;
		case SCSI_READ_FORMAT_CAPACITIES:
			printf("READ_FORMAT_CAPACITIES\r\n");
			//SCSI_ReadFormatCapacity_Cmd(CBW.bLUN);
			break;
		case SCSI_READ_CAPACITY10:
			printf("READ_CAPACITY10\r\n");
			//SCSI_ReadCapacity10_Cmd(CBW.bLUN);
			break;
		case SCSI_TEST_UNIT_READY:
			printf("TEST_UNIT_READY\r\n");
			Mass_CMD_TestUnitReady(&g_CBW, &g_CSW);
			break;
		case SCSI_READ10:
			printf("READ10\r\n");
			//SCSI_Read10_Cmd(CBW.bLUN, SCSI_LBA , SCSI_BlkLen);
			break;
		case SCSI_WRITE10:
			printf("WRITE10\r\n");
			//SCSI_Write10_Cmd(CBW.bLUN, SCSI_LBA , SCSI_BlkLen);
			break;
		case SCSI_VERIFY10:
			printf("VERIFY10\r\n");
			//SCSI_Verify10_Cmd(CBW.bLUN);
			break;
		case SCSI_FORMAT_UNIT:
			printf("FORMAT_UNIT\r\n");
			//SCSI_Format_Cmd(CBW.bLUN);
			break;
		/*Unsupported command*/
		case SCSI_MODE_SELECT10:
		case SCSI_MODE_SELECT6:
		case SCSI_SEND_DIAGNOSTIC:
		case SCSI_READ6:
		case SCSI_READ12:
		case SCSI_READ16:
		case SCSI_READ_CAPACITY16:
		case SCSI_WRITE6:
		case SCSI_WRITE12:
		case SCSI_WRITE16:
		case SCSI_VERIFY12:
		case SCSI_VERIFY16:
		default:
		{
			printf("UNSUPPORTED\r\n");
			_goToErrorStatus();
			break;
		}
	};

	return NUSB_SUCCESS;	
}

/* called when host send command or data out */
void MASS_OnReceive(void)
{
	int ret = NUSB_ERROR;
	u32 receivedLength;

	// read received data
	receivedLength = MASS_ReadReceived(&g_CBW, sizeof(g_CBW));

	switch(g_Status)
	{
		case MASS_SCSI_EXPECT_CBW:
		{
			printf(" CBW:");
			ret = _handleCBW(receivedLength);
			break;
		}	
		case MASS_SCSI_EXPECT_DATA:
		{
			printf(" RecvData\r\n");
			break;
		}
		default:
		{
			printf(" UNSUPPORT\r\n");
			_goToErrorStatus();
			break;
		}
	}

	if (NUSB_SUCCESS != ret)
	{
		_goToErrorStatus();	
	}

	return;
}

/* called when host need data or status in */
void MASS_OnSendFinish(void)
{
	switch(g_Status)
	{
		case MASS_SCSI_SEND_CSW:
		{
			printf(" SendCSW\r\n");

			/* waiting for ACK */
			g_Status = MASS_SCSI_EXPECT_ACK;

            /* TODO */
            SetEPRxStatus(MASS_RECEIVE_ENDPOINT, EP_RX_VALID); 

			/* send CSW */
			MASS_SendData(&g_CSW, sizeof(g_CSW));      
           
			break;
		}
		case MASS_SCSI_SEND_DATA:
		{
			printf(" ContinueSendData\r\n");
			break;
		}
		case MASS_SCSI_EXPECT_ACK:
		{
			printf(" Set EP2 RX VALID.\r\n");
			/* Send success, wait for next command */
			g_Status = MASS_SCSI_EXPECT_CBW;
			SetEPRxStatus(MASS_RECEIVE_ENDPOINT, EP_RX_VALID);
			break;
		}
		default:
		{
			printf(" UNSUPPORT\r\n");
			break;
		}
	}

	return;
}

void MASS_ResetState(void)
{
	g_Status = MASS_SCSI_EXPECT_CBW;
	return;	
}

void MASS_SetState(u8 state)
{
	g_Status = state;
	return;
}


