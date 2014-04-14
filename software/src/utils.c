#include "stm32f10x.h"
#include "utils.h"

vu32 g_jiffies = 0;

/* 公共的硬件初始化，当前仅初始化了外设总线 */
void HW_CommonInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* set 2 bit preemptible */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* We didn't use oscillator of any kind, so remap PD0,1 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);

    /* set PC13,14,15 to IPD */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

    /* set PD0,1 to IPD */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	    

	return;
}

void HW_InitSysTick(void)
{
    /* Setup SysTick Timer for 0.5 msec interrupts  */
    if (SysTick_Config(8000)) 
    { 
        /* Capture error */ 
        while (1);
    }  
    /* Configure the SysTick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x0);

    return;
}

/* 毫秒级延时 */
void delay_ms(int n)
{
	n += g_jiffies;
    
    while(time_after(n, g_jiffies)){
    }
    
    return;
}


