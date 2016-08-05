#ifndef __LUNCH_P_H__
#define __LUNCH_P_H__

#include	"Timer_Data.h"

typedef struct LunchPageBuffer_tag {
	Timer timer;
	unsigned char presscount;
	char buf[100];
	unsigned short lcdinput[100];
	unsigned char error;
}LunchPageBuffer;

unsigned char DspLunchPage(void *  parm);

#endif

