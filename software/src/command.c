#include <string.h>
#include "stm32f10x.h"
#include "assert.h"
#include "ringbuf.h"
#include "usart_io.h"
#include "utils.h"
#include "command.h"
#include "sequence.h"
#include "led.h"

#define CMD_MSG_BUFFER_SIZE   512
#define CMD_USART_PORT        USART1

//消息读取超时时间
#define CMD_RETRY_COUNT  10
#define CMD_RETRY_TIMES  50

/* 响应 */
#define CMD_RES_STR_SUCCESS  "OK\r\n"              /* 命令执行成功 */
#define CMD_RES_STR_FAILED   "ERROR\r\n"           /* 命令执行失败 */
#define CMD_RES_STR_CONTINUE "CONTINUE\r\n"        /* 命令没有发送完整，继续发送 */

/* 发送成功的相应 */
#define CMD_RESPONSE_SUCCESS() \
    USARTIO_SendString(CMD_USART_PORT, CMD_RES_STR_SUCCESS);

/* 发送失败的响应 */
#define CMD_RESPONSE_FAILED() \
    USARTIO_SendString(CMD_USART_PORT, CMD_RES_STR_FAILED);

/* 继续发送 */
#define CMD_RESPONSE_CONTINUE() \
	USARTIO_SendString(CMD_USART_PORT, CMD_RES_STR_CONTINUE);

/* 命令 */
#define CMD_STR_SET_LED1_RED            "SET_LED1_RED"
#define CMD_STR_SET_LED1_GREEN          "SET_LED1_GREEN"
#define CMD_STR_SET_LED1_BLUE           "SET_LED1_BLUE"
#define CMD_STR_SET_LED2_RED            "SET_LED1_RED"
#define CMD_STR_SET_LED2_GREEN          "SET_LED1_GREEN"
#define CMD_STR_SET_LED2_BLUE           "SET_LED1_BLUE"
#define CMD_STR_SET_LED3                "SET_LED3"
#define CMD_STR_SET_LED4                "SET_LED4"
#define CMD_STR_SET_MODE                "SET_MODE"

/* 通信命令token */
enum CmdToken{
	CMD_TOKEN_SET_LED1_RED   = 0,
	CMD_TOKEN_SET_LED1_GREEN,
	CMD_TOKEN_SET_LED1_BLUE,
	CMD_TOKEN_SET_LED2_RED,
	CMD_TOKEN_SET_LED2_GREEN,
	CMD_TOKEN_SET_LED2_BLUE,
	CMD_TOKEN_SET_LED3,
	CMD_TOKEN_SET_LED4,
	CMD_TOKEN_SET_MODE,
	CMD_TOKEN_UNKNOWN
};

/* 需要和token一一对应 */
const char* g_cmdStr[] = {
	CMD_STR_SET_LED1_RED,   
	CMD_STR_SET_LED1_GREEN, 
	CMD_STR_SET_LED1_BLUE,  
	CMD_STR_SET_LED2_RED,   
	CMD_STR_SET_LED2_GREEN, 
	CMD_STR_SET_LED2_BLUE,  
	CMD_STR_SET_LED3,       
	CMD_STR_SET_LED4,
	CMD_STR_SET_MODE,       
};

enum CmdStatus{
	CMD_STATUS_START,
	CMD_STATUS_FINISHED
};

static char g_msgBuf[CMD_MSG_BUFFER_SIZE];

//当前命令解析的状态
static u32 g_cmdStatus;

static u32* g_curDisBuf = g_bicycleSeq;

/* 解析命令 */
static int _parserCmd(char* msgBuf, char** pparg)
{
    char *sp, *cmd, *arg;
    int token, i;

    cmd = msgBuf;
    
    sp = strchr(msgBuf, ':');
    if (NULL == sp){
        arg = NULL;
    } else {
        *sp = '\0';
        arg = ++sp;
    }
    *pparg = arg;

    token = CMD_TOKEN_UNKNOWN;
    for(i = 0; i < sizeof(g_cmdStr)/sizeof(char*); ++i){
        if (0 == strcmp(cmd, g_cmdStr[i])){
            token = i;
			break;
        }
    }

    return token;
}

static void _setLED1Red(u32* buf, char* arg)
{
	int i;
	u32 tmp;
	u8  bright;

	for (i = 0; i < SEQ_LED_SEQUENCE_LEN && '\0' != arg[i]; ++i){
		tmp = buf[i];
		bright = UTILS_Char2Num(arg[i]);
		LED1_SET_RED(tmp, bright);
	}	 

	SEQ_SaveToFlash();

	return;
}

static void _setLED1Green(u32* buf, char* arg)
{
	int i;
	u32 tmp;
	u8  bright;

	for (i = 0; i < SEQ_LED_SEQUENCE_LEN && '\0' != arg[i]; ++i){
		tmp = buf[i];
		bright = UTILS_Char2Num(arg[i]);
		LED1_SET_GREEN(tmp, bright);
	}

	SEQ_SaveToFlash();

	return;
}

static void _setLED1Blue(u32* buf, char* arg)
{
	int i;
	u32 tmp;
	u8  bright;

	for (i = 0; i < SEQ_LED_SEQUENCE_LEN && '\0' != arg[i]; ++i){
		tmp = buf[i];
		bright = UTILS_Char2Num(arg[i]);
		LED1_SET_BLUE(tmp, bright);
	}

	SEQ_SaveToFlash();

	return;
}

