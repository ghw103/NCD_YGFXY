#ifndef UI_D_H
#define UI_D_H

#include	"MyStack.h"
#include	"Define.h"
#include	"Intent.h"

#define	lunchActivityName	"LunchActivity\0"
#define	paiduiActivityName	"PaiduiActivity\0"

MyState_TypeDef startActivity(MyState_TypeDef (* pageCreate)(Activity * thizactivity, Intent * pram), Intent * pram);
MyState_TypeDef backToActivity(char * pageName);
MyState_TypeDef backToFatherActivity(void);
void destroyTopActivity(void);
char * getFatherActivityName(void);
char * getCurrentActivityName(void);
bool checkFatherActivityIs(char * pageName);

void InitActivity(Activity * activity, char * activityName, void (* pageStart)(void), 
	void (* pageInput)(unsigned char *pbuf , unsigned short len), 
	void (* pageFresh)(void),
	void (* pageHide)(void),
	void (* pageResume)(void),
	void (* pageDestroy)(void));

void activityInputFunction(unsigned char *pbuf , unsigned short len);
void activityFreshFunction(void);
	
#endif

