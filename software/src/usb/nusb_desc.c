#include "stm32f10x.h"
#include "nusb_desc.h"
#include "nusb_hw.h"
#include "nusb_assert.h"

#define MASS_SIZ_STRING_SERIAL            2 * 16 + 2

uint8_t MASS_StringSerial[MASS_SIZ_STRING_SERIAL] =
{
    MASS_SIZ_STRING_SERIAL,
    0x03,
    /* Serial number*/
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define         ID1          (0x1FFFF7E8)
#define         ID2          (0x1FFFF7EC)
#define         ID3          (0x1FFFF7F0)

static unsigned char _num2Char(unsigned char num){
    if (num < 10){
        return num + '0';
    } else if (num >= 10 && num < 16){
        return num - 10 + 'A';
    } else {
        return 0;
    }
}

static void _u32ToChar(u32 num, char* buf){
	u8 *pu8 = (u8*)&num;

	buf[0] = _num2Char(pu8[0] >> 4);
	buf[1] = _num2Char(pu8[0]);
	buf[2] = _num2Char(pu8[1] >> 4);
	buf[3] = _num2Char(pu8[1]);
	buf[4] = _num2Char(pu8[2] >> 4);
	buf[5] = _num2Char(pu8[2]);
	buf[6] = _num2Char(pu8[3] >> 4);
	buf[7] = _num2Char(pu8[3]);

	return;
} 

void _generateSerialNum(char* buf, int len)
{
	u32 Device_Serial0, Device_Serial1;
	
	NUSB_ASSERT(len >= 16);

	Device_Serial0 = (*(uint32_t*)ID1) ^ (*(uint32_t*)ID2);
	Device_Serial1 = (*(uint32_t*)ID2) ^ (*(uint32_t*)ID3);

	_u32ToChar(Device_Serial0, buf + 0);
	_u32ToChar(Device_Serial1, buf + 8);
	
	return;	
}

void NUSB_UpdateSerialNum(void)
{
	char serialNum[16];
	int i;
	
	_generateSerialNum(serialNum, sizeof(serialNum));
	
	for(i = 0; i < sizeof(serialNum); ++i){
		MASS_StringSerial[i * 2 + 2] = serialNum[i];
	}	

	return;
}

