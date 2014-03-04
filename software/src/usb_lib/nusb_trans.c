#include "stm32f10x.h"
#include "usb_regs.h"
#include "usb_mem.h"
#include "nusb_trans.h"

/* read received data which is from host */
u32 NUSB_ReadReceivedData(u8 bEpNum, u8* pBufferPointer, u32 wBufferSize)
{
	u32 DataLength = 0;
	
	/* Get the number of received data on the selected Endpoint */
	DataLength = GetEPRxCount(bEpNum);
	if (DataLength > wBufferSize)
	{
		DataLength = wBufferSize;
	}	

	/* Use the memory interface function to write to the selected endpoint */
	PMAToUserBufferCopy(pBufferPointer, GetEPRxAddr(bEpNum), DataLength);
	
	/* Return the number of received data */
	return DataLength;
}

/* send data to host */
u32 NUSB_Send(u8 bEpNum, u8* pBufferPointer, u32 wBufferSize)
{
	/* Use the memory interface function to write to the selected endpoint */
	UserToPMABufferCopy(pBufferPointer, GetEPTxAddr(bEpNum), wBufferSize);
	
	/* Update the data length in the control register */
	_SetEPTxCount(bEpNum, wBufferSize);

	_SetEPTxStatus(bEpNum, EP_TX_VALID);
	
	return wBufferSize;
}



