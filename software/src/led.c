#include "stm32f10x.h"
#include "led.h"
#include "utils.h"

/* color 4 bits pre color, and we use the 4 LSB to represent red */
#define LED1_RED_MASK          0x0000000F
#define LED1_GREEN_MASK        0x000000F0
#define LED1_BLUE_MASK         0x00000F00
#define LED2_RED_MASK          0x0000F000
#define LED2_GREEN_MASK        0x000F0000
#define LED2_BLUE_MASK         0x00F00000
#define LED3_MASK              0x0F000000
#define LED4_MASK			   0xF0000000

#define LED_BITS_PRECOLOR     4

/* jiffies mask */
#define LED_JIFFIES_MASK      LED1_RED_MASK

#define LED1_GET_RED(color)    ((color & LED1_RED_MASK)   >> 0*LED_BITS_PRECOLOR)
#define LED1_GET_GREEN(color)  ((color & LED1_GREEN_MASK) >> 1*LED_BITS_PRECOLOR)
#define LED1_GET_BLUE(color)   ((color & LED1_BLUE_MASK)  >> 2*LED_BITS_PRECOLOR)
#define LED2_GET_RED(color)    ((color & LED2_RED_MASK)   >> 3*LED_BITS_PRECOLOR)
#define LED2_GET_GREEN(color)  ((color & LED2_GREEN_MASK) >> 4*LED_BITS_PRECOLOR)
#define LED2_GET_BLUE(color)   ((color & LED2_BLUE_MASK)  >> 5*LED_BITS_PRECOLOR)
#define LED3_GET_COLOR(color)  ((color & LED3_MASK)       >> 6*LED_BITS_PRECOLOR)
#define LED4_GET_COLOR(color)  ((color & LED4_MASK)       >> 7*LED_BITS_PRECOLOR)

#define LED1_SET_RED(color, bright)    (color = ((color & ~LED1_RED_MASK) | (bright << 0*LED_BITS_PRECOLOR)))
#define LED1_SET_GREEN(color, bright)  (color = ((color & ~LED1_GREEN_MASK) | (bright << 1*LED_BITS_PRECOLOR)))
#define LED1_SET_BLUE(color, bright)   (color = ((color & ~LED1_BLUE_MASK) | (bright << 2*LED_BITS_PRECOLOR)))
#define LED2_SET_RED(color, bright)    (color = ((color & ~LED2_RED_MASK) | (bright << 3*LED_BITS_PRECOLOR)))
#define LED2_SET_GREEN(color, bright)  (color = ((color & ~LED2_GREEN_MASK) | (bright << 4*LED_BITS_PRECOLOR)))
#define LED2_SET_BLUE(color, bright)   (color = ((color & ~LED2_BLUE_MASK) | (bright << 5*LED_BITS_PRECOLOR)))
#define LED3_SET_COLOR(color, bright)  (color = ((color & ~LED3_MASK) | (bright << 6*LED_BITS_PRECOLOR)))
#define LED4_SET_COLOR(color, bright)  (color = ((color & ~LED4_MASK) | (bright << 7*LED_BITS_PRECOLOR)))

/* led pin config */
#define LED_PORT  GPIOB
#define LED_RCC   RCC_APB2Periph_GPIOB

#define LED_1_RED    GPIO_Pin_13
#define LED_1_GREEN  GPIO_Pin_12
#define LED_1_BLUE	 GPIO_Pin_14
#define LED_2_RED    GPIO_Pin_6
#define LED_2_GREEN	 GPIO_Pin_7 
#define LED_2_BLUE	 GPIO_Pin_4
#define LED_3_PIN    GPIO_Pin_8
#define LED_4_PIN    GPIO_Pin_15

#define LED_ALL_PIN  (LED_1_RED|LED_1_GREEN|LED_1_BLUE|\
                      LED_2_RED|LED_2_GREEN|LED_2_BLUE|\
                      LED_3_PIN|LED_4_PIN)

#define LED_LIGHTUP(port, pin)   (port->BRR = pin)
#define LED_LIGHTSHUT(port, pin) (port->BSRR = pin)

/* led power control pin */
#define LED_POWER_PORT_RCC RCC_APB2Periph_GPIOA
#define LED_POWER_PORT     GPIOB
#define LED_POWER_PIN      GPIO_Pin_3

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

	RCC_APB2PeriphClockCmd(LED_RCC | LED_POWER_PORT_RCC, ENABLE);

    /* disable JTAG, leave only SWD, we use PB4 as LED2 Blue */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED_ALL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);
    LED_LIGHTSHUT(LED_PORT, LED_ALL_PIN);

	GPIO_InitStructure.GPIO_Pin = LED_POWER_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_POWER_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED_POWER_PORT, LED_POWER_PIN);

	return;
}

void LED_IRQHandler(void)
{
    register u16 counter = (u16)(g_jiffies & LED_JIFFIES_MASK);
    register u16 outreg;

    outreg = 0;
    /* light up the color */
    if (g_LED1_R > counter){
        outreg |= LED_1_RED;
    } 

    if (g_LED1_G > counter){
        outreg |= LED_1_GREEN;
    }

    if (g_LED1_B > counter){
        outreg |= LED_1_BLUE;
    } 

    /* light up the color */
    if (g_LED2_R > counter){
        outreg |= LED_2_RED;
    } 

    if (g_LED2_G > counter){
        outreg |= LED_2_GREEN;
    }

    if (g_LED2_B > counter){
        outreg |= LED_2_BLUE;
    }

	if (g_LED3 > counter){
        outreg |= LED_3_PIN;
	}

	if (g_LED4 > counter){
        outreg |= LED_4_PIN;
	}

	/* SHUTDOWN ALL LED */
    LED_LIGHTSHUT(LED_PORT, LED_ALL_PIN);
    LED_LIGHTUP(LED_PORT, outreg);

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

void LED_PowerOn(void)
{
    GPIO_SetBits(LED_POWER_PORT, LED_POWER_PIN);
}

void LED_PowerOff(void)
{
    GPIO_ResetBits(LED_POWER_PORT, LED_POWER_PIN);
}

