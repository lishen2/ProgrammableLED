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
struct APP_INTF g_appAlarm =
{
	_ALARM_Start,
    _ALARM_Stop
};

#define ALARM_TIMER          TIM4
#define ALARM_TIM_RCC        RCC_APB1Periph_TIM4
#define ALARM_TIM_IRQ        TIM4_IRQn
#define ALARM_TIM_ROUTINE    TIM4_IRQHandler

#define ALARM_TIMER_DELAY      30
#define ALARM_TIMER_SLEEP      5000
#define ALARM_PATTERN_LENGTH   32

static u32 g_displayBuffer[ALARM_PATTERN_LENGTH] = 
{
	0x00100100, 0x00200200, 0x00300300, 0x00400400, 
    0x00500500, 0x00600600, 0x00700700, 0x00800800,
	0x00900900, 0x00A00A00, 0x00B00B00, 0x00C00C00, 
	0x00D00D00, 0x00E00E00, 0x00F00F00, 0x00F00F00,
	0x00E00E00, 0x00C00C00, 0x00B00B00, 0x00A00A00, 
	0x00900900, 0x00800800, 0x00700700, 0x00600600,
	0x00500500, 0x00400400, 0x00300300, 0x00200200, 
	0x00100100, 0x00000000, 0x00000000, 0x00000000,
};
static u32 g_bufferPos;

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

static void _ALARM_Start(void)
{
    g_bufferPos = 0;
	_initAlarmTimer();
    
    return;
}

static void _ALARM_Stop(void)
{
    _deinitAlarmTimer();
    LED_SetColor(0x00);
    return;
}

static void _Sleep2S(void)
{
    TIM_SetCounter(ALARM_TIMER, ALARM_TIMER_SLEEP);
    return;
}

void ALARM_TIM_ROUTINE(void)
{
	if (TIM_GetITStatus(ALARM_TIMER, TIM_IT_Update) != RESET){
     
		TIM_ClearITPendingBit(ALARM_TIMER, TIM_IT_Update);
        
        LED_SetColor(g_displayBuffer[g_bufferPos++]);
        if (g_bufferPos >= ALARM_PATTERN_LENGTH){
        	g_bufferPos = 0;
            _Sleep2S();
        }
    }

    return;
}

