#ifndef __OTHERSET_P_H__
#define __OTHERSET_P_H__

#include	"Define.h"

typedef struct OtherSetPageBuffer_tag {
	MyTime_Def temptime;
	char buf[50];
}OtherSetPageBuffer;

unsigned char DspOtherSetPage(void *  parm);

#endif

