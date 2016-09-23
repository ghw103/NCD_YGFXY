#ifndef UI_D_H
#define UI_D_H

#include	"Define.h"

void SetGBSysPage(
	unsigned char (*CurrentPage)(void * pram), 								//当前页面
	unsigned char (*ParentPage)(void *  parm),								//父页面
	unsigned char (*ChildPage)(void *  parm),								//子页面
	void (*LCDInput)(unsigned char *pbuf , unsigned short len),				//页面输入
	void (*PageUpDate)(void),												//页面更新
	MyState_TypeDef (*PageInit)(void * pram),								//页面初始化
	MyState_TypeDef (*PageBufferMalloc)(void),								//页面缓存申请
	MyState_TypeDef (*PageBufferFree)(void)									//页面缓存释放
	);
SysPage * GetGBSysPage(void);
	
void SetGBCurrentPage(unsigned char (*CurrentPage)(void * pram));
unsigned char (*GetGBCurrentPage(void))(void* pram);

void SetGBParentPage(unsigned char (*ParentPage)(void *  parm));
unsigned char GotoGBParentPage(void *  parm);

void SetGBChildPage(unsigned char (*ChildPage)(void *  parm));
unsigned char GotoGBChildPage(void *  parm);

void SetGBPageUpDate(void (*PageUpDate)(void));
void GBPageUpDate(void);

void SetGBGBPageInput(void (*LCDInput)(unsigned char *pbuf , unsigned short len));
void GBPageInput(unsigned char *pbuf , unsigned short len);

void SetGBPageInit(MyState_TypeDef (*PageInit)(void * pram));
MyState_TypeDef GBPageInit(void * parm);

void SetGBPageBufferMalloc(MyState_TypeDef (*PageBufferMalloc)(void));
MyState_TypeDef GBPageBufferMalloc(void);

void SetGBPageBufferFree(MyState_TypeDef (*PageBufferFree)(void));
MyState_TypeDef GBPageBufferFree(void);
	
#endif

