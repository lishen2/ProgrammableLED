#ifndef _MASS_STATE_H_
#define _MASS_STATE_H_

/* SCSI state mechine status */
enum MASS_SCSI_STATUS
{
	MASS_SCSI_EXPECT_CBW,
	MASS_SCSI_EXPECT_DATA,
	MASS_SCSI_EXPECT_ACK,
	MASS_SCSI_SEND_DATA,
	MASS_SCSI_SEND_CSW,
	MASS_SCSI_ERROR
};

/* send data to host */
#define MASS_SendData(buf, size)      NUSB_Send(MASS_SEND_ENDPOINT, (u8*)buf, size)

/* read received data from host */
#define MASS_ReadReceived(buf, size)  NUSB_ReadReceivedData(MASS_RECEIVE_ENDPOINT, (u8*)buf, size)



/* called when received data from host */
void MASS_OnReceive(void);

/* called when finished sending data to host */
void MASS_OnSendFinish(void);

/* Reset scsi protocal module to the default status */
void MASS_ResetState(void);

void MASS_SetState(u8 state);

#endif

