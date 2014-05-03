#include "stm32f10x.h"
#include "utils.h"
#include "led.h"
#include "xl345.h"
#include "power.h"

void PWR_LowPower(void)
{   
	LED_SetColor(0x00);
	
    LED_PowerOff();

    /* close all debug port */
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    /* enter stop mode */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    
    return;
}

void PWR_Restore(void)
{        
    LED_PowerOn();
    
    return;
}

