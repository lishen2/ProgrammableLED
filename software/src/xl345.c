#include <stdio.h>
#include "stm32f10x.h"
#include "xl345.h"

/* 控制xl345 cs的GPIO引脚 */
#define XL345_CS_PIN   GPIO_Pin_1
#define XL345_CS_PORT  GPIOA 
#define XL345_CS_CLOCK RCC_APB2Periph_GPIOA

#define XL345_DISABLE() GPIO_SetBits(XL345_CS_PORT, XL345_CS_PIN); 
#define XL345_ENABLE()  GPIO_ResetBits(XL345_CS_PORT, XL345_CS_PIN);

//SPIx 读写一个字节
//返回值:读取到的字节
static u8 _spi_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 
	while((SPI1->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  

	SPI1->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //返回收到的数据				    
}

void xl345Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable SPI1 and GPIOA clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|XL345_CS_CLOCK, ENABLE);

	/* Configure SPI1 pins: NSS, SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* xl345 CS */
    GPIO_InitStructure.GPIO_Pin = XL345_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(XL345_CS_PORT, &GPIO_InitStructure);
	GPIO_SetBits(XL345_CS_PORT, XL345_CS_PIN);  

	/* SPI1 configuration */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI1设置为两线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                   //设置SPI1为主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	 		               //串行时钟在不操作时，时钟为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		               //第二个时钟沿开始采样数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			               //NSS信号由软件（使用SSI位）管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;						   //CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);
	/* Enable SPI1  */
	SPI_Cmd(SPI1, ENABLE); 
	
	return;	
}

void xl345Read(unsigned char count, unsigned char regaddr, unsigned char* buf)
{
	int i;

	XL345_ENABLE();

	//write address
	_spi_ReadWriteByte(regaddr|0xC0);

	//read data
	for(i = 0; i < count; ++i){
		buf[i] = _spi_ReadWriteByte(0xFF);
	}
	XL345_DISABLE();

	return;
}

void xl345Write(unsigned char count, unsigned char *buf)
{
	int i;

	XL345_ENABLE();

	_spi_ReadWriteByte(buf[0]|0x40);

	//write data
	for(i = 0; i < count; ++i){
		_spi_ReadWriteByte(buf[i]);
	}
	XL345_DISABLE();

	return;
}

void xl345Setup(void)
{
    u8 buffer[8];
    /* add software reset */

    /*--------------------------------------------------
    TAP Configuration
    --------------------------------------------------*/
    /* set up a buffer with all the initialization for tap */
    buffer[0] = XL345_THRESH_TAP; /* register address */
    buffer[1] = 80; /* THRESH_TAP = 5 Gee (1 lsb = 1/16 gee) */	
    xl345Write(2,buffer);

    buffer[0] = XL345_DUR; /* register address */
    buffer[1] = 13; /* DUR = 8ms 0.6125ms/lsb */
    buffer[2] = 80; /* LATENT = 100 ms 1.25ms/lsb */
    buffer[3] = 240;/* WINDOW 300ms 1.25ms/lsb */
    xl345Write(4,buffer);

    buffer[0] = XL345_TAP_AXES; /* register address */
    buffer[1] = XL345_TAP_Z_ENABLE | XL345_TAP_Y_ENABLE 
                | XL345_TAP_X_ENABLE | XL345_TAP_SUPPRESS;
    xl345Write(2,buffer);

    /*--------------------------------------------------
    activity - inactivity 
    --------------------------------------------------*/
    /* set up a buffer with all the initialization for activity and inactivity */
    buffer[0] = XL345_THRESH_ACT; /* register address */
    buffer[1] = 80; /* THRESH_ACT = 80/16 = 5 Gee (1 lsb = 1/16 gee) */
    buffer[2] = 4; /* THRESH_INACT = 14/16 .25 Gee (1 lsb = 1/16 gee) */
    buffer[3] = 5;/* TIME_INACT - 5 seconds */
    buffer[4] =     /* ACT_INACT_CTL */
                XL345_ACT_DC 
                | XL345_ACT_X_ENABLE | XL345_ACT_Y_ENABLE | XL345_ACT_Z_ENABLE
                | XL345_INACT_AC | XL345_INACT_X_ENABLE 
                | XL345_INACT_Y_ENABLE | XL345_INACT_Z_ENABLE;
    xl345Write(5,buffer);

    /*--------------------------------------------------
    Power, bandwidth-rate, interupt enabling
    --------------------------------------------------*/

    /* set up a buffer with all the initization for power*/
    buffer[0] = XL345_BW_RATE;    /* register address */
    buffer[1] =                   /* BW_RATE */
                XL345_RATE_100 | XL345_LOW_NOISE;
    buffer[2] =                   /* POWER_CTL */
                XL345_ACT_INACT_SERIAL | XL345_AUTO_SLEEP | 
                XL345_WAKEUP_8HZ | XL345_MEASURE;
    xl345Write(3,buffer);

    // set the FIFO control
    buffer[0] = XL345_FIFO_CTL;
    buffer[1] = XL345_FIFO_MODE_STREAM | // set FIFO mode
            	0 | 		 // link to INT1
            	30;		     // number of samples
    xl345Write(2, buffer);

    // set data format
    buffer[0] = XL345_DATA_FORMAT;
    buffer[1] = XL345_SPI4WIRE | XL345_INT_HIGH | XL345_10BIT |
                XL345_DATA_JUST_RIGHT | XL345_RANGE_2G;
    xl345Write(2, buffer);

    //set interrupt map
    buffer[0] = XL345_INT_MAP;
    buffer[1] = 0; //all interrupts set to pin INT1
    xl345Write(2, buffer);		
    
    // turn on the interrupts
    buffer[0] = XL345_INT_ENABLE;
    buffer[1] = XL345_ACTIVITY | XL345_INACTIVITY | XL345_WATERMARK; 
    xl345Write(2, buffer);		

    xl345Read(1, 0x00, buffer);
    printf("Read device id: [%hhu]\r\n", buffer[0]);
    
    return;
}

