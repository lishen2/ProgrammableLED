#ifndef _ASSERT_H_
#define _ASSERT_H_

#ifdef DEBUG

#define ASSERT(exp); \
	if (!(exp)){ \
		printf("ASSERT fault at %s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);\
		while(1); \
	}
	
#else

#define ASSERT(exp); 

#endif



#endif

