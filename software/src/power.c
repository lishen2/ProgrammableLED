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
    /* disable systick */
    SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
	
    LED_PowerOff();

    /* close all debug port */
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    g_isPowerOff = BOOL_TRUE;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
    /* enter stop mode */
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
    
    return;
}

void PWR_Restore(void)
{        
    LED_PowerOn();
    
    g_isPowerOff = BOOL_FALSE;

    /* enable systick */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    return;
}

int PWR_IsLPMode(void)
{
    return g_isPowerOff;    
}

