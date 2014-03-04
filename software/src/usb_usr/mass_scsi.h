#ifndef _MASS_SCSI_H_
#define _MASS_SCSI_H_

void MASS_SetSenseData(uint8_t lun, uint8_t Sens_Key, uint8_t Asc);
void MASS_RresetSenseData(uint8_t lun);
u8* MASS_GetSenseData(uint8_t lun);

void Mass_CMD_TestUnitReady(MASS_Bulk_Only_CBW *cbw, MASS_Bulk_Only_CSW *csw);
void MASS_CMD_RequestSense(MASS_Bulk_Only_CBW *cbw, MASS_Bulk_Only_CSW *csw);
void MASS_CMD_Inquiry_Cmd(MASS_Bulk_Only_CBW *cbw, MASS_Bulk_Only_CSW *csw);


#endif

