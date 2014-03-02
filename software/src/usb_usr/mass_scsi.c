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

	printf(" SendSense\r\n");
	MASS_SendData(&g_Scsi_Sense_Data, DataLength);

	/* Send CSW when successfully send sense data */
	MASS_SetState(MASS_SCSI_SEND_CSW);
	
	return;	
}
