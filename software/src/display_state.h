#ifndef _DISPLAY_STATE_H_
#define _DISPLAY_STATE_H_

enum DISPLAY_STATE
{
	STATE_BREAK_LIGHT,
    STATE_ALARM,
	STATE_MAX,
};

void STATE_Init(void);
void STATE_NextState(void);
void STATE_Poll(void);

#endif

