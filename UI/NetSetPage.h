#ifndef __NETSET_P_H__
#define __NETSET_P_H__

#include	"Define.h"
#include	"SystemSet_Data.h"
#include	"UI_Data.h"

typedef struct NetSetPageBuffer_tag {
	NetSet myNetSet;
	SystemSetData systemSetData;
	char buf[100];
	unsigned char ischanged;
	unsigned short lcdinput[100];
}NetSetPageBuffer;


MyState_TypeDef createNetSetActivity(Activity * thizActivity, Intent * pram);

#endif

