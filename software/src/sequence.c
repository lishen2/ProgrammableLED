#include <string.h>
#include "stm32f10x.h"
#include "sequence.h"
#include "display_state.h"
#include "utils.h"
#include "led.h"

#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

/* 板子使用48脚的stm32，因此是中低密度芯片，
   带了64kb的flash, 使用最后一页1024字节存放闪烁数据 */
   
/* flash中用于存放数据的位置 */
#define SEQ_DATA_START_ADDRESS  (0x8000000 + 0xFC00)

#define SEQ_BICYCLE_BUFFER_ADDRESS  (SEQ_DATA_START_ADDRESS + 0 * SEQ_LED_SEQUENCE_LEN)
#define SEQ_LEFT_BUFFER_ADDRESS     (SEQ_DATA_START_ADDRESS + 1 * SEQ_LED_SEQUENCE_LEN)
#define SEQ_RIGHT_BUFFER_ADDRESS	(SEQ_DATA_START_ADDRESS + 2 * SEQ_LED_SEQUENCE_LEN)

#define SEQ_LED_DEFAYLT_DELAYTIME  60

u32 g_bicycleSeq[SEQ_LED_SEQUENCE_LEN];
u32 g_leftSeq[SEQ_LED_SEQUENCE_LEN];

/* = {
	0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000,	//1
	0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00,	//2
	0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000,	//3
	0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00,	//4
	0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000,	//5
	0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00,	//6
	0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000,	//7
	0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00,	//8
	0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000,	//9
	0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00,	//10
	0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000, 0x0000F000, 0x00000000,	//11
	0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00, 0x00000000, 0x00000F00	//12
}; */

u32 g_rightSeq[SEQ_LED_SEQUENCE_LEN];
/* 
= {
	0x00F00000, 0x00E00001, 0x00D00002, 0x00C00003, 0x00B00004, 0x00A00005, 0x00900006, 0x00800007,	//1
	0x00700008, 0x00600009, 0x0050000A, 0x0040000B, 0x0030000C, 0x0020000D, 0x0010000E, 0x0000000F,	//2
	0x0000000F, 0x0000001E, 0x0000002D, 0x0000003C, 0x0000004B, 0x0000005A, 0x00000069, 0x00000078,	//3
	0x00000087, 0x00000096, 0x000000A5, 0x000000B4, 0x000000C3, 0x000000D2, 0x000000E1, 0x000000F0,	//4
	0x000000F0, 0x000001E0, 0x000002D0, 0x000003C0, 0x000004B0, 0x000005A0, 0x00000690, 0x00000780,	//5
	0x00000870, 0x00000960, 0x00000A50, 0x00000B40, 0x00000C30, 0x00000D20, 0x00000E10, 0x00000F00,	//6
	0x00000F00, 0x00001E00, 0x00002D00, 0x00003C00, 0x00004B00, 0x00005A00, 0x00006900, 0x00007800,	//7
	0x00008700, 0x00009600, 0x0000A500, 0x0000B400, 0x0000C300, 0x0000D200, 0x0000E100, 0x0000F000,	//8
	0x0000F000, 0x0001E000, 0x0002D000, 0x0003C000, 0x0004B000, 0x0005A000, 0x00069000, 0x00078000,	//9
	0x00087000, 0x00096000, 0x000A5000, 0x000B4000, 0x000C3000, 0x000D2000, 0x000E1000, 0x000F0000,	//10
	0x000F0000, 0x001E0000, 0x002D0000, 0x003C0000, 0x004B0000, 0x005A0000, 0x00690000, 0x00780000,	//11
	0x00870000, 0x00960000, 0x00A50000, 0x00B40000, 0x00C30000, 0x00D20000, 0x00E10000, 0x00F00000,	//12
}; */

static u32* g_curBuf;
static u32 g_counter;

#define SEQ_DISPLAY_TIMER      TIM3
#define SEQ_DISPLAY_RCC        RCC_APB1Periph_TIM3
#define SEQ_DISPLAY_IRQ        TIM3_IRQn
#define SEQ_DISPLAY_IRQROUTINE TIM3_IRQHandler

