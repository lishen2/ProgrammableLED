#include "stm32f10x.h"
#include "sequence.h"
#include "display_state.h"
#include "utils.h"
#include "led.h"

#define SEQ_LED_DEFAYLT_DELAYTIME  60
#define SEQ_LED_SEQUENCE_LEN       94

static u32 g_bicycleSeq[SEQ_LED_SEQUENCE_LEN] = {

};

static u32 g_leftSeq[SEQ_LED_SEQUENCE_LEN] = {

};

static u32 g_rightSeq[SEQ_LED_SEQUENCE_LEN] = {

};

static u32* g_curBuf;
static u32 g_counter;

#define SEQ_DISPLAY_TIMER      TIM3
#define SEQ_DISPLAY_RCC        RCC_APB1Periph_TIM3
#define SEQ_DISPLAY_IRQ        TIM3_IRQn
#define SEQ_DISPLAY_IRQROUTINE TIM3_IRQHandler

static void _initTimer(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(SEQ_DISPLAY_RCC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = SEQ_DISPLAY_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(SEQ_DISPLAY_TIMER);	
	TIM_TimeBaseStructure.TIM_Period = SEQ_LED_DEFAYLT_DELAYTIME;	
	TIM_TimeBaseStructure.TIM_Prescaler = 8000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(SEQ_DISPLAY_TIMER, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(SEQ_DISPLAY_TIMER, ENABLE);
	TIM_SetCounter(SEQ_DISPLAY_TIMER, SEQ_LED_DEFAYLT_DELAYTIME);

	TIM_Cmd(SEQ_DISPLAY_TIMER, DISABLE);
	TIM_ITConfig(SEQ_DISPLAY_TIMER, TIM_IT_Update, DISABLE);
	
	return;
}	

void SEQ_Init(void)
{
    _initTimer();
    return;
}

void SEQ_SetDelayTime(u32 delay)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
	TIM_TimeBaseStructure.TIM_Period = delay;	
	TIM_TimeBaseStructure.TIM_Prescaler = 8000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(SEQ_DISPLAY_TIMER, &TIM_TimeBaseStructure);
    
    return;
}

void SEQ_SetCurMode(int mode)
{
	switch(mode)
	{
		case STATE_BICYCLE_LIGHT:
		{
			g_curBuf = g_bicycleSeq;
			break;
		}        
		case STATE_LEFT:
		{
			g_curBuf = g_leftSeq;
			break;
		}
		case STATE_RIGHT:
		{
			g_curBuf = g_rightSeq;
			break;
		}
	    case STATE_GRADIENTER:
        default: 
        {
            break;
        }
        
	};

    g_counter = 0;

    return;
}

void SEQ_EnableDisplay(void)
{
	TIM_Cmd(SEQ_DISPLAY_TIMER, ENABLE);
	TIM_ITConfig(SEQ_DISPLAY_TIMER, TIM_IT_Update, ENABLE);

    return;
}

void SEQ_DisableDisplay(void)
{
	TIM_Cmd(SEQ_DISPLAY_TIMER, DISABLE);
	TIM_ITConfig(SEQ_DISPLAY_TIMER, TIM_IT_Update, DISABLE);

    return;
}

void SEQ_DISPLAY_IRQROUTINE(void)
{
	if (TIM_GetITStatus(SEQ_DISPLAY_TIMER, TIM_IT_Update) != RESET){
     
		TIM_ClearITPendingBit(SEQ_DISPLAY_TIMER, TIM_IT_Update);
        
        LED_SetColor(g_curBuf[g_counter++]);
        if (g_counter >= SEQ_LED_SEQUENCE_LEN){
        	g_counter = 0;
        }
    }

	return;	
}

