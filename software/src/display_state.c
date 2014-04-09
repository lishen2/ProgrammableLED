#include "stm32f10x.h"
#include "utils.h"
#include "led.h"
#include "display_state.h"
#include "break_light.h"
#include "alarm.h"

static vs32 g_DisState;

void STATE_SetState(int state)
{
    if (state == g_DisState){
        return;
    }

    //stop old display mode
    switch(g_DisState){
        case STATE_BREAK_LIGHT:
        {
            BKL_Stop();
            break;
        }
        case STATE_ALARM:
        {
            ALARM_Stop();
            break;
        }        
    };

    //start new display mode
  	g_DisState = state;
    switch(state){
        case STATE_BREAK_LIGHT:
        {
            BKL_Start();
            break;
        }
        case STATE_ALARM:
        {
            ALARM_Start();
            break;
        }        
    };

	return;
}

void STATE_NextState(void)
{
    g_DisState++;
    
	if (STATE_MAX == g_DisState){
		g_DisState = STATE_BREAK_LIGHT;
	}

	STATE_SetState(g_DisState);

	return;
}

void STATE_OnDataReady(u16 x, u16 y, u16 z)
{  
    switch(g_DisState)
    {
        case STATE_BREAK_LIGHT:
        {
            BKL_OnData(x, y, z);
            break;
        }
        case STATE_ALARM:
        {
            ALARM_OnData(x, y, z);
            break;
        }
    };
    
    return;
}

void STATE_Init(void)
{
  	g_DisState = STATE_BREAK_LIGHT;       
    BKL_Start();
}

