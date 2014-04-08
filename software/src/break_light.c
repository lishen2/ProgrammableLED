#include "stm32f10x.h"
#include "utils.h"
#include "break_light.h"
#include "led.h"

#define BREAK_LIGHT_TIMER          TIM3
#define BREAK_LIGHT_TIM_RCC        RCC_APB1Periph_TIM3
#define BREAK_LIGHT_TIM_IRQ        TIM3_IRQn
#define BREAK_LIGHT_TIM_ROUTINE    TIM3_IRQHandler

#define BREAK_LIGHT_DEFAULT_DELAYTIME  60

static void _initTimer(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(BREAK_LIGHT_TIM_RCC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = BREAK_LIGHT_TIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(BREAK_LIGHT_TIMER);	
	TIM_TimeBaseStructure.TIM_Period = BREAK_LIGHT_DEFAULT_DELAYTIME;	
	TIM_TimeBaseStructure.TIM_Prescaler = 8000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(BREAK_LIGHT_TIMER, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(BREAK_LIGHT_TIMER, ENABLE);
	TIM_SetCounter(BREAK_LIGHT_TIMER, BREAK_LIGHT_DEFAULT_DELAYTIME);

	TIM_Cmd(BREAK_LIGHT_TIMER, DISABLE);
	TIM_ITConfig(BREAK_LIGHT_TIMER, TIM_IT_Update, DISABLE);
	
	return;
}

void BKL_Start(void)
{
	TIM_Cmd(BREAK_LIGHT_TIMER, ENABLE);
	TIM_ITConfig(BREAK_LIGHT_TIMER, TIM_IT_Update, ENABLE);
    return;
}

void BKL_Stop(void)
{
	TIM_Cmd(BREAK_LIGHT_TIMER, DISABLE);
	TIM_ITConfig(BREAK_LIGHT_TIMER, TIM_IT_Update, DISABLE);
    return;
}

void BKL_OnData(u16 x, u16 y, u16 z)
{
    return;
}

