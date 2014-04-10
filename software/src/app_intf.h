#ifndef _APP_INTF_H_
#define _APP_INTF_H_

struct APP_INTF
{
	void (*Start)(void);
	void (*Stop)(void);
};

#endif

