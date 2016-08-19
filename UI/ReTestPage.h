#ifndef __RETSET_P_H__
#define __RETSET_P_H__

#include	"Define.h"

typedef struct ReTestPageBuffer_tag {
	unsigned short lcdinput[100];
	char buf[100];
	ReTestData retestdata;
	Timer timer;
	ScanCodeResult scancode;
	ResultState cardpretestresult;
	unsigned char playstatus;
}ReTestPageBuffer;


unsigned char DspReTestPage(void *  parm);

#endif

