#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "ringbuf.h"
#include "usart_io.h"
#include "utils.h"

#define USART_COMM_IO_READBUF_SIZE  128
#define USART_COMM_IO_WRITEBUF_SIZE 256

/* 串口1使用的读写缓冲区 */
static unsigned char g_USART1ReadDataBuf[USART_COMM_IO_READBUF_SIZE]; 
static unsigned char g_USART1WriteDataBuf[USART_COMM_IO_WRITEBUF_SIZE]; 
RingBuf g_USAER1ReadBufHead = {
    0,
    0,
    sizeof(g_USART1ReadDataBuf),
    g_USART1ReadDataBuf,
    0
};
RingBuf g_USAER1WriteBufHead = {
    0,
    0,
    sizeof(g_USART1WriteDataBuf),
    g_USART1WriteDataBuf,
    0
};

/* 配置串口参数使用的数据结构 */
static USART_InitTypeDef USART_InitStructure;

/* 向指定串口发送数据 */
int USARTIO_SendChar(void* port, unsigned char ch)
{  
    int ret = ERROR_FAILED;
    RingBuf* writeBuf;

    if (USART1 == port){
        writeBuf = &g_USAER1WriteBufHead;
    } 
    else {
        return ERROR_FAILED;
    }

    /* 将数据写入写缓存中 */
    ret = Ringbuf_PutChar(writeBuf, ch);
    
    /* 打开中断 */
    USART_ITConfig(port, USART_IT_TXE, ENABLE);
    
    return ret;
}

/* 从指定串口读取数据 */
int USARTIO_RecvChar(void* port, unsigned char* ch)
{
    int ret = ERROR_FAILED;
    RingBuf* readBuf;

    if (USART1 == port){
        readBuf = &g_USAER1ReadBufHead;
    }
    else {
        return ERROR_FAILED;
    }

    ret = Ringbuf_GetChar(readBuf, ch);

    return ret;
}

/* 向指定端口发送一个字符串 */
int USARTIO_SendString(void* port, unsigned char* str)
{
    int len, i, ret = ERROR_SUCCESS;
	RingBuf* writeBuf;

    if (USART1 == port){
        writeBuf = &g_USAER1WriteBufHead;
    } 
    else {
        return ERROR_FAILED;
    }

    len = strlen((char*)str);
    for(i = 0; i < len;){
        ret = Ringbuf_PutChar(writeBuf, str[i]);
		/* 打开中断 */
   	 	USART_ITConfig(port, USART_IT_TXE, ENABLE);

		if(ERROR_SUCCESS == ret){
			++i;
		}
    }

    printf("Send:%s", str);

    return ret;
}

/* 从串口中读取一行并放入缓存
   读取成功返回ERROR_SUCCESS
   读取失败返回ERROR_FAILED
*/
int USARTIO_ReadLine(void* port, unsigned char* buf, int bufsize)
{
    unsigned char ch;
    int i;
    int ret = ERROR_FAILED;
    RingBuf* readBuf;

    if (USART1 == port){
        readBuf = &g_USAER1ReadBufHead;
    }
    else {
        return ERROR_FAILED;
    }

    /* 如果缓存中还没有一整行，直接退出 */
    if (!RINGBUF_HAVELINE(readBuf)){
        return ERROR_FAILED;
    }

    for (i = 0; i < bufsize;){      

        /* 读取字符 */
        ret = Ringbuf_GetChar(readBuf, &ch);
        if (ERROR_SUCCESS != ret){
            break;
        }
        
        /* skip '\r' */
        if (ch == '\r'){
        } 
        /* 如果是\n则结束读取 */
        else if(ch == '\n') {
            ret = ERROR_SUCCESS;
            break;
        } 
        /* 如果是其他数据则保存到缓冲区中 */
        else {
            buf[i] = ch;
            ++i;
        }
    }

    /* 缓冲区末尾补上结束符符 */
    (i == bufsize)?(buf[i - 1] = '\0'):(buf[i] = '\0');

    if (ERROR_SUCCESS == ret){
        printf("Read:%s\n", buf);
    }
  
    return ret;
}

/* 初始化和通信模块通信使用的串口，使用串口2 */
void USARTIO_InitUSART1(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;    

    /*使能串口使用的GPIO时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /*使能串口时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  

    /* Enable the USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* GPIO */
    /* Configure USART1 Rx as input floating */
	GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 and USART2 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled */
    USART_InitStructure.USART_BaudRate = 115200;                 /*设置波特率*/
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;/*设置数据位为8*/
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*设置停止位为1位*/
    USART_InitStructure.USART_Parity = USART_Parity_No;        /*无奇偶校验*/
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*无硬件流控*/
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*发送和接收*/

    /*配置串口*/
    USART_Init(USART1, &USART_InitStructure);

    /*使能串口的发送和接收中断*/
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

    /* 使能串口 */
    USART_Cmd(USART1, ENABLE);

    return;
}

/* 关闭和通信模块通信使用的串口 */
void USARTIO_DeinitUSART1(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;    

    /* 去使能 */
    USART_Cmd(USART1, DISABLE);

    /* 去使能的发送和接收中断*/
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

    /* GPIO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Disable the debug USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*去使能串口时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);  
    /*去使能串口使用的GPIO时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);

    return;
}

/* 判断缓冲区中是否有完整的一行 */
int USARTIO_HaveLine(void *port)
{
    RingBuf* readBuf;

    if (USART1 == port){
        readBuf = &g_USAER1ReadBufHead;
    }
    else {
        return ERROR_FAILED;
    }

    if (RINGBUF_HAVELINE(readBuf)){
        return ERROR_SUCCESS;
    } else {
        return ERROR_FAILED;
    }
}



