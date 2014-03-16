#include "stm32f10x.h"
#include "utils.h"
#include "led.h"
#include "display_state.h"

enum DISPLAY_STATE
{
	STATE_INIT,
	STATE_GRADIENTER,
	STATE_BICYCLE_LIGHT,
	STATE_POLICE,
	STATE_SOS,
	STATE_FREEFALL,
	STATE_END,
};

static int g_DisState = STATE_INIT;

void STATE_Change(void)
{
	if (STATE_END == g_DisState + 1){
		g_DisState = STATE_INIT;
	} else {
		g_DisState++;
	}

	return;
}

static void _state_init(void)
{
	static int counter = 0;
	u16 color;
	u8 bright;

	if (counter < 16){
		bright = counter;
		color = bright | bright << 4 | bright << 8;
	} else if (counter >= 16 && counter <= 31){
		bright = 31 - counter;
		color = bright | bright << 4 | bright << 8;
	} else if (counter > 31){
		color = 0;
	}

	LED_SetColor(color, color);

	counter++;
	if (counter >= 140){
		counter = 0;
	}

	delay_ms(50);	

	return;
}

static void _state_gradienter(void)
{
	LED_SetColor(0x0111, 0x0111);
	delay_ms(10);	

	return;	
}

static void _state_bicycle(void)
{
	static int counter = 0;
	if (counter > 70){
		LED_SetColor(0x000F, 0x000F);
	}

	if (++counter > 100){
		counter = 0;
	}

	delay_ms(10);	
	LED_SetColor(0x0000, 0x0000);

	return;	
}

static void _state_police(void)
{
	static int counter = 0;
	static u16 color1 = 0x000F;
	static u16 color2 = 0x0000;

	/* lightup when counter is even */
	if (0 == counter % 2){
		LED_SetColor(color1, color2);
	} else {
		LED_SetColor(0x0000, 0x0000);
	}

	counter++;
	if (counter == 7){
		color1 = 0x0000;
		color2 = 0x0F00;
	} 
	if (counter >= 15){
		color1 = 0x000F;
		color2 = 0x0000;
		counter = 0;
	}

	delay_ms(50);	

	return;	
}

static void _state_sos(void)
{
	LED_SetColor(0x0001, 0x0000);
	delay_ms(10);	

	return;	
}
static void _state_freefall(void)
{
	LED_SetColor(0x0000, 0x0100);
	delay_ms(10);	

	return;	
}

void STATE_Poll(void)
{
	switch(g_DisState)
	{
		case STATE_INIT:
		{
			_state_init();
			break;
		}
		case STATE_GRADIENTER:
		{
			_state_gradienter();
			break;
		}
		case STATE_BICYCLE_LIGHT:
		{
			_state_bicycle();
			break;
		}
		case STATE_POLICE:
		{
			_state_police();
			break;
		}
		case STATE_SOS:
		{
			_state_sos();
			break;
		}
		case STATE_FREEFALL:
		{
			_state_freefall();
			break;
		}
		default:
		{
			break;
		}
	};

	return;
}

