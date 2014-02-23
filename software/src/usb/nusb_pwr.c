#include "stm32f10x.h"
#include "usb_regs.h"
#include "nusb_intf.h"

void NUSB_PowerOn(void)
{
	uint16_t wRegVal;
	
	/* Enable the Pull-Up, TODO*/
	
	/*** CNTR_PWDN = 0 ***/
	wRegVal = CNTR_FRES;
	_SetCNTR(wRegVal);
	
	/*** CNTR_FRES = 0 ***/
	wRegVal = 0;
	_SetCNTR(wRegVal);
	/* this will trigger a reset interupt */

	/*** Clear pending interrupts ***/
	_SetISTR(wRegVal);
	
	/*** Set interrupt mask ***/
	wRegVal = g_devConf.IMR_MSK;
	_SetCNTR(wRegVal);
	
	return;
}

void NUSB_PowerOff(void)
{
	/* disable all interrupts and force USB reset */
	_SetCNTR(CNTR_FRES);
	/* clear interrupt status register */
	_SetISTR(0);
	
	/* Disable the Pull-Up, TODO*/
	
	/* switch-off device */
	_SetCNTR(CNTR_FRES | CNTR_PDWN);
}

void NUSB_Suspend(void)
{
	uint32_t i =0;
	uint16_t wCNTR;
    __IO uint32_t EP[8];    

	/* suspend preparation */
	
	/*Store CNTR value */
	wCNTR = _GetCNTR();  

    /* This a sequence to apply a force RESET to handle a robustness case */
    
	/*Store endpoints registers status */
    for (i=0;i<8;i++) EP[i] = _GetENDPOINT(i);
	
	/* unmask RESET flag */
	wCNTR|=CNTR_RESETM;
	_SetCNTR(wCNTR);
	
	/*apply FRES */
	wCNTR|=CNTR_FRES;
	_SetCNTR(wCNTR);
	
	/*clear FRES*/
	wCNTR&=~CNTR_FRES;
	_SetCNTR(wCNTR);
	
	/*poll for RESET flag in ISTR*/
	while((_GetISTR()&ISTR_RESET) == 0);
	
	/* clear RESET flag in ISTR */
	_SetISTR((uint16_t)CLR_RESET);
	
	/*restore Enpoints*/
	for (i=0;i<8;i++)
	_SetENDPOINT(i, EP[i]);
	
	/* Now it is safe to enter macrocell in suspend mode */
	wCNTR |= CNTR_FSUSP;
	_SetCNTR(wCNTR);
	
	/* force low-power mode in the macrocell */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_LPMODE;
	_SetCNTR(wCNTR);

    return;
}

void NUSB_Resume(void)
{
    uint16_t wCNTR;

    /* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
    /* restart the clocks */
    /* ...  */

    /* CNTR_LPMODE = 0 */
    wCNTR = _GetCNTR();
    wCNTR &= (~CNTR_LPMODE);
    _SetCNTR(wCNTR);    

    /* reset FSUSP bit */
    _SetCNTR(g_devConf.IMR_MSK);

    /* reverse suspend preparation */
    /* ... */ 
    return;
}



