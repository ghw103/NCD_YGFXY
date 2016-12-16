#ifndef __WELCOME_P_H__
#define __WELCOME_P_H__


#include	"Timer_Data.h"

typedef struct WelcomePageBuffer_tag {
	Timer timer;													//页面超时时间
	unsigned char tempv1;											//临时数据
	unsigned short lcdinput[100];									//lcd输入解析
}WelcomePageBuffer;

unsigned char DspWelcomePage(void *  parm);

#endif

