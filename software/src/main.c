#include "stm32f10x.h"
#include "utils.h"
#include "acc_sensor.h"
#include "display_state.h"
#include "button.h"
#include "led.h"
#include "xl345.h"
#include "power.h"

int main()
{
	HW_CommonInit();
    PWR_SetUnusedPin2IPD();
  	LED_Init();
	HW_InitSysTick();  
	ACC_Init();  
    STATE_Init();
	BTN_Init();

	while(1){
        PWR_EnterSLEEPMode();
	}
}

