#ifndef _DISPLAY_STATE_H_
#define _DISPLAY_STATE_H_

enum DISPLAY_STATE
{
	STATE_GRADIENTER,
	STATE_BICYCLE_LIGHT,
	STATE_LEFT,
	STATE_RIGHT,
	STATE_MAX,
};

void STATE_Change(void);
void STATE_SetState(int state);
void STATE_Poll(void);

#endif

