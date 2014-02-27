#ifndef _NUSB_SCSI_H_
#define _NUSB_SCSI_H_

/* called when received data from host */
void NUSB_OnReceive(void);

/* called when finished sending data to host */
void NUSB_OnSendFinish(void);

/* Reset scsi protocal module to the default status */
void NUSB_ResetStatus(void);

#endif

