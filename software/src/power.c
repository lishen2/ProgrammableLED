#include <stdio.h>
#include "stm32f10x.h"
#include "led.h"
#include "xl345.h"
#include "power.h"

void PWR_LowPower(void)
{
    LED_PowerOff();
    xl345PowerOff();

    /* close all debug port */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
    /* enter stop mode */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    
    return;
}

void PWR_Restore(void)
{
    /* open SWD */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    
    xl345PowerOn();
    LED_PowerOn();

    return;
}

