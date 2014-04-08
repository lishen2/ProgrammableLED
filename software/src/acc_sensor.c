#include <stdio.h>
#include "stm32f10x.h"
#include "acc_sensor.h"
#include "xl345.h"
#include "power.h"
#include "display_state.h"

#define ACC_INT_PIN         GPIO_Pin_0
#define ACC_INT_PORT        GPIOA
#define ACC_INT_RCC         RCC_APB2Periph_GPIOA
#define ACC_INT_PORTSOURCE  GPIO_PortSourceGPIOA
#define ACC_INT_PINSOURCE   GPIO_PinSource0
#define ACC_INT_IRQ         EXTI0_IRQn
#define ACC_INT_IRQROUTINE  EXTI0_IRQHandler
#define ACC_INT_EXTILINE    EXTI_Line0

#define ACC_FIFO_LENGTH_SHIFT 3
/* how many elements used in fifo */
#define ACC_FIFO_LENGTH      (1<<ACC_FIFO_LENGTH_SHIFT)

/* for 10bit resolution, right justified, 
   leave the sign bit and 10bit data */
#define ACC_DATAMASK_10BIT_JUST_RIGHT    0x83FF

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

static void _xl345Setup(void)
{
    u8 buffer[8];
    /* add software reset */

    /*--------------------------------------------------
    TAP Configuration
    --------------------------------------------------*/
    /* set up a buffer with all the initialization for tap */
    buffer[0] = XL345_THRESH_TAP; /* register address */
    buffer[1] = 80; /* THRESH_TAP = 5 Gee (1 lsb = 1/16 gee) */	
    xl345Write(2,buffer);

    buffer[0] = XL345_DUR; /* register address */
    buffer[1] = 13; /* DUR = 8ms 0.6125ms/lsb */
    buffer[2] = 80; /* LATENT = 100 ms 1.25ms/lsb */
    buffer[3] = 240;/* WINDOW 300ms 1.25ms/lsb */
    xl345Write(4,buffer);

    buffer[0] = XL345_TAP_AXES; /* register address */
    buffer[1] = XL345_TAP_Z_ENABLE | XL345_TAP_Y_ENABLE 
                | XL345_TAP_X_ENABLE | XL345_TAP_SUPPRESS;
    xl345Write(2,buffer);

    /*--------------------------------------------------
    activity - inactivity 
    --------------------------------------------------*/
    /* set up a buffer with all the initialization for activity and inactivity */
    buffer[0] = XL345_THRESH_ACT; /* register address */
    buffer[1] = 80; /* THRESH_ACT = 80/16 = 5 Gee (1 lsb = 1/16 gee) */
    buffer[2] = 4; /* THRESH_INACT = 14/16 .25 Gee (1 lsb = 1/16 gee) */
    buffer[3] = 5;/* TIME_INACT - 5 seconds */
    buffer[4] =     /* ACT_INACT_CTL */
                XL345_ACT_DC 
                | XL345_ACT_X_ENABLE | XL345_ACT_Y_ENABLE | XL345_ACT_Z_ENABLE
                | XL345_INACT_AC | XL345_INACT_X_ENABLE 
                | XL345_INACT_Y_ENABLE | XL345_INACT_Z_ENABLE;
    xl345Write(5,buffer);

    /*--------------------------------------------------
    Power, bandwidth-rate, interupt enabling
    --------------------------------------------------*/

    /* set up a buffer with all the initization for power*/
    buffer[0] = XL345_BW_RATE;    /* register address */
    buffer[1] =                   /* BW_RATE */
                XL345_RATE_100 | XL345_LOW_NOISE;
    buffer[2] =                   /* POWER_CTL */
                XL345_ACT_INACT_SERIAL | XL345_AUTO_SLEEP | 
                XL345_WAKEUP_8HZ | XL345_MEASURE;
    xl345Write(3,buffer);

    // set the FIFO control
    buffer[0] = XL345_FIFO_CTL;
    buffer[1] = XL345_FIFO_MODE_STREAM | // set FIFO mode
            	0 | 		             // link to INT1
            	ACC_FIFO_LENGTH;		 // number of samples
    xl345Write(2, buffer);

    // set data format
    buffer[0] = XL345_DATA_FORMAT;
    buffer[1] = XL345_SPI4WIRE | XL345_INT_HIGH | XL345_10BIT |
                XL345_DATA_JUST_RIGHT | XL345_RANGE_2G;
    xl345Write(2, buffer);

    //set interrupt map
    buffer[0] = XL345_INT_MAP;
    buffer[1] = 0; //all interrupts set to pin INT1
    xl345Write(2, buffer);		
    
    // turn on the interrupts
    buffer[0] = XL345_INT_ENABLE;
    buffer[1] = XL345_ACTIVITY | XL345_INACTIVITY | XL345_WATERMARK; 
    xl345Write(2, buffer);		

    xl345Read(1, 0x00, buffer);
    printf("Read device id: [%hhu]\r\n", buffer[0]);
    
    return;
}

void ACC_Init(void)
{
    xl345Init();
	_initInterrupt();
    _xl345Setup();

    return;
}

static void _handleWatermark(void)
{
	u8 buf[6];
    u16 x, y, z;
    s32 avX, avY, avZ, sign;
    int i;

    avX = 0;
    avY = 0;
    avZ = 0;
    
    for (i = 0; i < ACC_FIFO_LENGTH; ++i){
        xl345Read(sizeof(buf), XL345_DATAX0, buf);
        x = (buf[1] << 8 | buf[0]) & ACC_DATAMASK_10BIT_JUST_RIGHT;
        y = (buf[3] << 8 | buf[2]) & ACC_DATAMASK_10BIT_JUST_RIGHT;        
        z = (buf[5] << 8 | buf[4]) & ACC_DATAMASK_10BIT_JUST_RIGHT;             

        avX += (s32)(s16)x;
        avY += (s32)(s16)y;
        avZ += (s32)(s16)z;        
    }

    STATE_OnDataReady(avX/ACC_FIFO_LENGTH, /* leave the optimizing job to compiler */
                      avY/ACC_FIFO_LENGTH, 
                      avZ/ACC_FIFO_LENGTH);

    return;
}

static u8 _handle_IRQ(void)
{
    u8 source;

    /* read int source */
	xl345Read(1, XL345_INT_SOURCE, &source);

    /* if data is ready */
    if (source & XL345_WATERMARK){
        _handleWatermark();
    }

    /* wake up */
    if (source & XL345_ACTIVITY) {
        PWR_Restore();        
    }

    /* into low power mode */
    if (source & XL345_INACTIVITY){
        PWR_LowPower();
    }

    return;
}

void ACC_INT_IRQROUTINE(void)
{
	if (SET == EXTI_GetFlagStatus(ACC_INT_EXTILINE)){
		EXTI_ClearFlag(ACC_INT_EXTILINE);
		printf("INT\r\n");
		//handle irq here
        _handle_IRQ();
	}	
}

