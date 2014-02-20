#ifndef _NUSB_ASSERT_H_
#define _NUSB_ASSERT_H_

#ifdef DEBUG

#include <stdio.h>

#define NUSB_ASSERT(exp); \
	if (!(exp)){ \
		printf("ASSERT fault at %s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);\
		while(1); \
	}
	
#else

#define NUSB_ASSERT(exp); 

#endif
#endif

