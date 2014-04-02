#include "stm32f10x.h"
#include "utils.h"
#include "button.h"
#include "display_state.h"

#define BTN_BUTTON_PIN         GPIO_Pin_2
#define BTN_BUTTON_PORT        GPIOB 
#define BTN_BUTTON_RCC         RCC_APB2Periph_GPIOB
#define BTN_BUTTON_PORTSOURCE  GPIO_PortSourceGPIOB
#define BTN_BUTTON_PINSOURCE   GPIO_PinSource2
#define BTN_BUTTON_IRQ 		   EXTI2_IRQn
#define BTN_BUTTON_IRQROUTINE  EXTI2_IRQHandler
#define BTN_BUTTON_EXTILINE    EXTI_Line2

#define BTN_ANTISHAKE_TIMER      TIM2
#define BTN_ANTISHAKE_RCC        RCC_APB1Periph_TIM2
#define BTN_ANTISHAKE_IRQ        TIM2_IRQn
#define BTN_ANTISHAKE_IRQROUTINE TIM2_IRQHandler

// unit ms
#define BTN_ANTISHAEK_TIME       50

static void _initTimer(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(BTN_ANTISHAKE_RCC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = BTN_ANTISHAKE_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(BTN_ANTISHAKE_TIMER);	
	TIM_TimeBaseStructure.TIM_Period = BTN_ANTISHAEK_TIME;	
	TIM_TimeBaseStructure.TIM_Prescaler = 8000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(BTN_ANTISHAKE_TIMER, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(BTN_ANTISHAKE_TIMER, DISABLE);
	TIM_SetCounter(BTN_ANTISHAKE_TIMER, BTN_ANTISHAEK_TIME);
	TIM_Cmd(BTN_ANTISHAKE_TIMER, DISABLE);

	TIM_ITConfig(BTN_ANTISHAKE_TIMER, TIM_IT_Update, ENABLE);
	
	return;
}	  

static void _initButton(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(BTN_BUTTON_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = BTN_BUTTON_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BTN_BUTTON_PORT, &GPIO_InitStructure);	
	GPIO_EXTILineConfig(BTN_BUTTON_PORTSOURCE, BTN_BUTTON_PINSOURCE); 

    NVIC_InitStructure.NVIC_IRQChannel = BTN_BUTTON_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	EXTI_InitStructure.EXTI_Line = BTN_BUTTON_EXTILINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	return;
}

void BTN_Init(void)
{
	_initTimer();
	_initButton();
	return;
}

void BTN_BUTTON_IRQROUTINE(void)
{
	if (SET == EXTI_GetFlagStatus(BTN_BUTTON_EXTILINE)){
		EXTI_ClearFlag(BTN_BUTTON_EXTILINE);
	
		//start timer	
		TIM_SetCounter(BTN_ANTISHAKE_TIMER, BTN_ANTISHAEK_TIME);
		TIM_Cmd(BTN_ANTISHAKE_TIMER, ENABLE);
	}	
}

void BTN_ANTISHAKE_IRQROUTINE(void)
{
	if(TIM_GetITStatus(BTN_ANTISHAKE_TIMER, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(BTN_ANTISHAKE_TIMER, TIM_IT_Update);

		TIM_Cmd(BTN_ANTISHAKE_TIMER, DISABLE);

		//check if the button is still pushed
		if (Bit_RESET == GPIO_ReadInputDataBit(BTN_BUTTON_PORT, BTN_BUTTON_PIN))
		{
			STATE_NextState();
		}
	}//if	
}


