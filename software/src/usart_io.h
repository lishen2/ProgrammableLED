#ifndef _USART_IO_H_
#define _USART_IO_H_

/* �ӿڱ�¶���жϴ�����ʹ�� */
extern RingBuf g_USAER1ReadBufHead;
extern RingBuf g_USAER1WriteBufHead;

/* @brief ��ָ�����ڷ���һ���ַ�
* @param void* port           ָ���˿ڣ�����: USART1
* @param unsigned char ch   Ҫ���͵��ַ�
* @return �ɹ�����ERROR_SUCCESS,ʧ�ܷ���ERROR_FAILED
*/
int USARTIO_SendChar(void* port, unsigned char ch);

/* @brief ��ָ�����ڶ�ȡһ���ַ�
* @param void* port           ָ���˿ڣ�����: USART1
* @param unsigned char ch   Ҫ��ȡ���ַ�
* @return �ɹ�����ERROR_SUCCESS,ʧ�ܷ���ERROR_FAILED
*/
int USARTIO_RecvChar(void* port, unsigned char* ch);

/* ��ָ���˿ڷ���һ���ַ��� */
int USARTIO_SendString(void* port, unsigned char* str);

/* ��ָ���˿ڽ���һ�� */
int USARTIO_ReadLine(void* port, unsigned char* buf, int bufsize);

/* ��ʼ������Ӳ�� */
void USARTIO_InitUSART1(void);

/* �ر�Ӳ������Լ���� */
void USARTIO_DeinitUSART1(void);

/* �жϻ����Ƿ�Ϊ�� */
int USARTIO_HaveLine(void *port);

#endif

