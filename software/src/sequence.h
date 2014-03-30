#ifndef _SEQUENCE_H_
#define _SEQUENCE_H_

#define SEQ_LED_SEQUENCE_LEN       96

extern u32 g_bicycleSeq[SEQ_LED_SEQUENCE_LEN];
extern u32 g_leftSeq[SEQ_LED_SEQUENCE_LEN];
extern u32 g_rightSeq[SEQ_LED_SEQUENCE_LEN];

void SEQ_Init(void);

void SEQ_EnableDisplay(void);
void SEQ_DisableDisplay(void);

void SEQ_SetDelayTime(u32 delay);
void SEQ_SetCurMode(int mode);

void SEQ_SaveToFlash(void);

#endif

