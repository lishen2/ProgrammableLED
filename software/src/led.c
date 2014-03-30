#include "stm32f10x.h"
#include "led.h"
#include "utils.h"

#define LED_1_PORT  GPIOB
#define LED_1_RCC   RCC_APB2Periph_GPIOB

#define LED_1_RED   GPIO_Pin_14
#define LED_1_GREEN GPIO_Pin_13
#define LED_1_BLUE	GPIO_Pin_15
#define LED_1_ALL   (LED_1_RED|LED_1_GREEN|LED_1_BLUE)

#define LED_2_PORT  GPIOA
#define LED_2_RCC   RCC_APB2Periph_GPIOA

#define LED_2_RED    GPIO_Pin_4 
#define LED_2_GREEN	 GPIO_Pin_3 
#define LED_2_BLUE	 GPIO_Pin_5
#define LED_2_ALL    (LED_2_RED|LED_2_GREEN|LED_2_BLUE)

#define LED_LIGHTUP(port, pin)   (port->BRR = pin)
#define LED_LIGHTSHUT(port, pin) (port->BSRR = pin)

#define LED_3_PORT  GOIOB
#define LED_3_RCC   RCC_APB2Periph_GPIOB
#define LED_3_PIN  

#define LED_4_PORT  GOIOB
#define LED_4_RCC   RCC_APB2Periph_GPIOB
#define LED_4_PIN  

/* jiffies mask */
#define LED_JIFFIES_MASK      LED1_RED_MASK

/* store led color */
static u8 g_LED1_R;
static u8 g_LED1_G;
static u8 g_LED1_B;
static u8 g_LED2_R;
static u8 g_LED2_G;
static u8 g_LED2_B;
static u8 g_LED3;
static u8 g_LED4;

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_APB2PeriphClockCmd(LED_1_RCC | LED_2_RCC | LED_3_RCC | LED_4_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED_1_ALL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(LED_1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_2_ALL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(LED_2_PORT, &GPIO_InitStructure);

/*  TODO
	GPIO_InitStructure.GPIO_Pin = LED_3_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(LED_3_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_4_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(LED_4_PORT, &GPIO_InitStructure);
*/

	GPIO_SetBits(LED_1_PORT, LED_1_ALL);	
	GPIO_SetBits(LED_2_PORT, LED_2_ALL);
	//GPIO_SetBits(LED_3_PORT, LED_3_PIN);
	//GPIO_SetBits(LED_4_PORT, LED_4_PIN);	

	return;
}

void LED_Interrupt(void)
{
    register u16 counter = (u16)(g_jiffies & LED_JIFFIES_MASK);

	/* TODO: SHUTDOWN ALL LED */

    /* shut led 1 */
    LED_LIGHTSHUT(LED_1_PORT, LED_1_ALL);

    /* light up the color */
    if (g_LED1_R > counter){
        LED_LIGHTUP(LED_1_PORT, LED_1_RED);
    } 

    if (g_LED1_G > counter){
        LED_LIGHTUP(LED_1_PORT, LED_1_GREEN);
    }

    if (g_LED1_B > counter){
        LED_LIGHTUP(LED_1_PORT, LED_1_BLUE);
    }

    /* shut led 2 */
    LED_LIGHTSHUT(LED_2_PORT, LED_2_ALL);

    /* light up the color */
    if (g_LED2_R > counter){
        LED_LIGHTUP(LED_2_PORT, LED_2_RED);
    } 

    if (g_LED2_G > counter){
        LED_LIGHTUP(LED_2_PORT, LED_2_GREEN);
    }

    if (g_LED2_B > counter){
        LED_LIGHTUP(LED_2_PORT, LED_2_BLUE);
    }

	/*
	if (g_LED3 > counter){
		LED_LIGHTUP(LED_3_PORT, LED_3_PIN);
	}

	if (g_LED4 > counter){
		LED_LIGHTUP(LED_4_PORT, LED_4_PIN);
	} */

    return;
}

void LED_SetColor(u32 color)
{
	g_LED1_R = LED1_GET_RED(color); 
	g_LED1_G = LED1_GET_GREEN(color); 
	g_LED1_B = LED1_GET_BLUE(color); 
	g_LED2_R = LED2_GET_RED(color); 
	g_LED2_G = LED2_GET_GREEN(color); 
	g_LED2_B = LED2_GET_BLUE(color);
	g_LED3   = LED3_GET_COLOR(color); 
	g_LED4   = LED4_GET_COLOR(color); 

	return;
}


