#ifndef __SHOWRESULT_P_H__
#define __SHOWRESULT_P_H__

#include	"MyTest_Data.h"

typedef struct ShowPageBuffer_tag {
	char tempbuf[100];
	Basic_ICO myico[3];
	TestData * testdata;
	LineInfo lineinfo;
}ShowPageBuffer;

unsigned char DspShowResultPage(void *  parm);

#endif

