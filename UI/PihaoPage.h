#ifndef __PIHAO_P_H__
#define __PIHAO_P_H__

#include	"Define.h"
#include	"Timer_Data.h"
#include	"MyTest_Data.h"

typedef struct PihaoPage_tag {
	Timer timer;															//一个计时器
	unsigned char tempbuf[150];
	unsigned int tempv1;
	ItemData * currenttestdata;												//当前测试数据
	unsigned short lcdinput[100];
}PihaoPage;

unsigned char DspPihaoPage(void *  parm);

#endif

