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
#include "xl345.h"

int main()
{
	HW_CommonInit();
	HW_InitSysTick();
	USARTIO_InitUSART1();
  	LED_Init();    
    STATE_Init();
	BTN_Init();

	printf("Init finished.\r\n");

	while(1){}
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

