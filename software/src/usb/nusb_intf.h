#ifndef _NUSB_INTF_H_
#define _NUSB_INTF_H_

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

//low priority interrupt handler
void NUSB_LP_INT(void);

//send data through EP0
void NUSB_EP0SendData(u8* buf, u32 length);

/****************************************************************
** interfaces that user programme must implement
*****************************************************************/

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

typedef struct _NUSB_REQUEST{
    u8 USBbmRequestType;       /* bmRequestType */
    u8 USBbRequest;            /* bRequest */
    uint16_t_uint8_t USBwValues;         /* wValue */
    uint16_t_uint8_t USBwIndexs;         /* wIndex */
    uint16_t_uint8_t USBwLengths;        /* wLength */
}NUSB_REQUEST;

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

	void (*SetConfiguration)(void); /* called when set configuration request issued by the host	 */
					 			    /* usually need to initlize used none-zero end poing */

	NUSB_RESULT (*ClassSetup)(NUSB_REQUEST* req);    

	void (*Ep_IN[7])(void);    /* called when host successfully 
	                              recived data send form ep 1~7 */
	void (*Ep_OUT[7])(void);   /* called when host send data to ep 1~7 */
}NUSB_DEVICE_OPS;

//device operations
extern NUSB_DEVICE_OPS g_devOps;

typedef struct _DEVICE_CONFIGURATION
{
    u16 IMR_MSK;
    u16 EP0BufferSize;
	u8  TotalConfiguration;
}NUSB_DEVICE_CONFIGURATION;

extern NUSB_DEVICE_CONFIGURATION g_devConf;

#endif

