#ifndef _LED_H_
#define _LED_H_

enum LED_COLOR_ARY
{
	LED1_RED = 0,
	LED1_GREEN,
	LED1_BLUE,
	LED2_RED,
	LED2_GREEN,
	LED2_BLUE,
};

enum LED_CONTROL
{
	LED_LIGHTUP,
	LED_SHUTDOWN,
};

void LED_Init(void);

void LED_LightupAll(void);
void LED_ShutdownAll(void);

void LED_LightControl(int led, int act);


#endif

