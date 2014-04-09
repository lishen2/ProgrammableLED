#ifndef _ACC_SENSOR_H_
#define _ACC_SENSOR_H_

typedef void (*ACC_IRQHandler)(u8 irq);

void ACC_Init(void);
void ACC_Deinit(void);

void ACC_ReadAcc(u8 fifoLen, s16 *x, s16 *y, s16*z);
void ACC_SetIRQHandler(ACC_IRQHandler handler);

#endif


