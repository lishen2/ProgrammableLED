#ifndef _USART_IO_H_
#define _USART_IO_H_

/* 接口暴露给中断处理函数使用 */
extern RingBuf g_USAER1ReadBufHead;
extern RingBuf g_USAER1WriteBufHead;

/* @brief 向指定串口发送一个字符
* @param void* port           指定端口，比如: USART1
* @param unsigned char ch   要发送的字符
* @return 成功返回ERROR_SUCCESS,失败返回ERROR_FAILED
*/
int USARTIO_SendChar(void* port, unsigned char ch);

/* @brief 向指定串口读取一个字符
* @param void* port           指定端口，比如: USART1
* @param unsigned char ch   要读取的字符
* @return 成功返回ERROR_SUCCESS,失败返回ERROR_FAILED
*/
int USARTIO_RecvChar(void* port, unsigned char* ch);

/* 想指定端口发送一个字符串 */
int USARTIO_SendString(void* port, unsigned char* str);

/* 从指定端口接收一行 */
int USARTIO_ReadLine(void* port, unsigned char* buf, int bufsize);

/* 初始化串口硬件 */
void USARTIO_InitUSART1(void);

/* 关闭硬件，节约电能 */
void USARTIO_DeinitUSART1(void);

/* 判断缓存是否为空 */
int USARTIO_HaveLine(void *port);

#endif

