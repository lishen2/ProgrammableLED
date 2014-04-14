#include <stdio.h>
#include "stm32f10x.h"
#include "utils.h"
#include "break_light.h"
#include "xl345.h"
#include "led.h"
#include "app_intf.h"
#include "acc_sensor.h"
#include "power.h"

#define BKL_TIMER          TIM3
#define BKL_TIM_RCC        RCC_APB1Periph_TIM3
#define BKL_TIM_IRQ        TIM3_IRQn
#define BKL_TIM_ROUTINE    TIM3_IRQHandler
#define BKL_TIMER_DELAY    1000

#define BKL_FIFO_LENGTH    16

#define BKL_TURNLIGHT_MASK   0x000F00F0
#define BKL_CLEAR_TURNLIGHT(color)  (color &= ~BKL_TURNLIGHT_MASK)
#define BKL_CLEAR_BREAKLIGHT(color) (color &= BKL_TURNLIGHT_MASK)

#define BKL_LED_TURNLEFT()  BKL_CLEAR_TURNLIGHT(g_ledColor);\
                            g_ledColor |= 0x000000A0;\
                            LED_SetColor(g_ledColor);

#define BKL_LED_TURNRIGHT()  BKL_CLEAR_TURNLIGHT(g_ledColor);\
                             g_ledColor |= 0x000F0000;\
                             LED_SetColor(g_ledColor);

#define BKL_LED_STRAIGHTAHEAD() BKL_CLEAR_TURNLIGHT(g_ledColor);\
                                LED_SetColor(g_ledColor);

#define BKL_LED_DECELERATION()  BKL_CLEAR_BREAKLIGHT(g_ledColor);\
                                g_ledColor |= 0xFF00F00F;\
                                LED_SetColor(g_ledColor);

#define BKL_LED_ACCELERATION()  BKL_CLEAR_BREAKLIGHT(g_ledColor);\
                                g_ledColor |= 0x00A01A01;\
                                LED_SetColor(g_ledColor);

#define BKL_LED_CONSTANTSPEED()  BKL_CLEAR_BREAKLIGHT(g_ledColor);  \
                                 g_ledColor |= 0x00001001;\
                                 LED_SetColor(g_ledColor);

static u32 g_ledColor;
static s16 g_lastZ;

static void _BKL_Start(void);
static void _BKL_Stop(void);

struct APP_INTF g_appBreakLight = 
{
	_BKL_Start,
	_BKL_Stop,
};

static void _bklTimerInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(BKL_TIM_RCC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = BKL_TIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(BKL_TIMER);	
	TIM_TimeBaseStructure.TIM_Period = 0;	
	TIM_TimeBaseStructure.TIM_Prescaler = 8000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(BKL_TIMER, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(BKL_TIMER, DISABLE);
	TIM_SetCounter(BKL_TIMER, BKL_TIMER_DELAY);

	TIM_Cmd(BKL_TIMER, DISABLE);
	TIM_ITConfig(BKL_TIMER, TIM_IT_Update, DISABLE);    
    return;
}

static void _bklTimerDeinit(void)
{
	NVIC_InitTypeDef         NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = BKL_TIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(BKL_TIMER);	
	TIM_Cmd(BKL_TIMER, DISABLE);
	TIM_ITConfig(BKL_TIMER, TIM_IT_Update, DISABLE);    
	RCC_APB1PeriphClockCmd(BKL_TIM_RCC, DISABLE);  
    
    return;
}

static void _startTimer(void)
{
    
}

static void _onDataReady(void)
{
    s16 x, y, z;
    s16 zDiff;

    ACC_ReadAcc(BKL_FIFO_LENGTH, &x, &y, &z);
//    printf("X:%hd Y:%hd Z:%hd\r\n", x, y, z);

    /* Z axis calculate */
    zDiff = z - g_lastZ;
    /* zDiff is positive means we are decelerate */
    if (zDiff >= 20){
        BKL_LED_DECELERATION();
    } 
    /* zDiff is negative means we are accelerate */
    else if (zDiff < -15){
        BKL_LED_ACCELERATION();
    }
    else 
    {
        BKL_LED_CONSTANTSPEED();
    }
    g_lastZ = z;

    /* Y axis calculate */
    if (y > 20){
        BKL_LED_TURNRIGHT();
    } else if (y < -20){
        BKL_LED_TURNLEFT();
    } else {
        BKL_LED_STRAIGHTAHEAD();
    }
    
    return;
}

static void _BKLIRQHandler(u8 irq)
{
    /* if data is ready */
    if (irq & XL345_WATERMARK){
        _onDataReady();
    }

    /* wake up */
    if (irq & XL345_ACTIVITY) {
        printf("Activity.\r\n");    
        //_bklTimerInit();
        PWR_Restore();        
    }

    /* into low power mode */
    if (irq & XL345_INACTIVITY){
        printf("Inactivity.\r\n");
        //_bklTimerDeinit();
        PWR_LowPower();
    }
    
    return;
}

static void _BKLInitAccSensor(void)
{
    u8 buffer[8];
    /* add software reset */
	buffer[0] = XL345_SOFT_RESET;
	xl345Write(1, XL345_RESERVED1, buffer);

	delay_ms(50);

    /*--------------------------------------------------
    activity - inactivity 
    --------------------------------------------------*/
    /* set up a buffer with all the initialization for activity and inactivity */
    buffer[0] = 20; /* THRESH_ACT = 80/16 = 5 Gee (1 lsb = 1/16 gee) */
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
                XL345_RATE_200 | XL345_LOW_NOISE;
    buffer[1] =                   /* POWER_CTL */
                XL345_ACT_INACT_SERIAL | XL345_AUTO_SLEEP |
                XL345_WAKEUP_8HZ | XL345_MEASURE;
    xl345Write(2, XL345_BW_RATE, buffer);

    // set the FIFO control
    buffer[0] = XL345_FIFO_MODE_FIFO    | // set FIFO mode
            	0 | 		             // link to INT1
            	BKL_FIFO_LENGTH;		 // number of samples
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

    return;
}

static void _BKLDeinitAccSensor(void)
{
	u8 reset = XL345_SOFT_RESET;

    /* just reset */
	xl345Write(1, XL345_RESERVED1, &reset);
	delay_ms(50);

    return;
}

static void _BKL_Start(void)
{
    _bklTimerInit();

    /* register irq handler to acc module */
    ACC_SetIRQHandler(_BKLIRQHandler);
    
    _BKLInitAccSensor();

    return;
}

static void _BKL_Stop(void)
{
    _BKLDeinitAccSensor();
    
    _bklTimerDeinit();
    
    LED_SetColor(0x00);
    
    return;
}

