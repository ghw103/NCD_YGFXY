#ifndef __PAIDUI_P_H__
#define __PAIDUI_P_H__

#include	"Define.h"
#include	"MyTest_Data.h"

typedef struct PaiDuiPageBuffer_tag {
	TestData tempd;
	ItemData *tempd2;
	Basic_ICO myico;
	Timer timer0;
	unsigned short tarindex;
	float tempvalue;
	unsigned int tempvalue1;
	char buf[100];
	char buf2[100];
	unsigned short lcdinput[100];
	CreateTestErrorType error;
	unsigned short count;
	unsigned short count2;
	bool pageisbusy;
}PaiDuiPageBuffer;


unsigned char DspPaiDuiPage(void *  parm);
void CheckTime(void);

#endif

