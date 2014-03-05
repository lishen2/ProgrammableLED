#ifndef _MASS_CONF_H_
#define _MASS_CONF_H_


#define MASS_BTABLE_ADDRESS      (0x00)

/* EP0  */
/* rx/tx buffer base address */
#define MASS_ENDP0_RXADDR        (0x18)
#define MASS_ENDP0_TXADDR        (0x58)

/* EP0 max buffer size */
#define MASS_ENDP0_BUFFER_SIZE    (0x40)

/* EP1  */
/* tx buffer base address */
#define MASS_ENDP1_TXADDR        (0x98)

/* EP1 buffer size */
#define MASS_ENDP1_BUFFER_SIZE    (0x40)

/* EP2  */
/* Rx buffer base address */
#define MASS_ENDP2_RXADDR        (0xD8)
/* EP2 buffer size */
#define MASS_ENDP2_BUFFER_SIZE    (0x40)


/* define ENDP1 as receive end point, ENDP2 as send end point */
#define MASS_SEND_ENDPOINT    ENDP1
#define MASS_RECEIVE_ENDPOINT ENDP2   


/* ISTR events */
/* mask defining which events has to be handled by the device application software */
#define MASS_IMR_MSK (CNTR_CTRM  | CNTR_WKUPM | CNTR_SUSPM | CNTR_RESETM )

/* The number of configurations */
#define MASS_TOTAL_CONFIGURATION     1

#define MASS_MAX_LUN                 0 /* zero means one logic unit */


#define MASS_PULLUP_PIN             GPIO_Pin_10        
#define MASS_PULLUP_GPIO            GPIOA
#define MASS_PULLUP_GPIO_CLK        RCC_APB2Periph_GPIOA

#endif /* __USB_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
