#ifndef __UTILS_H
#define __UTILS_H

#define ERROR_ERROR -1
#define ERROR_FAILED ERROR_ERROR
#define ERROR_SUCCESS 0

#define inline __inline

//判断a是否在b之后
#define time_after(a, b) ((signed int)((signed int)(b) - (signed int)(a)) < 0)

/* 延时使用的计数器 */
extern vu32 g_jiffies;

/* 将字符转换为其对应的数字，可以处理16进制和大小写 */
static inline unsigned char UTILS_Char2Num(unsigned char ch){
	
	if (ch >= 48 && ch <= 57){
		return ch - 48;
	} else if (ch >= 65 && ch <= 70){
		return ch - 55;
	}  else if (ch >= 97 && ch <= 102){
		return ch - 87;
	} else {
		return 0;
	}
}

/* 将数字转化为其对应的字符，转换结果为16进制 */
static inline unsigned char UTILS_Num2Char(unsigned char num){
    if (num < 10){
        return num + '0';
    } else if (num >= 10 && num < 16){
        return num - 10 + 'A';
    } else {
        return 0;
    }
}

/* 将两个表示一个16进制数字的字母转化为16进制数 */
static inline unsigned char UTILS_ASCII2Bin(unsigned char high, 
                                            unsigned char low)
{
	high = UTILS_Char2Num(high);
	low = UTILS_Char2Num(low);

	return (high << 4) | low;
}

/* 毫秒延时 */
void delay_ms(int n);
void HW_CommonInit(void);
void HW_InitSysTick(void);

#endif /* __UITLS_H */

