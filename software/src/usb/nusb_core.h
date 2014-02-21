#ifndef _NUSB_CORE_H_
#define _NUSB_CORE_H_

typedef enum _CONTROL_STATE
{
    NUSB_STAT_WAIT_SETUP,       /* 0 */
    NUSB_STAT_SEND_DESCRIPTOR_WAIT_STATUS_IN,
  
//  NUSB_STAT_SETTING_UP,       /* 1 */
//  NUSB_IN_DATA,          /* 2 */
//  NUSB_OUT_DATA,         /* 3 */
//  NUSB_LAST_IN_DATA,     /* 4 */
//  NUSB_LAST_OUT_DATA,    /* 5 */
//  NUSB_WAIT_STATUS_IN,   /* 7 */
//  NUSB_WAIT_STATUS_OUT,  /* 8 */
    NUSB_STALLED,          /* 9 */
//  NUSB_PAUSE             /* 10 */
} NUSB_CONTROL_STATE;

typedef struct _NUSB_REQUEST{
    u8 USBbmRequestType;       /* bmRequestType */
    u8 USBbRequest;            /* bRequest */
    uint16_t_uint8_t USBwValues;         /* wValue */
    uint16_t_uint8_t USBwIndexs;         /* wIndex */
    uint16_t_uint8_t USBwLengths;        /* wLength */
}NUSB_REQUEST;

typedef struct _NUSB_SEND_STATUS{
    u32 DataSend;                           /* how many bytes already send */
    u32 DataLength;                         /* total data length */
    u8* (*copyCB)(u32 offset, u32 length);  /* function to retrieve data */
}NUSB_SEND_STATUS;

typedef struct _NUSB_SETUP_STATUS{
    uint8_t ControlState;           /* of type CONTROL_STATE */
//    uint8_t Current_Feature;
//    uint8_t Current_Configuration;   /* Selected configuration */
//    uint8_t Current_Interface;       /* Selected interface of current configuration */
//    uint8_t Current_AlternateSetting;/* Selected Alternate Setting of current */
    NUSB_SEND_STATUS SendStatus;
}NUSB_SETUP_STATUS;

//extern NUSB_DEVICE_STATE g_DevStatus;
extern NUSB_SETUP_STATUS g_SetupStatus;

extern vu16 g_SaveRxStatus;
extern vu16 g_SaveTxStatus;

void NUSB_EP0_InProcess(void);
void NUSB_EP0_OutProcess(void);
void NUSB_EP0_SetupProcess(void);

#endif

