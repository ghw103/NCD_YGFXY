#ifndef __WELCOME_P_H__
#define __WELCOME_P_H__

#include	"MyStack.h"
#include	"Timer_Data.h"
#include	"SystemSet_Data.h"

typedef struct WelcomePageBuffer_tag {
	SystemSetData systemSetData;
	Timer timer;													//页面超时时间
	unsigned char tempv1;											//临时数据
	unsigned short lcdinput[100];									//lcd输入解析
}WelcomePageBuffer;

MyState_TypeDef createWelcomeActivity(Activity * thizActivity, void * pram);

#endif

