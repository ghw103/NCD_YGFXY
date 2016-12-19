#ifndef __OTHERSET_P_H__
#define __OTHERSET_P_H__

#include	"Define.h"
#include	"SystemSet_Data.h"

typedef struct OtherSetPageBuffer_tag {
	MyTime_Def temptime;
	SystemSetData mySystemSetData;
	bool isChange;
	char buf[50];
	unsigned short lcdinput[100];
	unsigned short tempvalue;
}OtherSetPageBuffer;

unsigned char DspOtherSetPage(void *  parm);

#endif

