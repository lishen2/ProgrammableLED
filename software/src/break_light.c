#include <stdlib.h>
#include "stm32f10x.h"
#include "utils.h"
#include "break_light.h"
#include "xl345.h"
#include "led.h"
#include "app_intf.h"
#include "acc_sensor.h"
#include "power.h"

enum BKL_Status{
    BKL_STATUS_CONSTANTSPEED,
    BKL_STATUS_DECELERATION,
};

#define BKL_TIMER          TIM3
#define BKL_TIM_RCC        RCC_APB1Periph_TIM3
#define BKL_TIM_IRQ        TIM3_IRQn
#define BKL_TIM_ROUTINE    TIM3_IRQHandler
#define BKL_TIMER_DELAY    400

#define BKL_FIFO_LENGTH    8

#define BKL_LED_DECELERATION()  LED_SetColor(0xFF00F00F);
#define BKL_LED_CONSTANTSPEED() LED_SetColor(0x00011011);

static s32 g_totalZ = 0;
static s32 g_totalX = 0;
static s32 g_totalY = 0;
static s32 g_count = 0;

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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(BKL_TIMER);	
	TIM_TimeBaseStructure.TIM_Prescaler = 8000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(BKL_TIMER, &TIM_TimeBaseStructure);

    TIM_ClearFlag(BKL_TIMER, TIM_FLAG_Update);  
    TIM_ITConfig(BKL_TIMER, TIM_IT_Update, ENABLE); 
    
    return;
}

static void _bklTimerDeinit(void)
{
	NVIC_InitTypeDef         NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = BKL_TIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(BKL_TIMER);	
	TIM_Cmd(BKL_TIMER, DISABLE);
	TIM_ITConfig(BKL_TIMER, TIM_IT_Update, DISABLE);    
	RCC_APB1PeriphClockCmd(BKL_TIM_RCC, DISABLE);  
    
    return;
}

static inline void _displayDeceleration(void)
{
    BKL_LED_DECELERATION();

	TIM_SetCounter(BKL_TIMER, BKL_TIMER_DELAY);
	TIM_Cmd(BKL_TIMER, ENABLE);

    return;
}

void BKL_TIM_ROUTINE(void)
{
    if(TIM_GetITStatus(BKL_TIMER, TIM_IT_Update) != RESET){
       TIM_ClearITPendingBit(BKL_TIMER , TIM_FLAG_Update);

       	TIM_Cmd(BKL_TIMER, DISABLE);
        BKL_LED_CONSTANTSPEED();
    }

	return;
}

static void _BKLonDataReady(void)
{
    s16 x, y, z;
	s16 zDiff, xDiff, yDiff;

    ACC_ReadAcc(BKL_FIFO_LENGTH, &x, &y, &z);

	g_count += 1;
	g_totalZ += z; 
	zDiff = z - g_totalZ/g_count;
	g_totalX += x;
	xDiff = abs(x - g_totalX/g_count);
	g_totalY += y;
	yDiff = abs(y - g_totalY/g_count);

//	printf("X:%hd,Y:%hd,Z:%hd\r\n", x, y, z);
//	printf("Xd:%hd,Yd:%hd,Zd:%hd\r\n", xDiff, yDiff, zDiff);

    /* zDiff is positive means we are decelerate */
    if ((zDiff >= 20 && xDiff < 30 && yDiff < 30) || 
		zDiff >= 250){
		_displayDeceleration();		        
    }

	if (g_count > 512){
		g_count >>= 3;
		g_totalZ >>= 3;
		g_totalX >>= 3;
		g_totalY >>= 3;
	}

    return;
}

static void _BKLIRQHandler(u8 irq)
{
    /* if data is ready */
    if (irq & XL345_WATERMARK){
        _BKLonDataReady();
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

static void _BKLInitAccSensor(void)
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
    buffer[0] = 20; /* THRESH_ACT */
    buffer[1] = 5; /* THRESH_INACT */
    buffer[2] = 20;/* TIME_INACT (seconds) */
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
            	BKL_FIFO_LENGTH;		 // number of samples
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

static void _BKLDeinitAccSensor(void)
{
	u8 reset = XL345_SOFT_RESET;

    /* just reset */
	XL345_Write(1, XL345_RESERVED1, &reset);
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

