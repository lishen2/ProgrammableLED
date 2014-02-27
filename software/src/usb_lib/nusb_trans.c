#include "stm32f10x.h"
#include "usb_regs.h"
#include "nusb_trans.h"

/* read received data which is from host */
u32 NUSB_ReadReceivedData(u8 bEpAddr, u8* pBufferPointer, u32 wBufferSize)
{
	u32 DataLength = 0;
	
	/* Get the number of received data on the selected Endpoint */
	DataLength = GetEPRxCount(bEpAddr & 0x7F);
	if (DataLength > wBufferSize)
	{
		DataLength = wBufferSize;
	}	

	/* Use the memory interface function to write to the selected endpoint */
	PMAToUserBufferCopy(pBufferPointer, GetEPRxAddr(bEpAddr & 0x7F), DataLength);
	
	/* Return the number of received data */
	return DataLength;
}

/* send data to host */
u32 NUSB_Send(u8 bEpAddr, u8* pBufferPointer, u32 wBufferSize)
{
	/* Use the memory interface function to write to the selected endpoint */
	UserToPMABufferCopy(pBufferPointer, GetEPTxAddr(bEpAddr & 0x7F), wBufferSize);
	
	/* Update the data length in the control register */
	SetEPTxCount((bEpAddr & 0x7F), wBufferSize);
	
	return wBufferSize;
}



