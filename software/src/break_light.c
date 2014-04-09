#include "stm32f10x.h"
#include "utils.h"
#include "break_light.h"
#include "led.h"
#include "app_intf.h"

static void _BKL_Start(void);
static void _BKL_Stop(void);
static void _BKL_Periodic(void);

struct APP_INTF g_appBreakLight = 
{
	_BKL_Start,
	_BKL_Stop,
	_BKL_Periodic
};

static void _BKL_Start(void)
{
    return;
}

static void _BKL_Stop(void)
{
    return;
}

static void _BKL_Periodic(void)
{

}



