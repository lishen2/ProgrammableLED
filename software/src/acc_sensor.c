#include <stdio.h>
#include "stm32f10x.h"
#include "acc_sensor.h"
#include "xl345.h"

void ACC_Init(void)
{
    u8 buffer[8];
    xl345Init();

    /* add soft ware reset */

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
                XL345_WAKEUP_8HZ | XL345_MEASURE;
    xl345Write(3,buffer);

    // set the FIFO control
    buffer[0] = XL345_FIFO_CTL;
    buffer[1] = XL345_FIFO_MODE_STREAM | // set FIFO mode
            	0 | 		 // link to INT1
            	30;		     // number of samples
    xl345Write(2, buffer);

    //set interrupt map
    buffer[0] = XL345_INT_MAP;
    buffer[1] = XL345_ACTIVITY | XL345_INACTIVITY; //send activity and inactivity to int2
    xl345Write(2, buffer);		
    
    // turn on the interrupts
    buffer[0] = XL345_INT_ENABLE;
    buffer[1] = XL345_WATERMARK | XL345_DOUBLETAP | 
                XL345_ACTIVITY | XL345_INACTIVITY;
    xl345Write(2, buffer);		

    xl345Read(1, 0x00, buffer);
    printf("Read device id: [%hhu]\r\n", buffer[0]);

    return;
}



u16 ACC_ReadX(void);
u16 ACC_ReadY(void);

u16 ACC_ReadZ(void)
{
	u16 Z;
	u8 ZAry[2];

	xl345Read(2, 0x36, ZAry);
	
	Z = ZAry[0];
	Z = Z << 8;
	Z |= ZAry[1]; 

	return Z;
}


