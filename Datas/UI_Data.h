#ifndef UI_D_H
#define UI_D_H

#include	"MyStack.h"
#include	"Define.h"

MyState_TypeDef startActivity(MyState_TypeDef (* pageCreate)(Activity * thizactivity, void * pram), void * pram);
MyState_TypeDef backToActivity(char * pageName);
void InitActivity(Activity * activity, char * activityName, void (* pageStart)(void), 
	void (* pageInput)(unsigned char *pbuf , unsigned short len), 
	void (* pageFresh)(void),
	void (* pageHide)(void),
	void (* pageResume)(void),
	void (* pageDestroy)(void));

void activityInputFunction(unsigned char *pbuf , unsigned short len);
void activityFreshFunction(void);
	
#endif