static void _setLED2Red(u32* buf, char* arg)
{
	int i;
	u32 tmp;
	u8  bright;

	for (i = 0; i < SEQ_LED_SEQUENCE_LEN && '\0' != arg[i]; ++i){
		tmp = buf[i];
		bright = UTILS_Char2Num(arg[i]);
		LED2_SET_RED(tmp, bright);
	}

	SEQ_SaveToFlash();

	return;
}

static void _setLED2Green(u32* buf, char* arg)
{
	int i;
	u32 tmp;
	u8  bright;

	for (i = 0; i < SEQ_LED_SEQUENCE_LEN && '\0' != arg[i]; ++i){
		tmp = buf[i];
		bright = UTILS_Char2Num(arg[i]);
		LED2_SET_GREEN(tmp, bright);
	}

	SEQ_SaveToFlash();

	return;
}

static void _setLED2Blue(u32* buf, char* arg)
{
	int i;
	u32 tmp;
	u8  bright;

	for (i = 0; i < SEQ_LED_SEQUENCE_LEN && '\0' != arg[i]; ++i){
		tmp = buf[i];
		bright = UTILS_Char2Num(arg[i]);
		LED2_SET_BLUE(tmp, bright);
	}
 
	SEQ_SaveToFlash();

	return;
}

static void _setLED3(u32* buf, char* arg)
{
	int i;
	u32 tmp;
	u8  bright;

	for (i = 0; i < SEQ_LED_SEQUENCE_LEN && '\0' != arg[i]; ++i){
		tmp = buf[i];
		bright = UTILS_Char2Num(arg[i]);
		LED3_SET_COLOR(tmp, bright);
	}

	SEQ_SaveToFlash();

	return;
}

static void _setLED4(u32* buf, char* arg)
{
	int i;
	u32 tmp;
	u8  bright;

	for (i = 0; i < SEQ_LED_SEQUENCE_LEN && '\0' != arg[i]; ++i){
		tmp = buf[i];
		bright = UTILS_Char2Num(arg[i]);
		LED4_SET_COLOR(tmp, bright);
	}

	SEQ_SaveToFlash();

	return;
}

static void _setMode(char* arg)
{
	int ret;

	if (0 == strcmp(arg, "BREAK_LIGHT")){
		g_curDisBuf = g_bicycleSeq;	
		ret = ERROR_SUCCESS;
	} else if (0 == strcmp(arg, "CUSTOM1")) {
		g_curDisBuf = g_leftSeq;
		ret = ERROR_SUCCESS;
	} else if (0 == strcmp(arg, "CUSTOM2")) {
		g_curDisBuf = g_rightSeq;
		ret = ERROR_SUCCESS;
	} else {
		ret = ERROR_FAILED;
	}

	if (ERROR_SUCCESS == ret){
		CMD_RESPONSE_SUCCESS();
	} else {
		CMD_RESPONSE_FAILED();
	}

	return;
}

static void _handleOneCommand(char* buf)
{
	char *arg; 
    int token;
    
    /* 解析命令 */
    token = _parserCmd(buf, &arg);
	/* 处理命令 */
    switch (token)
    {
        case CMD_TOKEN_SET_LED1_RED:
        {
			_setLED1Red(g_curDisBuf, arg);
            break;
        } 
        case CMD_TOKEN_SET_LED1_GREEN:
        {
			_setLED1Green(g_curDisBuf, arg);
            break;
        } 
        case CMD_TOKEN_SET_LED1_BLUE:
        {
			_setLED1Blue(g_curDisBuf, arg);
            break;
        } 
        case CMD_TOKEN_SET_LED2_RED:
        {
			_setLED2Red(g_curDisBuf, arg);
            break;
        } 
        case CMD_TOKEN_SET_LED2_GREEN:
        {
			_setLED2Green(g_curDisBuf, arg);
            break;
        } 
        case CMD_TOKEN_SET_LED2_BLUE:
        {
			_setLED2Blue(g_curDisBuf, arg);
            break;
        } 
	    case CMD_TOKEN_SET_LED3:
        {
			_setLED3(g_curDisBuf, arg);
            break;
        } 
	    case CMD_TOKEN_SET_LED4:
        {
			_setLED4(g_curDisBuf, arg);
            break;
        } 
		case CMD_TOKEN_SET_MODE:
		{
			_setMode(arg);
		}				            
        default:
        {
			CMD_RESPONSE_FAILED();
            break;
        }
    };//switch
}

void CMD_HandleCommand(void)
{
	int retry, ret;

	g_cmdStatus = CMD_STATUS_START;
	for(retry = 0; (retry < CMD_RETRY_COUNT) && (CMD_STATUS_FINISHED != g_cmdStatus);){

		/* 读取命令 */
		g_msgBuf[0] = '\0';
	    ret = USARTIO_ReadLine(CMD_USART_PORT, (unsigned char*)g_msgBuf, sizeof(g_msgBuf));

		//如果读取成功，执行命令
		if (ERROR_SUCCESS == ret && '\0' != g_msgBuf[0]){
			_handleOneCommand(g_msgBuf);
			retry = 0;		
		} else {
			delay_ms(CMD_RETRY_TIMES);
			++retry;		
		}
	}

	return;
}


