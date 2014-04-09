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
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
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
#if 0
static void _xl345Setup(void)
{
    u8 buffer[8];
    /* add software reset */
	buffer[0] = XL345_SOFT_RESET;
	xl345Write(1, XL345_RESERVED1, buffer);

	delay_ms(200);

    /*--------------------------------------------------
    TAP Configuration
    --------------------------------------------------*/
    /* set up a buffer with all the initialization for tap */
    buffer[0] = 80; /* THRESH_TAP = 5 Gee (1 lsb = 1/16 gee) */	
    xl345Write(1, XL345_THRESH_TAP, buffer);

    buffer[0] = 13; /* DUR = 8ms 0.6125ms/lsb */
    buffer[1] = 80; /* LATENT = 100 ms 1.25ms/lsb */
    buffer[2] = 240;/* WINDOW 300ms 1.25ms/lsb */
    xl345Write(3, XL345_DUR, buffer);

    buffer[0] = XL345_TAP_Z_ENABLE | XL345_TAP_Y_ENABLE 
                | XL345_TAP_X_ENABLE | XL345_TAP_SUPPRESS;
    xl345Write(1, XL345_TAP_AXES, buffer);

    /*--------------------------------------------------
    activity - inactivity 
    --------------------------------------------------*/
    /* set up a buffer with all the initialization for activity and inactivity */
    buffer[0] = 30; /* THRESH_ACT = 80/16 = 5 Gee (1 lsb = 1/16 gee) */
    buffer[1] = 5; /* THRESH_INACT = 14/16 .25 Gee (1 lsb = 1/16 gee) */
    buffer[2] = 5;/* TIME_INACT - 5 seconds */
    buffer[3] =     /* ACT_INACT_CTL */
                XL345_ACT_DC 
                | XL345_ACT_X_ENABLE | XL345_ACT_Y_ENABLE | XL345_ACT_Z_ENABLE
                | XL345_INACT_AC | XL345_INACT_X_ENABLE 
                | XL345_INACT_Y_ENABLE | XL345_INACT_Z_ENABLE;
    xl345Write(4, XL345_THRESH_ACT, buffer);

    /*--------------------------------------------------
    Power, bandwidth-rate, interupt enabling
    --------------------------------------------------*/

    /* set up a buffer with all the initization for power*/
    buffer[0] =                   /* BW_RATE */
                XL345_RATE_100 | XL345_LOW_NOISE;
    buffer[1] =                   /* POWER_CTL */
                XL345_ACT_INACT_SERIAL | XL345_AUTO_SLEEP | 
                XL345_WAKEUP_8HZ | XL345_MEASURE;
    xl345Write(2, XL345_BW_RATE, buffer);

    // set the FIFO control
    buffer[0] = XL345_FIFO_MODE_FIFO    | // set FIFO mode
            	0 | 		             // link to INT1
            	ACC_FIFO_LENGTH;		 // number of samples
    xl345Write(1, XL345_FIFO_CTL, buffer);

    // set data format
    buffer[0] = XL345_SPI4WIRE | XL345_INT_HIGH | XL345_10BIT |
                XL345_DATA_JUST_RIGHT | XL345_RANGE_2G;
    xl345Write(1, XL345_DATA_FORMAT, buffer);

    //set interrupt map
    buffer[0] = 0; //all interrupts set to pin INT1
    xl345Write(1, XL345_INT_MAP, buffer);		
    
    // turn on the interrupts
    buffer[0] = XL345_ACTIVITY | XL345_INACTIVITY | 
                XL345_WATERMARK;
    xl345Write(1, XL345_INT_ENABLE, buffer);		

    xl345Read(1, 0x00, buffer);
    printf("Read device id: [%hhu]\r\n", buffer[0]);
    
    return;
}
#endif

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
#if 0
static void _handle_IRQ(void)
{
    u8 source;

    /* read int source */
	xl345Read(1, XL345_INT_SOURCE, &source);

    /* if data is ready */
    if (source & XL345_WATERMARK){
        printf("Watermark.\r\n");        
        _handleWatermark();
    }

    /* wake up */
    if (source & XL345_ACTIVITY) {
        printf("Activity.\r\n");                
        PWR_Restore();        
    }

    /* into low power mode */
    if (source & XL345_INACTIVITY){
        printf("Inactivity.\r\n");                
        PWR_LowPower();
    }

    return;
}
#endif

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

