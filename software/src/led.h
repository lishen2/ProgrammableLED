#ifndef _LED_H_
#define _LED_H_

void LED_Init(void);
void LED_SetColor(u32 color);
void LED_IRQHandler(void);

void LED_PowerOn(void);
void LED_PowerOff(void);

#endif

