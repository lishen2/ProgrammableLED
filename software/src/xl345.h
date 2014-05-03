#ifndef _XL345_H_
#define _XL345_H_

/* ------- Register names ------- */
#define XL345_DEVID           0x00
#define XL345_RESERVED1       0x01
#define XL345_THRESH_TAP      0x1d
#define XL345_OFSX            0x1e
#define XL345_OFSY            0x1f
#define XL345_OFSZ            0x20
#define XL345_DUR             0x21
#define XL345_LATENT          0x22
#define XL345_WINDOW          0x23
#define XL345_THRESH_ACT      0x24
#define XL345_THRESH_INACT    0x25
#define XL345_TIME_INACT      0x26
#define XL345_ACT_INACT_CTL   0x27
#define XL345_THRESH_FF       0x28
#define XL345_TIME_FF         0x29
#define XL345_TAP_AXES        0x2a
#define XL345_ACT_TAP_STATUS  0x2b
#define XL345_BW_RATE         0x2c
#define XL345_POWER_CTL       0x2d
#define XL345_INT_ENABLE      0x2e
#define XL345_INT_MAP         0x2f
#define XL345_INT_SOURCE      0x30
#define XL345_DATA_FORMAT     0x31
#define XL345_DATAX0          0x32
#define XL345_DATAX1          0x33
#define XL345_DATAY0          0x34
#define XL345_DATAY1          0x35
#define XL345_DATAZ0          0x36
#define XL345_DATAZ1          0x37
#define XL345_FIFO_CTL        0x38
#define XL345_FIFO_STATUS     0x39

/*----------------------------------------------------------------------
  Bit field definitions and register values
  ----------------------------------------------------------------------*/
//#define XL345_
/* register values for DEVID                                            */
/* The device ID should always read this value, The customer does not
  need to use this value but it can be samity checked to check that the
  device can communicate                                                */
 
#define XL345_ID              0xe5

/* Reserved soft reset value                                            */
#define XL345_SOFT_RESET      0x52

/* Registers THRESH_TAP through TIME_INACT just take 8 bit values
   There are no specific bit fields in these registers                  */

/* Bit values in ACT_INACT_CTL                                          */
#define XL345_INACT_Z_ENABLE  0x01
#define XL345_INACT_Z_DISABLE 0x00
#define XL345_INACT_Y_ENABLE  0x02
#define XL345_INACT_Y_DISABLE 0x00
#define XL345_INACT_X_ENABLE  0x04
#define XL345_INACT_X_DISABLE 0x00
#define XL345_INACT_AC        0x08
#define XL345_INACT_DC        0x00
#define XL345_ACT_Z_ENABLE    0x10
#define XL345_ACT_Z_DISABLE   0x00
#define XL345_ACT_Y_ENABLE    0x20
#define XL345_ACT_Y_DISABLE   0x00
#define XL345_ACT_X_ENABLE    0x40
#define XL345_ACT_X_DISABLE   0x00
#define XL345_ACT_AC          0x80
#define XL345_ACT_DC          0x00

/* Registers THRESH_FF and TIME_FF just take 8 bit values
   There are no specific bit fields in these registers
   units of THRESH_FF 1/16 gee/LSB
   units of TIME_FF 5 ms.                                               */

/* Bit values in TAP_AXES                                               */
#define XL345_TAP_Z_ENABLE    0x01
#define XL345_TAP_Z_DISABLE   0x00
#define XL345_TAP_Y_ENABLE    0x02
#define XL345_TAP_Y_DISABLE   0x00
#define XL345_TAP_X_ENABLE    0x04
#define XL345_TAP_X_DISABLE   0x00
#define XL345_TAP_SUPPRESS    0x08

/* Bit values in ACT_TAP_STATUS                                         */
#define XL345_TAP_Z_SOURCE    0x01
#define XL345_TAP_Y_SOURCE    0x02
#define XL345_TAP_X_SOURCE    0x04
#define XL345_STAT_ASLEEP     0x08
#define XL345_ACT_Z_SOURCE    0x10
#define XL345_ACT_Y_SOURCE    0x20
#define XL345_ACT_X_SOURCE    0x40

