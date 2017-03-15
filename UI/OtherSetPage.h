#ifndef __OTHERSET_P_H__
#define __OTHERSET_P_H__

#include	"Define.h"
#include	"SystemSet_Data.h"
#include	"UI_Data.h"

typedef struct OtherSetPageBuffer_tag {
	MyTime_Def temptime;
	SystemSetData systemSetData;
	bool isChange;
	char buf[50];
	unsigned short lcdinput[100];
	unsigned short tempvalue;
	Basic_ICO ico;
}OtherSetPageBuffer;

MyState_TypeDef createOtherSetActivity(Activity * thizActivity, Intent * pram);

#endif

