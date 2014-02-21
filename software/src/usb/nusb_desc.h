#ifndef _NUSB_DESC_H_
#define _NUSB_DESC_H_

#define MASS_SIZ_DEVICE_DESC              18
#define MASS_SIZ_CONFIG_DESC              32

#define MASS_SIZ_STRING_LANGID            4
#define MASS_SIZ_STRING_VENDOR            38
#define MASS_SIZ_STRING_PRODUCT           38
#define MASS_SIZ_STRING_SERIAL            2 * 16 + 2
#define MASS_SIZ_STRING_INTERFACE         16

/* Exported functions ------------------------------------------------------- */
extern uint8_t MASS_DeviceDescriptor[MASS_SIZ_DEVICE_DESC];
extern uint8_t MASS_ConfigDescriptor[MASS_SIZ_CONFIG_DESC];
extern u8* MASS_StringArray[5];

void NUSB_UpdateSerialNum(void);

#endif
