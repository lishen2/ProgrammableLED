#include <stdio.h>
#include "stm32f10x.h"
#include "utils.h"
#include "led.h"
#include "xl345.h"
#include "power.h"

static vs32 g_isPowerOff = BOOL_FALSE;

void PWR_LowPower(void)
{   
	LED_SetColor(0x00);
	
    LED_PowerOff();

    /* close all debug port */
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    g_isPowerOff = BOOL_TRUE;

    /* enter stop mode */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    
    return;
}

void PWR_Restore(void)
{        
    LED_PowerOn();
    
    g_isPowerOff = BOOL_FALSE;

    return;
}

int PWR_IsLPMode(void)
{
    return g_isPowerOff;    
}

