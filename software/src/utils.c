#include "stm32f10x.h"
#include "utils.h"

vu32 g_jiffies = 0;

/* ������Ӳ����ʼ������ǰ����ʼ������������ */
void HW_CommonInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* ����2bit����ռ��2bit������ռ */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	return;
}

//��ʼ��systick
void HW_InitSysTick(void)
{
    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(4000)) 
    { 
        /* Capture error */ 
        while (1);
    }  
    /* Configure the SysTick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x0);//SysTick�ж����ȼ�����

    return;
}

/* ���뼶��ʱ */
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


