#ifndef _NUSB_TRANS_H_
#define _NUSB_TRANS_H_

/* read received data which is from host */
u32 NUSB_ReadReceivedData(u8 bEpAddr, u8* pBufferPointer, u32 wBufferSize);

/* send data to host */
u32 NUSB_Send(u8 bEpAddr, u8* pBufferPointer, u32 wBufferSize);


#endif

