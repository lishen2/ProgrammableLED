#include <stdio.h>
#include "stm32f10x.h"
#include "usb_regs.h"
#include "nusb_trans.h"
#include "mass_defs.h"
#include "mass_scsi.h"
#include "mass_conf.h"
#include "mass_state.h"

/* sense related functions and data */
static uint8_t g_Scsi_Sense_Data[] =
{
	0x70, /*RespCode*/
	0x00, /*SegmentNumber*/
	SCSI_SENSE_KEY_NO_SENSE, /* Sens_Key*/
	0x00,
	0x00,
	0x00,
	0x00, /*Information*/
	0x0A, /*AdditionalSenseLength*/
	0x00,
	0x00,
	0x00,
	0x00, /*CmdInformation*/
	SCSI_SENSE_ASC_NO_SENSE, /*Asc*/
	0x00, /*ASCQ*/
	0x00, /*FRUC*/
	0x00, /*TBD*/
	0x00,
	0x00 /*SenseKeySpecific*/
};

void MASS_SetSenseData(uint8_t lun, uint8_t Sens_Key, uint8_t Asc)
{
	g_Scsi_Sense_Data[2] = Sens_Key;
	g_Scsi_Sense_Data[12] = Asc;

	return;
}

void MASS_RresetSenseData(uint8_t lun)
{
	g_Scsi_Sense_Data[2] = SCSI_SENSE_KEY_NO_SENSE;
	g_Scsi_Sense_Data[12] = SCSI_SENSE_ASC_NO_SENSE;

	return;
}

u8* Mass_GetSenseData(uint8_t lun)
{
	return (u8*)g_Scsi_Sense_Data;
}

void Mass_CMD_TestUnitReady(MASS_Bulk_Only_CBW *cbw, MASS_Bulk_Only_CSW *csw)
{
	if (cbw->bLUN <= MASS_MAX_LUN){
		csw->bStatus = SCSI_CSW_PASSED;	
	} else {
		csw->bStatus = SCSI_CSW_FAILED;
		MASS_SetSenseData(cbw->bLUN, SCSI_SENSE_KEY_NOT_READY, SCSI_SENSE_ASC_MEDIUM_NOT_PRESENT);	
	}

	MASS_SetState(MASS_SCSI_EXPECT_ACK);

	printf(" SendCSW\r\n");
	MASS_SendData(csw, sizeof(MASS_Bulk_Only_CSW));

	return;	
}

void MASS_CMD_RequestSense(MASS_Bulk_Only_CBW *cbw, MASS_Bulk_Only_CSW *csw)
{
	uint8_t DataLength;
	
	if (cbw->CB[4] < sizeof(g_Scsi_Sense_Data))
	{
		DataLength = cbw->CB[4];
	}
	else
	{
		DataLength = sizeof(g_Scsi_Sense_Data);
	}

	/* Send CSW when successfully send sense data */
	csw->bStatus = SCSI_CSW_PASSED;
	MASS_SetState(MASS_SCSI_SEND_CSW);

	printf(" SendSense\r\n");
	MASS_SendData(&g_Scsi_Sense_Data, DataLength);
	
	return;	
}

static uint8_t g_Page00_Inquiry_Data[] =
{
	0x00, /* PERIPHERAL QUALIFIER & PERIPHERAL DEVICE TYPE*/
	0x00,
	0x00,
	0x00,
	0x00 /* Supported Pages 00*/
};

static uint8_t g_Standard_Inquiry_Data[] =
  {
    0x00,          /* Direct Access Device */
    0x80,          /* RMB = 1: Removable Medium */
    0x00,          /* Version: No conformance claim to standard */
    0x01,

    36 - 4,          /* Additional Length */
    0x00,          /* SCCS = 1: Storage Controller Component */
    0x00,
    0x00,
    /* Vendor Identification */
    'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ',
    /* Product Identification */
    'N', 'A', 'N', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',
    'D', 'i', 's', 'k', ' ',
    /* Product Revision Level */
    '1', '.', '0', ' '
  };

void MASS_CMD_Inquiry_Cmd(MASS_Bulk_Only_CBW *cbw, MASS_Bulk_Only_CSW *csw)
{
	uint8_t* Inquiry_Data;
	uint16_t Inquiry_Data_Length;
	u16 wEPVal1, wEPVal2;
	
	if (cbw->CB[1] & 0x01)/*Evpd is set*/
	{
		Inquiry_Data = g_Page00_Inquiry_Data;
		Inquiry_Data_Length = sizeof(g_Page00_Inquiry_Data);
	}
	else
	{
		Inquiry_Data = g_Standard_Inquiry_Data;

		if (cbw->CB[4] < sizeof(g_Standard_Inquiry_Data))
			Inquiry_Data_Length = cbw->CB[4];
		else
			Inquiry_Data_Length = sizeof(g_Standard_Inquiry_Data);
	}

	/* Send CSW when successfully send data */
	csw->bStatus = SCSI_CSW_PASSED;
	csw->dDataResidue = cbw->dDataLength - Inquiry_Data_Length;
	MASS_SetState(MASS_SCSI_SEND_CSW);

	//printf(" SendInquiry\r\n");
	wEPVal1 = _GetENDPOINT(ENDP1);
	wEPVal2	= _GetENDPOINT(ENDP2);
	printf("Before EP1:%hx EP2:%hx\r\n", wEPVal1, wEPVal2);	

	MASS_SendData(Inquiry_Data, Inquiry_Data_Length);

  	wEPVal1 = _GetENDPOINT(ENDP1);
	wEPVal2	= _GetENDPOINT(ENDP2);
	printf("After EP1:%hx EP2:%hx\r\n", wEPVal1, wEPVal2);	

	return;
}
