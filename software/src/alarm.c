#include <stdlib.h>
#include "stm32f10x.h"
#include "utils.h"
#include "alarm.h"
#include "led.h"

#define ALARM_TIMER          TIM4
#define ALARM_TIM_RCC        RCC_APB1Periph_TIM4
#define ALARM_TIM_IRQ        TIM4_IRQn
#define ALARM_TIM_ROUTINE    TIM4_IRQHandler

#define ALARM_THRESHOLD_CAUTION1  300
#define ALARM_THRESHOLD_CAUTION2  1000

#define ALARM_TIMER_DELAY      60
#define ALARM_PATTERN_LENGTH   64

enum ALARM_STATE{
    ALARM_STATE_STATIC,
    ALARM_STATE_CAUTION1,
    ALARM_STATE_CAUTION2,    
};

static u32 g_alarmStatic[ALARM_PATTERN_LENGTH] = 
{0};

static u32 g_alarmCaution1[ALARM_PATTERN_LENGTH] = 
{0};

static u32 g_alarmCaution2[ALARM_PATTERN_LENGTH] = 
{0};

static u32 *g_curBuf;
static u32 g_curPos;

static int g_status = ALARM_STATE_STATIC;

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
    g_status = ALARM_STATE_STATIC;
    g_curBuf = g_alarmStatic;
    g_curPos = 0;
    return;
}

static void _enteryCaution1(void)
{
    g_status = ALARM_STATE_CAUTION1;
    g_curBuf = g_alarmCaution1;
    g_curPos = 0;
    return;
}

static void _entryCatuion2(void)
{
    g_status = ALARM_STATE_CAUTION2;
    g_curBuf = g_alarmCaution2;   
    g_curPos = 0;
    return;
}

void ALARM_Start(void)
{
    _enterStatic();
    _initAlarmTimer();  
    
    return;
}

void ALARM_Stop(void)
{
    _deinitAlarmTimer();
    LED_SetColor(0x00);
    return;
}

void ALARM_OnData(u16 x, u16 y, u16 z)
{
    u32 acc;

    acc = abs(x) + abs(y) + abs(z);
    
    if (acc >= ALARM_THRESHOLD_CAUTION2 && 
        ALARM_STATE_CAUTION2 != g_status){
        _entryCatuion2();
    } 
    else if (acc >= ALARM_THRESHOLD_CAUTION1 && 
               acc < ALARM_THRESHOLD_CAUTION2  && 
               ALARM_STATE_CAUTION1 != g_status){
        _enteryCaution1();
    } 
    else if (ALARM_STATE_STATIC != g_status){
        _enterStatic();
    }

    return;
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

