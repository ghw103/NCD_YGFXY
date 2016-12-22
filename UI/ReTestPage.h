#ifndef __RETSET_P_H__
#define __RETSET_P_H__

#include	"Define.h"
#include	"UI_Data.h"

typedef struct ReTestPageBuffer_tag {
	unsigned short lcdinput[100];
	char buf[100];
	ReTestData retestdata;
	Timer timer;
	ScanCodeResult scancode;
	ResultState cardpretestresult;
	unsigned char playstatus;
}ReTestPageBuffer;


MyState_TypeDef createReTestActivity(Activity * thizActivity, Intent * pram);

#endif

