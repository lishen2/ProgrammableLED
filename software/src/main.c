#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "utils.h"
#include "ringbuf.h"
#include "usart_io.h"
#include "nusb_intf.h"
#include "led.h"
#include "acc_sensor.h"

int main()
{
//	int led;
	u16 curZ, lastZ;

	HW_InitSysTick();
	USARTIO_InitUSART1();
	NUSB_init();
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
  	LED_Init();
    LED_SetColor(0x0111, 0x0111);
    delay_10ms(200);

	while(1){
        LED_SetColor(0x0500, 0x0050);    
        delay_10ms(50);
        LED_SetColor(0x0050, 0x0005);    
        delay_10ms(50);
        LED_SetColor(0x0005, 0x0500);    
        delay_10ms(50);     
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

