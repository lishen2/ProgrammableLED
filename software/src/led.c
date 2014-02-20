#include "stm32f10x.h"
#include "led.h"

#define LED_1_GPIO_PORT  GPIOB
#define LED_1_GPIO_RCC   RCC_APB2Periph_GPIOB

#define LED_1_R        GPIO_Pin_14
#define LED_1_G		   GPIO_Pin_13
#define LED_1_B		   GPIO_Pin_15
#define LED_1_ALL      (LED_1_R|LED_1_G|LED_1_B)

#define LED_2_GPIO_PORT  GPIOA
#define LED_2_GPIO_RCC   RCC_APB2Periph_GPIOA

#define LED_2_R		   GPIO_Pin_4 
#define LED_2_G		   GPIO_Pin_3 
#define LED_2_B		   GPIO_Pin_5
#define LED_2_ALL      (LED_2_R|LED_2_G|LED_2_B)

void LED_Init(void)
{
	/*LED管脚初始化*/  
	GPIO_InitTypeDef GPIO_InitStructure; 

	//设置彩色LED
	RCC_APB2PeriphClockCmd(LED_1_GPIO_RCC | LED_2_GPIO_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED_1_ALL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(LED_1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_2_ALL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(LED_2_GPIO_PORT, &GPIO_InitStructure);

	GPIO_SetBits(LED_1_GPIO_PORT, LED_1_ALL);	
	GPIO_SetBits(LED_2_GPIO_PORT, LED_2_ALL);	

	return;
}

void LED_LightupAll(void)
{
    GPIO_ResetBits(LED_1_GPIO_PORT, LED_1_ALL);
	GPIO_ResetBits(LED_2_GPIO_PORT, LED_2_ALL);
	return;
}

void LED_ShutdownAll(void)
{
    GPIO_SetBits(LED_1_GPIO_PORT, LED_1_ALL);
	GPIO_SetBits(LED_2_GPIO_PORT, LED_2_ALL);	 
	return;
}

void LED_LightControl(int led, int act)
{
	GPIO_TypeDef *port;
	u16 pin;

	switch(led)
	{
		case LED1_RED:
		{
			port = LED_1_GPIO_PORT;
			pin = LED_1_R;
			break;
		}
		case LED1_GREEN:
		{
			port = LED_1_GPIO_PORT;
			pin = LED_1_G;
			break;
		}
		case LED1_BLUE:
		{
			port = LED_1_GPIO_PORT;
			pin = LED_1_B;
			break;
		}
		case LED2_RED:
		{
			port = LED_2_GPIO_PORT;
			pin = LED_2_G;
			break;
		}
		case LED2_GREEN:
		{
			port = LED_2_GPIO_PORT;
			pin = LED_2_G;
			break;
		}
		case LED2_BLUE:
		{
			port = LED_2_GPIO_PORT;
			pin = LED_2_B;
			break;
		}
		default:
		{
			return;
		}
	}

	if (LED_LIGHTUP == act){
		GPIO_ResetBits(port, pin);
	} else {
		GPIO_SetBits(port, pin);
	}

	return;
}

