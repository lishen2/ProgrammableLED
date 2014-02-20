#ifndef _NUSB_INTF_H_
#define _NUSB_INTF_H_

typedef enum _DEVICE_STATE
{
  NUSB_UNCONNECTED,
  NUSB_ATTACHED,
  NUSB_POWERED,
  NUSB_SUSPENDED,
  NUSB_ADDRESSED,
  NUSB_CONFIGURED
} NUSB_DEVICE_STATE;

typedef struct _DEVICE_OPS
{
  void (*Init)(void);        /* called when USB moduel initialized */
  void (*Reset)(void);       /* called when RESET interrupt occur */
}NUSB_DEVICE_OPS;

/////////////////////////////////////////////////////////////////////////////////////////
//interfaces that USB module explode to user programme
/////////////////////////////////////////////////////////////////////////////////////////

//initialize USB module 
void NUSB_init(void);

//get current status of usb device
NUSB_DEVICE_STATE NUSB_GetStatus(void);

//low priority interrupt handler
void NUSB_LP_INT(void);

/////////////////////////////////////////////////////////////////////////////////////////
//interfaces that need user to implement
/////////////////////////////////////////////////////////////////////////////////////////
//device operations
extern NUSB_DEVICE_OPS g_devOps;


#endif

