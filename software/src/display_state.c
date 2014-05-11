#include "stm32f10x.h"
#include "utils.h"
#include "display_state.h"
#include "app_intf.h"
#include "break_light.h"
#include "gradienter.h"
#include "color_light.h"

static vs32 g_DisState;

static void _dummy(void) {};
static struct APP_INTF g_dummyApp = {
	_dummy, _dummy
};
static struct APP_INTF* g_curApp = &g_dummyApp;

void STATE_SetState(enum DISPLAY_STATE state)
{
    /* stop old app */
    g_curApp->Stop();            

    /* set new state*/
    switch(state){
        case STATE_BREAK_LIGHT:
        {
            g_curApp = &g_appBreakLight;
            break;
        }
        case STATE_GRADIENTER:
        {
            g_curApp = &g_appGradienter;
            break;
        } 
		case STATE_COLORLIGHT:
		{
			g_curApp = &g_appColorLight;
			break;
		}
		default:
		{
			g_curApp = &g_dummyApp;
			break;
		}      
    };

	/* start new app */
	g_curApp->Start();

	return;
}

void STATE_NextState(void)
{
    g_DisState++;
    
	if (STATE_MAX == g_DisState){
		g_DisState = STATE_BREAK_LIGHT;
	}

	STATE_SetState((enum DISPLAY_STATE)g_DisState);

	return;
}

void STATE_Init(void)
{
  	g_DisState = STATE_BREAK_LIGHT; 
	g_curApp = &g_appBreakLight;  
    g_appBreakLight.Start();

	return;
}

