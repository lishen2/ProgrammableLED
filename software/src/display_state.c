#include "stm32f10x.h"
#include "utils.h"
#include "led.h"
#include "display_state.h"
#include "sequence.h"

static int g_DisState = STATE_GRADIENTER;

void STATE_SetState(int state)
{
	g_DisState = state;

	switch(state)
	{
	    case STATE_GRADIENTER:
        {
            SEQ_DisableDisplay();
			break;
        }
		case STATE_BICYCLE_LIGHT:
		case STATE_LEFT:
		case STATE_RIGHT:
		{
            SEQ_SetCurMode(state);
			SEQ_EnableDisplay();
			break;
		}
	};

	return;
}

void STATE_NextState(void)
{
	if (STATE_MAX == g_DisState + 1){
		g_DisState = STATE_GRADIENTER;
	} else {
		g_DisState++;
	}

	return;
}

