#include <stdlib.h>
#include "stm32f10x.h"
#include "utils.h"
#include "gradienter.h"
#include "led.h"
#include "acc_sensor.h"
#include "app_intf.h"
#include "xl345.h"
#include "power.h"

#define GRAD_FIFO_LENGTH    8

static void _GRAD_Start(void);
static void _GRAD_Stop(void);
struct APP_INTF g_appGradienter =
{
	_GRAD_Start,
    _GRAD_Stop
};

static void _gradInitAccSensor(void)
{
    u8 buffer[8];
    /* add software reset */
	buffer[0] = XL345_SOFT_RESET;
	XL345_Write(1, XL345_RESERVED1, buffer);

	delay_ms(50);

    /*--------------------------------------------------
    activity - inactivity 
    --------------------------------------------------*/
    /* set up a buffer with all the initialization for activity and inactivity */
    buffer[0] = 20; 
    buffer[1] = 5; 
    buffer[2] = 5;/* TIME_INACT - 5 seconds */
    buffer[3] =     /* ACT_INACT_CTL */
                XL345_ACT_DC 
                | XL345_ACT_X_ENABLE | XL345_ACT_Y_ENABLE | XL345_ACT_Z_ENABLE
                | XL345_INACT_AC | XL345_INACT_X_ENABLE 
                | XL345_INACT_Y_ENABLE | XL345_INACT_Z_ENABLE;
    XL345_Write(4, XL345_THRESH_ACT, buffer);

    /*--------------------------------------------------
    Power, bandwidth-rate, interupt enabling
    --------------------------------------------------*/

    /* set up a buffer with all the initization for power*/
    buffer[0] =                   /* BW_RATE */
                XL345_RATE_100 | XL345_LOW_NOISE;
    buffer[1] =                   /* POWER_CTL */
                XL345_ACT_INACT_SERIAL | XL345_AUTO_SLEEP |
                XL345_WAKEUP_8HZ | XL345_MEASURE;
    XL345_Write(2, XL345_BW_RATE, buffer);

    // set the FIFO control
    buffer[0] = XL345_FIFO_MODE_FIFO    | // set FIFO mode
            	0 | 		             // link to INT1
            	GRAD_FIFO_LENGTH;		 // number of samples
    XL345_Write(1, XL345_FIFO_CTL, buffer);

    // set data format
    buffer[0] = XL345_SPI4WIRE | XL345_INT_HIGH | XL345_10BIT |
                XL345_DATA_JUST_RIGHT | XL345_RANGE_2G;
    XL345_Write(1, XL345_DATA_FORMAT, buffer);

    //set interrupt map
    buffer[0] = 0; //all interrupts set to pin INT1
    XL345_Write(1, XL345_INT_MAP, buffer);		
    
    // turn on the interrupts
    buffer[0] = XL345_ACTIVITY | XL345_INACTIVITY | 
                XL345_WATERMARK;
    XL345_Write(1, XL345_INT_ENABLE, buffer);

    return;
}

static void _gradDeinitAccSensor(void)
{
	u8 reset = XL345_SOFT_RESET;

    /* just reset */
	XL345_Write(1, XL345_RESERVED1, &reset);
	delay_ms(50);

    return;
}
/*
static void _gradSetupColor(u16 level, u32* pcolor){
	u32 color = 0;
	
	if (level > 0xFFF) {
		level = 0xFFF;
	}

	if (level <= 0x0F){
		LED2_SET_GREEN(color, level);
	} else if (level > 0x0F && level <= 0xFF) {
		LED2_SET_GREEN(color, 1);	
		LED2_SET_BLUE(color, level >> 4);
	} else {
		LED2_SET_GREEN(color, 1);	
		LED2_SET_BLUE(color, 1);
		LED2_SET_RED(color, level >> 8);		
	}

	*pcolor = color;

	return;
} */

static void _gradOnDataReady(void)
{
    s16 x, y, z;
	u16 uX, uY;
	u32 color = 0;

    ACC_ReadAcc(GRAD_FIFO_LENGTH, &x, &y, &z);

	uX = abs(x);
	uX >>= 5;
	if (uX > 0x0F) {
		uX = 0x0F;
	}
	if (x > 0){
		LED4_SET_COLOR(color, uX);
	} else {
	    LED3_SET_COLOR(color, uX);
	}

	uY = abs(y);
	uY >>= 5;
	if (uY > 0x0FFF) {
		uY = 0x0FFF;
	}

	if (y > 0){
	    LED2_SET_RED(color, uY);
	} else {
	    LED1_SET_RED(color, uY);
	}

	LED_SetColor(color);

    return;
}

void _gradIRQHandler(u8 irq)
{
    /* if data is ready */
    if (irq & XL345_WATERMARK){
        _gradOnDataReady();
    }

    /* wake up */
    if (irq & XL345_ACTIVITY) {    
        PWR_Restore();        
    }

    /* into low power mode */
    if (irq & XL345_INACTIVITY){
        PWR_LowPower();
    }

	return;
}

static void _GRAD_Start(void)
{
    /* register irq handler to acc module */
    ACC_SetIRQHandler(_gradIRQHandler);

    _gradInitAccSensor();

    return;
}

static void _GRAD_Stop(void)
{
	_gradDeinitAccSensor();

    LED_SetColor(0x00);

    return;
}

