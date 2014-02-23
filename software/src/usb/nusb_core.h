#ifndef _NUSB_CORE_H_
#define _NUSB_CORE_H_

typedef enum _CONTROL_STATE
{
    NUSB_STAT_WAIT_SETUP, 
    NUSB_STAT_SEND_DESCRIPTOR_WAIT_ACK,
  	NUSB_STAT_SET_ADDRESS_WAIT_ACK,
	NUSB_STAT_MORE_DATA_TO_SEND,    /* TODO, used when sending data is bigger then ep0 send buffer */
    NUSB_STALLED, 
} NUSB_CONTROL_STATE;

typedef struct _NUSB_SEND_STATUS{
    u32 DataSend;                           /* how many bytes already send */
    u32 DataLength;                         /* total data length */
    u8* (*copyCB)(u32 offset, u32 length);  /* function to retrieve data */
}NUSB_SEND_STATUS;

typedef struct _NUSB_SETUP_STATUS{
    uint8_t ControlState;           /* of type CONTROL_STATE */
//    uint8_t CurrentFeature;
    uint8_t CurrentConfiguration;   /* Selected configuration */
//    uint8_t CurrentInterface;       /* Selected interface of current configuration */
//    uint8_t CurrentAlternateSetting;/* Selected Alternate Setting of current */
    NUSB_SEND_STATUS SendStatus;
}NUSB_SETUP_STATUS;

extern NUSB_SETUP_STATUS g_SetupStatus;

extern vu16 g_SaveRxStatus;
extern vu16 g_SaveTxStatus;

void NUSB_EP0_InProcess(void);
void NUSB_EP0_OutProcess(void);
void NUSB_EP0_SetupProcess(void);

#endif

