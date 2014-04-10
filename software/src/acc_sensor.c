#include <stdio.h>
#include "stm32f10x.h"
#include "utils.h"
#include "acc_sensor.h"
#include "xl345.h"
#include "power.h"

#define ACC_INT_PIN         GPIO_Pin_0
#define ACC_INT_PORT        GPIOA
#define ACC_INT_RCC         RCC_APB2Periph_GPIOA
#define ACC_INT_PORTSOURCE  GPIO_PortSourceGPIOA
#define ACC_INT_PINSOURCE   GPIO_PinSource0
#define ACC_INT_IRQ         EXTI0_IRQn
#define ACC_INT_IRQROUTINE  EXTI0_IRQHandler
#define ACC_INT_EXTILINE    EXTI_Line0

ACC_IRQHandler g_handler = NULL;

static void _initInterrupt(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(ACC_INT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = ACC_INT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
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
	_initInterrupt();

    return;
}

void ACC_ReadAcc(u8 fifoLen, s16 *X, s16 *Y, s16*Z)
{
	u8 buf[6];
    u16 x, y, z;
    s32 avX, avY, avZ;
    u8 i;

    avX = 0;
    avY = 0;
    avZ = 0;
    
    for (i = 0; i < fifoLen; ++i){
        xl345Read(sizeof(buf), XL345_DATAX0, buf);
        x = (buf[1] << 8 | buf[0]);
        y = (buf[3] << 8 | buf[2]);        
        z = (buf[5] << 8 | buf[4]);             

        avX += (s32)(s16)x;
        avY += (s32)(s16)y;
        avZ += (s32)(s16)z;        
    }

	*X = (s16)(avX/fifoLen);
	*Y = (s16)(avY/fifoLen);
	*Z = (s16)(avZ/fifoLen);

    return;
}

void ACC_SetIRQHandler(ACC_IRQHandler handler)
{
	g_handler = handler;
}

void ACC_INT_IRQROUTINE(void)
{
	u8 irq;

	if (SET == EXTI_GetFlagStatus(ACC_INT_EXTILINE)){
		EXTI_ClearFlag(ACC_INT_EXTILINE);

	    /* read int source */
		xl345Read(1, XL345_INT_SOURCE, &irq);

		/* call handler */
		if (NULL != g_handler){
			g_handler(irq);
		}
	}	
}

