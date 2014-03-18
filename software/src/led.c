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

/* color 4 bits pre color, and we use the 4 LSB to represent red */
#define LED_RED_MASK          0x000F
#define LED_GREEN_MASK        0x00F0
#define LED_BLUE_MASK         0x0F00

#define LED_BITS_PRECOLOR     4

/* jiffies mask */
#define LED_JIFFIES_MASK      LED_RED_MASK

#define LED_GET_RED(color)    ((color & LED_RED_MASK) >> 0)
#define LED_GET_GREEN(color)  ((color & LED_GREEN_MASK) >> LED_BITS_PRECOLOR)
#define LED_GET_BLUE(color)   ((color & LED_BLUE_MASK) >> 2*LED_BITS_PRECOLOR)

/* store led color */
static u8 g_LED1_R;
static u8 g_LED1_G;
static u8 g_LED1_B;
static u8 g_LED2_R;
static u8 g_LED2_G;
static u8 g_LED2_B;

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_APB2PeriphClockCmd(LED_1_RCC | LED_2_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED_1_ALL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(LED_1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_2_ALL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(LED_2_PORT, &GPIO_InitStructure);

	GPIO_SetBits(LED_1_PORT, LED_1_ALL);	
	GPIO_SetBits(LED_2_PORT, LED_2_ALL);	

	return;
}

void LED_Interrupt(void)
{
    register u16 counter = (u16)(g_jiffies & LED_JIFFIES_MASK);

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

    return;
}

void LED_SetColor(u16 led1, u16 led2)
{
	g_LED1_R = LED_GET_RED(led1); 
	g_LED1_G = LED_GET_GREEN(led1); 
	g_LED1_B = LED_GET_BLUE(led1); 
	g_LED2_R = LED_GET_RED(led2); 
	g_LED2_G = LED_GET_GREEN(led2); 
	g_LED2_B = LED_GET_BLUE(led2); 
	return;
}


