#ifndef _LED_H_
#define _LED_H_

/* 4 bits pre color */
#define LED_BITS_PRECOLOR     4

#define LED1_RED_MASK          0x0000000F
#define LED1_GREEN_MASK        0x000000F0
#define LED1_BLUE_MASK         0x00000F00
#define LED2_RED_MASK          0x0000F000
#define LED2_GREEN_MASK        0x000F0000
#define LED2_BLUE_MASK         0x00F00000
#define LED3_MASK              0x0F000000
#define LED4_MASK			   0xF0000000

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

void LED_Init(void);
void LED_SetColor(u32 color);
void LED_IRQHandler(void);

void LED_PowerOn(void);
void LED_PowerOff(void);

#endif

