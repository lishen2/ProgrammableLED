#include "stm32f10x.h"
#include "flash_pattern.h"
#include "led.h"
#include "utils.h"

void PAT_REDFLASH(u16 delay)
{
	LED_SetColor(0x000F, 0x000F);
	delay_ms(delay);
	LED_SetColor(0x000F, 0x000F);
	delay_ms(delay);
	
	return;	
}