/* Bit values in BW_RATE                                                */
/* Expresed as output data rate */
#define XL345_RATE_3200       0x0f
#define XL345_RATE_1600       0x0e
#define XL345_RATE_800        0x0d
#define XL345_RATE_400        0x0c
#define XL345_RATE_200        0x0b
#define XL345_RATE_100        0x0a
#define XL345_RATE_50         0x09
#define XL345_RATE_25         0x08
#define XL345_RATE_12_5       0x07
#define XL345_RATE_6_25       0x06
#define XL345_RATE_3_125      0x05
#define XL345_RATE_1_563      0x04
#define XL345_RATE__782       0x03
#define XL345_RATE__39        0x02
#define XL345_RATE__195       0x01
#define XL345_RATE__098       0x00

/* Expresed as output bandwidth */
/* Use either the bandwidth or rate code,
   which ever is more appropraite for your application */
#define XL345_BW_1600         0x0f
#define XL345_BW_800          0x0e
#define XL345_BW_400          0x0d
#define XL345_BW_200          0x0c
#define XL345_BW_100          0x0b
#define XL345_BW_50           0x0a
#define XL345_BW_25           0x09
#define XL345_BW_12_5         0x08
#define XL345_BW_6_25         0x07
#define XL345_BW_3_125        0x06
#define XL345_BW_1_563        0x05
#define XL345_BW__782         0x04
#define XL345_BW__39          0x03
#define XL345_BW__195         0x02
#define XL345_BW__098         0x01
#define XL345_BW__048         0x00

#define XL345_LOW_POWER       0x10
#define XL345_LOW_NOISE       0x00
/* Bit values in POWER_CTL                                              */
#define XL345_WAKEUP_8HZ           0x00
#define XL345_WAKEUP_4HZ           0x01
#define XL345_WAKEUP_2HZ           0x02
#define XL345_WAKEUP_1HZ           0x03
#define XL345_SLEEP                0x04
#define XL345_MEASURE              0x08
#define XL345_STANDBY              0x00
#define XL345_AUTO_SLEEP           0x10
#define XL345_ACT_INACT_SERIAL     0x20
#define XL345_ACT_INACT_CONCURRENT 0x00

/* Bit values in INT_ENABLE, INT_MAP, and INT_SOURCE are identical
   use these bit values to read or write any of these registers.        */
#define XL345_OVERRUN              0x01
#define XL345_WATERMARK            0x02
#define XL345_FREEFALL             0x04
#define XL345_INACTIVITY           0x08
#define XL345_ACTIVITY             0x10
#define XL345_DOUBLETAP            0x20
#define XL345_SINGLETAP            0x40
#define XL345_DATAREADY            0x80

/* Bit values in DATA_FORMAT                                            */

/* Register values read in DATAX0 through DATAZ1 are dependant on the 
   value specified in data format.  Customer code will need to interpret
   the data as desired.                                                 */
#define XL345_RANGE_2G             0x00
#define XL345_RANGE_4G             0x01
#define XL345_RANGE_8G             0x02
#define XL345_RANGE_16G            0x03
#define XL345_DATA_JUST_RIGHT      0x00
#define XL345_DATA_JUST_LEFT       0x04
#define XL345_10BIT                0x00
#define XL345_FULL_RESOLUTION      0x08
#define XL345_INT_LOW              0x20
#define XL345_INT_HIGH             0x00
#define XL345_SPI3WIRE             0x40
#define XL345_SPI4WIRE             0x00
#define XL345_SELFTEST             0x80

/* Bit values in FIFO_CTL                                               */
/* The low bits are a value 0-31 used for the watermark or the number
   of pre-trigger samples when in triggered mode                        */
#define XL345_TRIGGER_INT1         0x00
#define XL345_TRIGGER_INT2         0x20
#define XL345_FIFO_MODE_BYPASS     0x00
#define XL345_FIFO_RESET           0x00
#define XL345_FIFO_MODE_FIFO       0x40
#define XL345_FIFO_MODE_STREAM     0x80
#define XL345_FIFO_MODE_TRIGGER    0xc0

/* Bit values in FIFO_STATUS                                            */
/* The low bits are a value 0-32 showing the number of entries
   currently available in the fifo buffer                               */

#define XL345_FIFO_TRIGGERED       0x80

void XL345_Init(void);
void XL345_Deinit(void);
void XL345_Read(unsigned char count, unsigned char regaddr, unsigned char* buf);
void XL345_Write(unsigned char count, unsigned char regaddr, unsigned char *buf);

#endif

