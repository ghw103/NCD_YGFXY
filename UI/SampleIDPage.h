#ifndef __SampleID_P_H__
#define __SampleID_P_H__

#include	"Define.h"
#include	"Timer_Data.h"
#include	"MyTest_Data.h"

typedef struct SampleIDPage_tag {
	char tempid[MaxSampleIDLen];				//临时的条码数据
	char *tempbuf;								//临时指针
	Timer timer;				//一个计时器
	ItemData * currenttestdata;												//当前测试数据
}SampleIDPage;

unsigned char DspSampleIDPage(void *  parm);

#endif

