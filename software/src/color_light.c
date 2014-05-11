#include <stdlib.h>
#include "stm32f10x.h"
#include "utils.h"
#include "app_intf.h"
#include "color_light.h"
#include "xl345.h"
#include "power.h"
#include "led.h"
#include "acc_sensor.h"

#define COLOR_FIFO_LENGTH    8
#define COLOR_TIMER          TIM4
#define COLOR_TIM_RCC        RCC_APB1Periph_TIM4
#define COLOR_TIM_IRQ        TIM4_IRQn
#define COLOR_TIM_ROUTINE    TIM4_IRQHandler
#define COLOR_TIMER_DELAY    50

static void _COLOR_Start(void);
static void _COLOR_Stop(void);
struct APP_INTF g_appColorLight =
{
	_COLOR_Start,
    _COLOR_Stop
};
/*
static u32 g_pattern[] = {
	0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007,
	0x00000008, 0x00000009, 0x0000000A, 0x0000000B, 0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F,
	0x0000000F, 0x0000001E, 0x0000002D, 0x0000003C, 0x0000004B, 0x0000005A, 0x00000069, 0x00000078,
	0x00000087, 0x00000096, 0x000000A5, 0x000000B4, 0x000000C3, 0x000000D2, 0x000000E1, 0x000000F0,
	0x000000F0, 0x000001E0, 0x000002D0, 0x000003C0, 0x000004B0, 0x000005A0, 0x00000690, 0x00000780,
	0x00000870, 0x00000960, 0x00000A50, 0x00000B40, 0x00000C30, 0x00000D20, 0x00000E10, 0x00000F00,
	0x00000F00, 0x00001E00, 0x00002D00, 0x00003C00, 0x00004B00, 0x00005A00, 0x00006900, 0x00007800,
	0x00008700, 0x00009600, 0x0000A500, 0x0000B400, 0x0000C300, 0x0000D200, 0x0000E100, 0x0000F000,
	0x0000F000, 0x0001E000, 0x0002D000, 0x0003C000, 0x0004B000, 0x0005A000, 0x00069000, 0x00078000,
	0x00087000, 0x00096000, 0x000A5000, 0x000B4000, 0x000C3000, 0x000D2000, 0x000E1000, 0x000F0000,
	0x000F0000, 0x000E0000, 0x000D0000, 0x000C0000, 0x000B0000, 0x000A0000, 0x00090000, 0x00080000,
	0x00070000, 0x00060000, 0x00050000, 0x00040000, 0x00030000, 0x00020000, 0x00010000, 0x00000000 
};*/
static u32 g_pattern[] = {
	0xF0000000, 0xE0000001, 0xD0000002, 0xC0000003, 0xB0000004, 0xA0000005, 0x90000006, 0x80000007,
	0x70000008, 0x60000009, 0x5000000A, 0x4000000B, 0x3000000C, 0x2000000D, 0x1000000E, 0x0000000F,
	0x0000000F, 0x0000001E, 0x0000002D, 0x0000003C, 0x0000004B, 0x0000005A, 0x00000069, 0x00000078,
	0x00000087, 0x00000096, 0x000000A5, 0x000000B4, 0x000000C3, 0x000000D2, 0x000000E1, 0x000000F0,
	0x000000F0, 0x000001E0, 0x000002D0, 0x000003C0, 0x000004B0, 0x000005A0, 0x00000690, 0x00000780,
	0x00000870, 0x00000960, 0x00000A50, 0x00000B40, 0x00000C30, 0x00000D20, 0x00000E10, 0x00000F00,
	0x00000F00, 0x00001E00, 0x00002D00, 0x00003C00, 0x00004B00, 0x00005A00, 0x00006900, 0x00007800,
	0x00008700, 0x00009600, 0x0000A500, 0x0000B400, 0x0000C300, 0x0000D200, 0x0000E100, 0x0000F000,
	0x0000F000, 0x0001E000, 0x0002D000, 0x0003C000, 0x0004B000, 0x0005A000, 0x00069000, 0x00078000,
	0x00087000, 0x00096000, 0x000A5000, 0x000B4000, 0x000C3000, 0x000D2000, 0x000E1000, 0x000F0000,
	0x000F0000, 0x001E0000, 0x002D0000, 0x003C0000, 0x004B0000, 0x005A0000, 0x00690000, 0x00780000,
	0x00870000, 0x00960000, 0x00A50000, 0x00B40000, 0x00C30000, 0x00D20000, 0x00E10000, 0x00F00000, 
	0x00F00000, 0x01E00000, 0x02D00000, 0x03C00000, 0x04B00000, 0x05A00000, 0x06900000, 0x07800000, 
	0x08700000, 0x09600000, 0x0A500000, 0x0B400000, 0x0C300000, 0x0D200000, 0x0E100000, 0x0F000000, 
	0x0F000000, 0x1E000000, 0x2D000000, 0x3C000000, 0x4B000000, 0x5A000000, 0x69000000, 0x78000000, 
	0x87000000, 0x96000000, 0xA5000000, 0xB4000000, 0xC3000000, 0xD2000000, 0xE1000000, 0xF0000000 
};
static u32 g_curIdx = 0;

