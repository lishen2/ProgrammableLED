#ifndef __UTILS_H
#define __UTILS_H

#define BOOL_TRUE    1
#define BOOL_FALSE   0

#define ERROR_ERROR -1
#define ERROR_FAILED ERROR_ERROR
#define ERROR_SUCCESS 0

#define NULL 0

#define inline __inline

/* determine if a is after b */
#define time_after(a, b) ((signed int)((signed int)(b) - (signed int)(a)) < 0)

extern vu32 g_jiffies;

static inline void PWR_EnterSLEEPMode(void)
{
    SCB->SCR |= SCB_SCR_SLEEPONEXIT;   // Set SLEEPONEXIT                       
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP;    // Clear SLEEPDEEP bit

    __WFI();                           // Request Wait For Interrupt
}

void delay_ms(int n);
void HW_CommonInit(void);
void HW_InitSysTick(void);

#endif /* __UITLS_H */

