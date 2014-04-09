#include <stdio.h>
#include "stm32f10x.h"
#include "xl345.h"

/* 控制xl345 cs的GPIO引脚 */
#define XL345_CS_PIN   GPIO_Pin_0
#define XL345_CS_PORT  GPIOB 
#define XL345_CS_CLOCK RCC_APB2Periph_GPIOB

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
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 
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

