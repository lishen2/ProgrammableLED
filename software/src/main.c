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
	u8 buf[6];
    u16 x, y, z;

	HW_CommonInit();
	HW_InitSysTick();
	USARTIO_InitUSART1();;
	ACC_Init();

	printf("Init finished.\r\n");

	BTN_Init();
  	LED_Init();

#define ACC_DATAMASK_10BIT_JUST_RIGHT    0x83FF

	while(1){
        /* TODO, GOTO sleep */
        xl345Read(sizeof(buf), XL345_DATAX0, buf);
        x = (buf[1] << 8 | buf[0]) & ACC_DATAMASK_10BIT_JUST_RIGHT;
        y = (buf[3] << 8 | buf[2]) & ACC_DATAMASK_10BIT_JUST_RIGHT;        
        z = (buf[5] << 8 | buf[4]) & ACC_DATAMASK_10BIT_JUST_RIGHT; 

		printf("x:%hd, y:%hd, z:%hd\r\n", (s16)x, (s16)y, (s16)z);

		delay_ms(2000);
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

