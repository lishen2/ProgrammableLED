#include <stdio.h>
#include "stm32f10x.h"
#include "acc_sensor.h"
#include "xl345.h"

#define ACC_INT_PIN         GPIO_Pin_0
#define ACC_INT_PORT        GPIOA
#define ACC_INT_RCC         RCC_APB2Periph_GPIOA
#define ACC_INT_PORTSOURCE  GPIO_PortSourceGPIOA
#define ACC_INT_PINSOURCE   GPIO_PinSource0
#define ACC_INT_IRQ         EXTI0_IRQn
#define ACC_INT_IRQROUTINE  EXTI0_IRQHandler
#define ACC_INT_EXTILINE    EXTI_Line0

static void _initInterrupt(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(ACC_INT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = ACC_INT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ACC_INT_PORT, &GPIO_InitStructure);	
	GPIO_EXTILineConfig(ACC_INT_PORTSOURCE, ACC_INT_PINSOURCE); 

    NVIC_InitStructure.NVIC_IRQChannel = ACC_INT_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	EXTI_InitStructure.EXTI_Line = ACC_INT_EXTILINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	return;
}

void ACC_Init(void)
{
    xl345Init();
    xl345Setup();
    
	_initInterrupt();

    return;
}
/*
u16 ACC_ReadZ(void)
{
	u16 Z;
	u8 ZAry[2];

	xl345Read(2, 0x36, ZAry);
	
	Z = ZAry[0];
	Z = Z << 8;
	Z |= ZAry[1]; 

	return Z;
}*/

void ACC_INT_IRQROUTINE(void)
{
	if (SET == EXTI_GetFlagStatus(ACC_INT_EXTILINE)){
		EXTI_ClearFlag(ACC_INT_EXTILINE);
		printf("INT\r\n");
		//handle irq here

	}	
}

