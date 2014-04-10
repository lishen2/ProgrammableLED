#include <stdio.h>
#include "stm32f10x.h"
#include "xl345.h"

/* 控制xl345 cs的GPIO引脚 */
#define XL345_CS_PIN   GPIO_Pin_0
#define XL345_CS_PORT  GPIOB 
#define XL345_CS_CLOCK RCC_APB2Periph_GPIOB

#define XL345_COM_SPI       SPI1
#define XL345_COM_PORT      GPIOA
#define XL345_COM_RCC       RCC_APB2Periph_GPIOA
#define XL345_COM_SCK_PIN   GPIO_Pin_5
#define XL345_COM_MOSI_PIN  GPIO_Pin_7
#define XL345_COM_MISO_PIN  GPIO_Pin_6

#define XL345_DISABLE() GPIO_SetBits(XL345_CS_PORT, XL345_CS_PIN); 
#define XL345_ENABLE()  GPIO_ResetBits(XL345_CS_PORT, XL345_CS_PIN);

//SPIx 读写一个字节
//返回值:读取到的字节
static u8 _spi_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 
	while((XL345_COM_SPI->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  

	XL345_COM_SPI->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((XL345_COM_SPI->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return XL345_COM_SPI->DR;          //返回收到的数据				    
}

void xl345Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable SPI1 and GPIOA clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|XL345_CS_CLOCK, ENABLE);

	/* Configure SPI pins */
	GPIO_InitStructure.GPIO_Pin = XL345_COM_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(XL345_COM_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = XL345_COM_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(XL345_COM_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = XL345_COM_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(XL345_COM_PORT, &GPIO_InitStructure);

	/* xl345 CS */
    GPIO_InitStructure.GPIO_Pin = XL345_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(XL345_CS_PORT, &GPIO_InitStructure);
	GPIO_SetBits(XL345_CS_PORT, XL345_CS_PIN);  

	/* SPI1 configuration */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                   
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	 		               
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		               
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			               
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   
	SPI_InitStructure.SPI_CRCPolynomial = 7;						   
	SPI_Init(XL345_COM_SPI, &SPI_InitStructure);
	/* Enable SPI1  */
	SPI_Cmd(XL345_COM_SPI, ENABLE); 
	
	return;	
}

void xl345PowerOff(void)
{
    SPI_Cmd(XL345_COM_SPI, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
}

void xl345PowerOn(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    SPI_Cmd(XL345_COM_SPI, ENABLE);
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

void xl345Write(unsigned char count, unsigned char addr, unsigned char *buf)
{
	int i;

	XL345_ENABLE();

	_spi_ReadWriteByte(addr|0x40);

	//write data
	for(i = 0; i < count; ++i){
		_spi_ReadWriteByte(buf[i]);
	}
	XL345_DISABLE();

	return;
}

