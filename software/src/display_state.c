#include "stm32f10x.h"
#include "utils.h"
#include "display_state.h"
#include "app_intf.h"
#include "break_light.h"
#include "alarm.h"

static vs32 g_DisState;
static u32 g_StateCount;

void STATE_SetState(int state)
{
    if (state == g_DisState){
        return;
    }

    //stop old display mode
    switch(g_DisState){
        case STATE_BREAK_LIGHT:
        {
            g_appBreakLight.Stop();            
            break;
        }
        case STATE_ALARM:
        {
			g_appAlarm.Stop();
            break;
        }        
    };

    //start new display mode
  	g_DisState = state;
    switch(state){
        case STATE_BREAK_LIGHT:
        {
            g_appBreakLight.Start();
            break;
        }
        case STATE_ALARM:
        {
            g_appAlarm.Start();
            break;
        }        
    };

	return;
}

void STATE_NextState(void)
{
    g_StateCount++;
    
	if (STATE_MAX == g_StateCount){
		g_StateCount = STATE_BREAK_LIGHT;
	}

	STATE_SetState(g_StateCount);

	return;
}

void STATE_Poll(void)
{
    switch(g_DisState){
        case STATE_BREAK_LIGHT:
        {
            g_appBreakLight.Periodic();            
            break;
        }
        case STATE_ALARM:
        {
			g_appAlarm.Periodic();
            break;
        }        
    };
}

void STATE_Init(void)
{
  	g_DisState = STATE_BREAK_LIGHT; 
	g_StateCount = STATE_BREAK_LIGHT;     
    g_appBreakLight.Start();

	return;
}

