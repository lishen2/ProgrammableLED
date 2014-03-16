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

int main()
{
	HW_InitSysTick();
	USARTIO_InitUSART1();;
	ACC_Init();


	printf("Init finished.\r\n");

/*
	curZ = lastZ = 0;
	while(1){
		curZ = ACC_ReadZ();
		if (abs(curZ - lastZ) > 2000){
			printf("%hu\r\n", curZ);
		}
		lastZ = curZ;
		
		delay_10ms(100); 
	}
*/
	BTN_Init();
  	LED_Init();

	LED_SetColor(0x0555, 0x0555);
	delay_ms(500);
	LED_SetColor(0, 0);

	while(1){
		STATE_Poll();
	}

    return 0;
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