static void _colorInitAccSensor(void)
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
    buffer[2] = 8;/* TIME_INACT (seconds) */
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
            	COLOR_FIFO_LENGTH;		 // number of samples
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

static void _colorDeinitAccSensor(void)
{
	u8 reset = XL345_SOFT_RESET;

    /* just reset */
	XL345_Write(1, XL345_RESERVED1, &reset);
	delay_ms(50);

    return;
}

static void _colorOnDataReady(void)
{
    s16 x, y, z;
	u32 delta;

    ACC_ReadAcc(COLOR_FIFO_LENGTH, &x, &y, &z);

	delta = COLOR_TIMER_DELAY + (y/10);
	if (delta < 5 ){
		delta = 5;
	}

	TIM_SetAutoreload(COLOR_TIMER, delta);

    return;
} 

void _colorIRQHandler(u8 irq)
{
    /* if data is ready */
    if (irq & XL345_WATERMARK){
        _colorOnDataReady();
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

static void _colorTimerInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(COLOR_TIM_RCC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = COLOR_TIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(COLOR_TIMER);	
	TIM_TimeBaseStructure.TIM_Prescaler = 8000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseStructure.TIM_Period = COLOR_TIMER_DELAY;
	TIM_TimeBaseInit(COLOR_TIMER, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(COLOR_TIMER, ENABLE);//½ûÖ¹ARRÔ¤×°ÔØ»º³åÆ÷

    TIM_ClearFlag(COLOR_TIMER, TIM_FLAG_Update);  
    TIM_ITConfig(COLOR_TIMER, TIM_IT_Update, ENABLE); 
    				   
	TIM_SetCounter(COLOR_TIMER, COLOR_TIMER_DELAY);
	TIM_Cmd(COLOR_TIMER, ENABLE);

    return;
}

static void _colorTimerDeinit(void)
{
	NVIC_InitTypeDef         NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = COLOR_TIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(COLOR_TIMER);	
	TIM_Cmd(COLOR_TIMER, DISABLE);
	TIM_ITConfig(COLOR_TIMER, TIM_IT_Update, DISABLE);    
	RCC_APB1PeriphClockCmd(COLOR_TIM_RCC, DISABLE);  
    
    return;
}

void COLOR_TIM_ROUTINE(void)
{
    if(TIM_GetITStatus(COLOR_TIMER, TIM_IT_Update) != RESET){
       TIM_ClearITPendingBit(COLOR_TIMER , TIM_FLAG_Update);

		g_curIdx++;
		if (g_curIdx >= (sizeof(g_pattern)>>2)){
			g_curIdx = 0;
		}

		LED_SetColor(g_pattern[g_curIdx]);
    }

	return;
}

static void _COLOR_Start(void)
{
    /* register irq handler to acc module */
    ACC_SetIRQHandler(_colorIRQHandler);

	_colorTimerInit();

	_colorInitAccSensor();


	return;
}

static void _COLOR_Stop(void)
{
	_colorDeinitAccSensor();

	_colorTimerDeinit();

	LED_SetColor(0x00);

	return;
}


