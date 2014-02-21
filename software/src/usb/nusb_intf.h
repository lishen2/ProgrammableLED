#ifndef _NUSB_INTF_H_
#define _NUSB_INTF_H_
/*
typedef enum _DEVICE_STATE
{
  NUSB_UNCONNECTED,
  NUSB_ATTACHED,
  NUSB_POWERED,
  NUSB_SUSPENDED,
  NUSB_ADDRESSED,
  NUSB_CONFIGURED
} NUSB_DEVICE_STATE;
*/
typedef enum _RESULT
{
  NUSB_SUCCESS = 0,    /* Process successfully */
  NUSB_ERROR,
  NUSB_UNSUPPORT
} NUSB_RESULT;

/****************************************************************
** interfaces that USB module explode to user programme
*****************************************************************/

//initialize USB module 
void NUSB_init(void);

//get current status of usb device
//NUSB_DEVICE_STATE NUSB_GetStatus(void);

//low priority interrupt handler
void NUSB_LP_INT(void);

/****************************************************************
** interfaces that need user to implement
*****************************************************************/

typedef struct _DEVICE_OPS
{
	void (*Init)(void);        /* called when USB moduel initialized */
	void (*Reset)(void);       /* called when RESET interrupt occur */

    u32 (*GetDeviceDescriptorLength)(void);
    u8* (*GetDeviceDescriptor)(void);
    u32 (*GetConfigDescriptorLength)(void);
    u8* (*GetConfigDescriptor)(void);
    u32 (*GetStringDescriptorLength)(u8 index);    
    u8* (*GetStringDescriptor)(u8 index);

    
	void (*Ep_IN[7])(void);    /* called when host successfully 
	                              recived data send form ep 1~7 */
	void (*Ep_OUT[7])(void);   /* called when host send data to ep 1~7 */
}NUSB_DEVICE_OPS;

//device operations
extern NUSB_DEVICE_OPS g_devOps;

typedef struct _DEVICE_CONFIGURATION
{
	u8 MaxEPNum;
    u16 IMR_MSK;
    u16 EP0Bufsize;
}NUSB_DEVICE_CONFIGURATION;

extern NUSB_DEVICE_CONFIGURATION g_devConf;

#endif

