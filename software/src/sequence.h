#ifndef _SEQUENCE_H_
#define _SEQUENCE_H_

#define SEQ_LED_SEQUENCE_LEN       94

extern u32 g_bicycleSeq[SEQ_LED_SEQUENCE_LEN];
extern u32 g_leftSeq[SEQ_LED_SEQUENCE_LEN];
extern u32 g_rightSeq[SEQ_LED_SEQUENCE_LEN];

void SEQ_SetDelayTime(u32 delay);

void SEQ_DisplayBicycle(void);
void SEQ_DisplayLeft(void);
void SEQ_DisplayRight(void);

#endif