static void _initTimer(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(SEQ_DISPLAY_RCC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = SEQ_DISPLAY_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(SEQ_DISPLAY_TIMER);	
	TIM_TimeBaseStructure.TIM_Period = SEQ_LED_DEFAYLT_DELAYTIME;	
	TIM_TimeBaseStructure.TIM_Prescaler = 8000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(SEQ_DISPLAY_TIMER, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(SEQ_DISPLAY_TIMER, ENABLE);
	TIM_SetCounter(SEQ_DISPLAY_TIMER, SEQ_LED_DEFAYLT_DELAYTIME);

	TIM_Cmd(SEQ_DISPLAY_TIMER, DISABLE);
	TIM_ITConfig(SEQ_DISPLAY_TIMER, TIM_IT_Update, DISABLE);
	
	return;
}	

static void _restoreSeq(void)
{
	memcpy(g_bicycleSeq, (void*)SEQ_BICYCLE_BUFFER_ADDRESS, SEQ_LED_SEQUENCE_LEN);
	memcpy(g_leftSeq,    (void*)SEQ_LEFT_BUFFER_ADDRESS,    SEQ_LED_SEQUENCE_LEN); 
	memcpy(g_rightSeq,   (void*)SEQ_RIGHT_BUFFER_ADDRESS,   SEQ_LED_SEQUENCE_LEN);	

	return;
}

void SEQ_Init(void)
{
    _initTimer();
	_restoreSeq();

    return;
}

void SEQ_SetDelayTime(u32 delay)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
	TIM_TimeBaseStructure.TIM_Period = delay;	
	TIM_TimeBaseStructure.TIM_Prescaler = 8000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(SEQ_DISPLAY_TIMER, &TIM_TimeBaseStructure);
    
    return;
}

void SEQ_SetCurMode(int mode)
{
	switch(mode)
	{
		case STATE_BICYCLE_LIGHT:
		{
			g_curBuf = g_bicycleSeq;
			break;
		}        
		case STATE_LEFT:
		{
			g_curBuf = g_leftSeq;
			break;
		}
		case STATE_RIGHT:
		{
			g_curBuf = g_rightSeq;
			break;
		}
	    case STATE_GRADIENTER:
        default: 
        {
            break;
        }
        
	};

    g_counter = 0;

    return;
}

void SEQ_EnableDisplay(void)
{
	TIM_Cmd(SEQ_DISPLAY_TIMER, ENABLE);
	TIM_ITConfig(SEQ_DISPLAY_TIMER, TIM_IT_Update, ENABLE);

    return;
}

void SEQ_DisableDisplay(void)
{
	TIM_Cmd(SEQ_DISPLAY_TIMER, DISABLE);
	TIM_ITConfig(SEQ_DISPLAY_TIMER, TIM_IT_Update, DISABLE);
	LED_SetColor(0);

    return;
}

void SEQ_DISPLAY_IRQROUTINE(void)
{
	if (TIM_GetITStatus(SEQ_DISPLAY_TIMER, TIM_IT_Update) != RESET){
     
		TIM_ClearITPendingBit(SEQ_DISPLAY_TIMER, TIM_IT_Update);
        
        LED_SetColor(g_curBuf[g_counter++]);
        if (g_counter >= SEQ_LED_SEQUENCE_LEN){
        	g_counter = 0;
        }
    }

	return;	
}

void _writeData(u32 addr, u8* data, int len)
{
    int offset;

    /* 长度和地址必须为4的整数倍 */
    addr = addr & 0xFFFFFFFC;
    len  = len & 0xFFFFFFFC;
    offset += 4;
    while(offset < len)
    {
        FLASH_ProgramWord(addr + offset, (u32)data + offset);
        offset += 4;
    }
    
    return;
}

void SEQ_SaveToFlash(void)
{
	/* 解锁flash */
    FLASH_UnlockBank1();

    /* 先擦除 */
    FLASH_ErasePage(SEQ_DATA_START_ADDRESS);
    
    /* 按字写入 */
    _writeData(SEQ_BICYCLE_BUFFER_ADDRESS, (u8*)g_bicycleSeq, SEQ_LED_SEQUENCE_LEN);
	_writeData(SEQ_LEFT_BUFFER_ADDRESS, (u8*)g_leftSeq, SEQ_LED_SEQUENCE_LEN);
	_writeData(SEQ_RIGHT_BUFFER_ADDRESS, (u8*)g_rightSeq, SEQ_LED_SEQUENCE_LEN);  

    FLASH_LockBank1();

	return;
}


