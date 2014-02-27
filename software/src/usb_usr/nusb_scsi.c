#include "stm32f10x.h"
#include "nusb_scsi.h"
#include "nusb_trans.h"
#include "nusb_conf.h"
#include "nusb_intf.h"

#define NUSB_SCSI_SendData(buf, size)      NUSB_Send(NUSB_SEND_ENDPOINT, (u8*)buf, size);
#define NUSB_SCSI_ReadReceived(buf, size)  NUSB_ReadReceivedData(NUSB_RECEIVE_ENDPOINT, (u8*)buf, size);

enum NUSB_SCSI_STATUS
{
	NUSB_SCSI_EXPECT_CBW,
	NUSB_SCSI_EXPECT_DATA,
	NUSB_SCSI_SENDING,
	NUSB_SCSI_ERROR
};

/* Bulk-only Command Block Wrapper */
typedef struct _Bulk_Only_CBW
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataLength;
  uint8_t  bmFlags;
  uint8_t  bLUN;
  uint8_t  bCBLength;
  uint8_t  CB[16];
}
Bulk_Only_CBW;

/* Bulk-only Command Status Wrapper */
typedef struct _Bulk_Only_CSW
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataResidue;
  uint8_t  bStatus;
}
Bulk_Only_CSW;

static u8 g_Status;
static Bulk_Only_CBW g_CBW;
static Bulk_Only_CSW g_CSW;

static void _goToErrorStatus(void)
{
	g_Status = NUSB_SCSI_ERROR;

	SetEPTxStatus(NUSB_SEND_ENDPOINT,    EP_TX_STALL);
	SetEPRxStatus(NUSB_RECEIVE_ENDPOINT, EP_RX_STALL);		

	return;	
}

static int _decodeCBW(void)
{
	
}

/* called when host send command or data out */
void NUSB_OnReceive(void)
{
	int ret;
	u32 receivedLength;

	// read received data
	receivedLength = NUSB_SCSI_ReadReceived(&g_CBW, sizeof(g_CBW));
	
	switch(g_Status)
	{
		case NUSB_SCSI_EXPECT_CBW:
		{
			ret = _decodeCBW();
			break;
		}	
		case NUSB_SCSI_EXPECT_DATA:
		{
			break;
		}
		default:
		{
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
void NUSB_OnSendFinish(void)
{
	return;
}

void NUSB_ResetStatus(void)
{
	g_Status = NUSB_SCSI_EXPECT_CBW;
	return;	
}


