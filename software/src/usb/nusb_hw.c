#include "stm32f10x.h"
#include "nusb_hw.h"
#include "usb_regs.h"
#include "nusb_conf.h"

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
	wRegVal = NUSB_IMR_MSK;
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

void NUSB_HwConfig(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the EXTI line 18 connected internally to the USB IP */
	EXTI_ClearITPendingBit(EXTI_Line18);
	EXTI_InitStructure.EXTI_Line = EXTI_Line18;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Select USBCLK source */
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
	/* Enable the USB clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

	/* 2 bit for pre-emption priority, 2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USB Wake-up interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
		
	return;	  
}


