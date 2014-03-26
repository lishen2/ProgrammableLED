#include "stm32f10x.h"
#include "sequence.h"
#include "display_state.h"
#include "utils.h"
#include "led.h"

#define SEQ_LED_DEFAYLT_DELAYTIME  60

static u32 g_delayTime = SEQ_LED_DEFAYLT_DELAYTIME;

u32 g_bicycleSeq[SEQ_LED_SEQUENCE_LEN] = {
	0x11111111
};

u32 g_leftSeq[SEQ_LED_SEQUENCE_LEN];
u32 g_rightSeq[SEQ_LED_SEQUENCE_LEN];

static u32* g_curBuf;
static u32 g_counter;

static void _displaySequence(void)
{
	LED_SetColor(g_curBuf[g_counter++]);
	if (g_counter >= SEQ_LED_SEQUENCE_LEN){
		g_counter = 0;
	}

	delay_ms(g_delayTime);
	return;	
}

void SEQ_DisplayBicycle(void)
{
	g_curBuf = g_bicycleSeq;
	_displaySequence();

	return;
}

void SEQ_DisplayLeft(void)
{
	g_curBuf = g_leftSeq;
	_displaySequence();

	return;
}

void SEQ_DisplayRight(void)
{
	g_curBuf = g_rightSeq;
	_displaySequence();

	return;
}

