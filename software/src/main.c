#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "utils.h"
#include "ringbuf.h"
#include "usart_io.h"
#include "led.h"
#include "acc_sensor.h"
#include "display_state.h"
#include "button.h"
#include "sequence.h"
#include "command.h"

int main()
{
	HW_InitSysTick();
	USARTIO_InitUSART1();;
	ACC_Init();

	printf("Init finished.\r\n");

	//BTN_Init();
  	LED_Init();
    //SEQ_Init();

	while(1){
		//CMD_HandleCommand();
		LED_SetColor(0x0000000F);
		delay_ms(1000);
		LED_SetColor(0x000000F0);
		delay_ms(1000);
		LED_SetColor(0x00000F00);
		delay_ms(1000);
		LED_SetColor(0x0000F000);
		delay_ms(1000);
		LED_SetColor(0x000F0000);
		delay_ms(1000);
		LED_SetColor(0x00F00000);
		delay_ms(1000);
	}
}

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
	USARTIO_SendChar(USART1, ch);
	return(ch);
}

