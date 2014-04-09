#include <stdio.h>
#include "stm32f10x.h"
#include "utils.h"
#include "alarm.h"
#include "led.h"
#include "acc_sensor.h"
#include "app_intf.h"
#include "xl345.h"

static void _ALARM_Start(void);
static void _ALARM_Stop(void);
static void _ALARM_Periodic(void);
struct APP_INTF g_appAlarm =
{
	_ALARM_Start,
    _ALARM_Stop,
	_ALARM_Periodic
};

#define ALARM_TIMER          TIM4
#define ALARM_TIM_RCC        RCC_APB1Periph_TIM4
#define ALARM_TIM_IRQ        TIM4_IRQn
#define ALARM_TIM_ROUTINE    TIM4_IRQHandler

#define ALARM_TIMER_DELAY      60
#define ALARM_PATTERN_LENGTH   64

static u32 g_alarmStatic[ALARM_PATTERN_LENGTH] = 
{
	0x00000100, 0x00000200, 0x00000300, 0x00000500, 0x00000700, 0x00000900, 0x00000B00, 0x00000D00,
	0x00000F10, 0x00000D20, 0x00000B30, 0x00000950, 0x00000570, 0x00000390, 0x000002B0, 0x000001D0,
	0x000000F0, 0x000000D0, 0x000000B0, 0x00000090, 0x00000050, 0x00000030, 0x00000020, 0x00000010,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static u32 g_alarmCaution1[ALARM_PATTERN_LENGTH] = 
{0};

static u32 g_alarmCaution2[ALARM_PATTERN_LENGTH] = 
{0};

static u32 *g_curBuf;
static u32 g_curPos;

static void _initAlarmTimer(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(ALARM_TIM_RCC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = ALARM_TIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(ALARM_TIMER);	
	TIM_TimeBaseStructure.TIM_Period = ALARM_TIMER_DELAY;	
	TIM_TimeBaseStructure.TIM_Prescaler = 8000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(ALARM_TIMER, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(ALARM_TIMER, ENABLE);
	TIM_SetCounter(ALARM_TIMER, ALARM_TIMER_DELAY);

	TIM_Cmd(ALARM_TIMER, ENABLE);
	TIM_ITConfig(ALARM_TIMER, TIM_IT_Update, ENABLE);
	
	return;
}

static void _deinitAlarmTimer(void)
{
	TIM_Cmd(ALARM_TIMER, DISABLE);
	TIM_ITConfig(ALARM_TIMER, TIM_IT_Update, DISABLE);
	RCC_APB1PeriphClockCmd(ALARM_TIM_RCC, DISABLE);
    return;
}

static void _enterStatic(void)
{
    g_curBuf = g_alarmStatic;
    g_curPos = 0;
    return;
}

static void _enterCaution1(void)
{
    g_curBuf = g_alarmCaution1;
    g_curPos = 0;
    return;
}

static void _enterCatuion2(void)
{
    g_curBuf = g_alarmCaution2;   
    g_curPos = 0;
    return;
}

static void _alarmIRQHandler(u8 irq)
{
    /* into low power mode */
    if (irq & XL345_SINGLETAP){
        printf("Single Tap.\r\n");                
    }

	return;
}

static void _setupAccSensor(void)
{

	return;
}

static void _ALARM_Start(void)
{
	//set irq handler
	ACC_SetIRQHandler(_alarmIRQHandler);

	_setupAccSensor();

    _enterStatic();  

	_initAlarmTimer();

    return;
}

static void _ALARM_Stop(void)
{
    _deinitAlarmTimer();
    LED_SetColor(0x00);
    return;
}

static void _ALARM_Periodic(void)
{

}

void ALARM_TIM_ROUTINE(void)
{
	if (TIM_GetITStatus(ALARM_TIMER, TIM_IT_Update) != RESET){
     
		TIM_ClearITPendingBit(ALARM_TIMER, TIM_IT_Update);
        
        LED_SetColor(g_curBuf[g_curPos++]);
        if (g_curPos >= ALARM_PATTERN_LENGTH){
        	g_curPos = 0;
        }
    }

    return;
}

