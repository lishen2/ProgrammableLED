#include "stm32f10x.h"
#include "utils.h"

vu32 g_jiffies = 0;

/* 公共的硬件初始化，当前仅初始化了外设总线 */
void HW_CommonInit(void)
{
    /* 设置2bit可抢占，2bit不可抢占 */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	return;
}

//初始化systick
void HW_InitSysTick(void)
{
    /* Setup SysTick Timer for 0.5 msec interrupts  */
    if (SysTick_Config(4000)) 
    { 
        /* Capture error */ 
        while (1);
    }  
    /* Configure the SysTick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x0);//SysTick中断优先级设置

    return;
}

/* 毫秒级延时 */
void delay_ms(int n)
{
	n <<= 1;
	n += g_jiffies;

    //PWR_PrepareSleep(0);
    
    while(time_after(n, g_jiffies)){
        //__WFI();
    }
    
    return;
}


