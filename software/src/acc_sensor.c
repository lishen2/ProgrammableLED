#include <stdio.h>
#include "stm32f10x.h"
#include "acc_sensor.h"
#include "xl345.h"

void ACC_Init(void)
{
    xl345Init();
    xl345Setup();

    return;
}

u16 ACC_ReadX(void);
u16 ACC_ReadY(void);

u16 ACC_ReadZ(void)
{
	u16 Z;
	u8 ZAry[2];

	xl345Read(2, 0x36, ZAry);
	
	Z = ZAry[0];
	Z = Z << 8;
	Z |= ZAry[1]; 

	return Z;
}


