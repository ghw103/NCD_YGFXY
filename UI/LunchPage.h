#ifndef __LUNCH_P_H__
#define __LUNCH_P_H__

#include	"Timer_Data.h"

typedef struct LunchPageBuffer_tag {
	Timer timer;
	unsigned char presscount;
}LunchPageBuffer;

unsigned char DspLunchPage(void *  parm);

#endif

