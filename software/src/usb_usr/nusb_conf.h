#ifndef _NUSB_CONF_H_
#define _NUSB_CONF_H_


#define NUSB_BTABLE_ADDRESS      (0x00)

/* EP0  */
/* rx/tx buffer base address */
#define NUSB_ENDP0_RXADDR        (0x18)
#define NUSB_ENDP0_TXADDR        (0x58)

/* EP0 max buffer size */
#define NUSB_ENDP0_BUFFER_SIZE    (0x40)

/* EP1  */
/* tx buffer base address */
#define NUSB_ENDP1_TXADDR        (0x98)

/* EP1 buffer size */
#define NUSB_ENDP1_BUFFER_SIZE    (0x40)

/* EP2  */
/* Rx buffer base address */
#define NUSB_ENDP2_RXADDR        (0xD8)
/* EP2 buffer size */
#define NUSB_ENDP2_BUFFER_SIZE    (0x40)


/* define ENDP1 as receive end point, ENDP2 as send end point */
#define NUSB_SEND_EENDPOINT   ENDP1
#define NUSB_RECEIVE_ENDPOINT ENDP2   


/* ISTR events */
/* mask defining which events has to be handled by the device application software */
#define NUSB_IMR_MSK (CNTR_CTRM  | CNTR_WKUPM | CNTR_SUSPM | CNTR_RESETM )

/* The number of configurations */
#define NUSB_TOTAL_CONFIGURATION     1

#define NUSB_MAX_LUN                 0 /* zero means one logic unit */


#endif /* __USB_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
