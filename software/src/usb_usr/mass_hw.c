#include "stm32f10x.h"
#include "mass_hw.h"
#include "usb_regs.h"
#include "mass_conf.h"

void MASS_HwConfig(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;    

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
    /* Enalbe D+ Pull pu PIN, GPIO CLOCK */
    RCC_APB2PeriphClockCmd(MASS_PULLUP_GPIO_CLK, ENABLE);

	/* 2 bit for pre-emption priority, 2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USB Wake-up interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);	

    /* config pull-up pin */
    GPIO_InitStructure.GPIO_Pin = MASS_PULLUP_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MASS_PULLUP_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(MASS_PULLUP_GPIO, MASS_PULLUP_PIN);
	
	return;	  
}


