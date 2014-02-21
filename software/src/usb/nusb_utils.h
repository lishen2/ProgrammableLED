#ifndef _NUSB_UTILS_H_
#define _NUSB_UTILS_H_

typedef union
{
  uint16_t w;
  struct BW
  {
    uint8_t bb1;
    uint8_t bb0;
  }
  bw;
} uint16_t_uint8_t;

void NUSB_SetDeviceAddress(uint8_t Val);

#endif

