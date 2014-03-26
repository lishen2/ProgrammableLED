#include "stm32f10x.h"
#include "utils.h"
#include "led.h"
#include "display_state.h"
#include "sequence.h"

static int g_DisState = STATE_GRADIENTER;

void STATE_SetState(int state)
{
	 g_DisState = state;

	 return;
}

void STATE_Change(void)
{
	if (STATE_MAX == g_DisState + 1){
		g_DisState = STATE_GRADIENTER;
	} else {
		g_DisState++;
	}

	return;
}

static void _state_gradienter(void)
{
	LED_SetColor(0x00111111);
	delay_ms(50);	

	return;	
}

void STATE_Poll(void)
{
	switch(g_DisState)
	{
		case STATE_GRADIENTER:
		{
			_state_gradienter();
			break;
		}
		case STATE_BICYCLE_LIGHT:
		{
			SEQ_DisplayBicycle();
			break;
		}
		case STATE_LEFT:
		{
			SEQ_DisplayLeft();
			break;
		}
		case STATE_RIGHT:
		{
			SEQ_DisplayRight();
			break;
		}
		default:
		{
			break;
		}
	};

	return;
}

