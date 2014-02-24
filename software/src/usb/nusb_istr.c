#include <stdio.h>
#include "stm32f10x.h"
#include "usb_regs.h"
#include "nusb_intf.h"
#include "nusb_utils.h"
#include "nusb_core.h"
#include "nusb_pwr.h"

static void _LP_CTR(void)
{
	vu16 wIstr;
	vu16 EPVal;
	vu8  EPIndex;
	
	/* stay in loop while pending interrupts */
	while (((wIstr = _GetISTR()) & ISTR_CTR) != 0)
	{
		/* extract highest priority endpoint number */
		EPIndex = (uint8_t)(wIstr & ISTR_EP_ID);	

		/* Decode and service control endpoint interrupt */
		if (0 == EPIndex)
		{
            g_SaveRxStatus = _GetENDPOINT(ENDP0);
	        g_SaveTxStatus = g_SaveRxStatus & EPTX_STAT;
	        g_SaveRxStatus &=  EPRX_STAT;	    
			
			EPVal = _GetENDPOINT(ENDP0);
			if ((EPVal & EP_CTR_TX) != 0)
			{
				printf("I\r\n");
				_ClearEP_CTR_TX(ENDP0);
            	NUSB_EP0_InProcess();
				
			}
			if ((EPVal & EP_CTR_RX) != 0)
			{
				_ClearEP_CTR_RX(ENDP0);             
    		    if ((EPVal & EP_SETUP) != 0)
                {
					printf("S\r\n");
                    NUSB_EP0_SetupProcess();
                }
                else
                {
					printf("O\r\n");
                    NUSB_EP0_OutProcess();
                }
			}  

            //in case of racing condition, set rx and tx togither
            _SetEPRxTxStatus(ENDP0, g_SaveRxStatus, g_SaveTxStatus);
		}
		/* Decode and service non control endpoints interrupt  */
		else 
		{
			EPVal = _GetENDPOINT(EPIndex);

			if ((EPVal & EP_CTR_RX) != 0)
			{
				/* clear int flag */
				_ClearEP_CTR_RX(EPIndex);
				/* call OUT service function */
				printf("EP%hu_R:\r\n", EPIndex);
				(*(g_devOps.Ep_IN)[EPIndex-1])();			
			}
			
			if ((EPVal & EP_CTR_TX) != 0)
			{
				/* clear int flag */
				_ClearEP_CTR_TX(EPIndex);
				/* call IN service function */
				printf("EP%hu_T:\r\n", EPIndex);
				(*(g_devOps.Ep_IN)[EPIndex-1])();
			}
		} //else
	} //while

	return;
}

void NUSB_LP_INT(void)
{
	vu16 wIstr;
	wIstr = _GetISTR();

	if (wIstr & ISTR_CTR)
	{
		_SetISTR((uint16_t)CLR_CTR);
		_LP_CTR();
	}
	if (wIstr & ISTR_RESET)
	{
		_SetISTR((uint16_t)CLR_RESET);

		g_devOps.Reset();

		/* Set the device to response on default address
		   and enable device */
		_SetDADDR(0 | DADDR_EF);
        g_SetupStatus.ControlState = NUSB_STAT_WAIT_SETUP;

		printf("RESET\r\n");
	}
	if (wIstr & ISTR_WKUP)
	{
		_SetISTR((uint16_t)CLR_WKUP);
		NUSB_Resume();
		printf("WAKEUP\r\n");
	}
	if (wIstr & ISTR_SUSP)
	{
		//TODO, why first suspend then clear SUSP flag
		NUSB_Suspend();
		_SetISTR((uint16_t)CLR_SUSP); 
        printf("SUSPEND\r\n");
	}

	return;
}


