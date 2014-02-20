#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "ringbuf.h"
#include "usart_io.h"
#include "utils.h"

#define USART_COMM_IO_READBUF_SIZE  128
#define USART_COMM_IO_WRITEBUF_SIZE 256

/* ����1ʹ�õĶ�д������ */
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

/* ���ô��ڲ���ʹ�õ����ݽṹ */
static USART_InitTypeDef USART_InitStructure;

/* ��ָ�����ڷ������� */
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

    /* ������д��д������ */
    ret = Ringbuf_PutChar(writeBuf, ch);
    
    /* ���ж� */
    USART_ITConfig(port, USART_IT_TXE, ENABLE);
    
    return ret;
}

/* ��ָ�����ڶ�ȡ���� */
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

/* ��ָ���˿ڷ���һ���ַ��� */
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
		/* ���ж� */
   	 	USART_ITConfig(port, USART_IT_TXE, ENABLE);

		if(ERROR_SUCCESS == ret){
			++i;
		}
    }

    printf("Send:%s", str);

    return ret;
}

/* �Ӵ����ж�ȡһ�в����뻺��
   ��ȡ�ɹ�����ERROR_SUCCESS
   ��ȡʧ�ܷ���ERROR_FAILED
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

    /* ��������л�û��һ���У�ֱ���˳� */
    if (!RINGBUF_HAVELINE(readBuf)){
        return ERROR_FAILED;
    }

    for (i = 0; i < bufsize;){      

        /* ��ȡ�ַ� */
        ret = Ringbuf_GetChar(readBuf, &ch);
        if (ERROR_SUCCESS != ret){
            break;
        }
        
        /* skip '\r' */
        if (ch == '\r'){
        } 
        /* �����\n�������ȡ */
        else if(ch == '\n') {
            ret = ERROR_SUCCESS;
            break;
        } 
        /* ��������������򱣴浽�������� */
        else {
            buf[i] = ch;
            ++i;
        }
    }

    /* ������ĩβ���Ͻ������� */
    (i == bufsize)?(buf[i - 1] = '\0'):(buf[i] = '\0');

    if (ERROR_SUCCESS == ret){
        printf("Read:%s\n", buf);
    }
  
    return ret;
}

/* ��ʼ����ͨ��ģ��ͨ��ʹ�õĴ��ڣ�ʹ�ô���2 */
void USARTIO_InitUSART1(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;    

    /*ʹ�ܴ���ʹ�õ�GPIOʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /*ʹ�ܴ���ʱ��*/
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
    USART_InitStructure.USART_BaudRate = 115200;                 /*���ò�����*/
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;/*��������λΪ8*/
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     /*����ֹͣλΪ1λ*/
    USART_InitStructure.USART_Parity = USART_Parity_No;        /*����żУ��*/
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/*��Ӳ������*/
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  /*���ͺͽ���*/

    /*���ô���*/
    USART_Init(USART1, &USART_InitStructure);

    /*ʹ�ܴ��ڵķ��ͺͽ����ж�*/
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

    /* ʹ�ܴ��� */
    USART_Cmd(USART1, ENABLE);

    return;
}

/* �رպ�ͨ��ģ��ͨ��ʹ�õĴ��� */
void USARTIO_DeinitUSART1(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;    

    /* ȥʹ�� */
    USART_Cmd(USART1, DISABLE);

    /* ȥʹ�ܵķ��ͺͽ����ж�*/
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

    /*ȥʹ�ܴ���ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);  
    /*ȥʹ�ܴ���ʹ�õ�GPIOʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);

    return;
}

/* �жϻ��������Ƿ���������һ�� */
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



