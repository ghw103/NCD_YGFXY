#ifndef UI_D_H
#define UI_D_H

#include	"Define.h"

//保存一个页面信息
#pragma pack(1)
typedef struct PageInfo_Tag
{
	unsigned char (*CurrentPage)(void * pram);
	void * pram;														//传进界面的参数
	void * tempP;														//备用指针
	void (*LCDInput)(unsigned char *pbuf , unsigned short len);
	void (*PageUpDate)(void);
	MyState_TypeDef (*PageInit)(void * pram);
	MyState_TypeDef (*PageBufferMalloc)(void);
	MyState_TypeDef (*PageBufferFree)(void);
	
	unsigned char (*ParentPage)(void *  parm);
	unsigned char (*ChildPage)(void *  parm);
}PageInfo;
#pragma pack()

//界面级联，链栈
#pragma pack(1)
typedef struct PageStackNode
{
    PageInfo * pageinfo;											//当前节点的页面信息
    struct PageStackNode * lastpagenode;						//上一节点地址
}PageStackNode;
#pragma pack()

#pragma pack(1)
typedef struct
{
    struct PageStackNode * top;									//栈顶节点
}PageLinkStack;
#pragma pack()

typedef enum
{ 
	NoDisplayPage = 0,											//不显示
	DisplayPage = 1												//显示
}DisplayType;

#define	OriginPageIndex			3								//所有子界面的起始界面索引，本系统来说欢迎动画为1，自检为2，主界面为3
#define	OriginPage				0xff							//回到起始界面的层级
#define	ParentPage				1								//对于当前界面来说，回父界面，即是切换到前一个界面

MyState_TypeDef PageAdvanceTo(unsigned char (*page)(void * pram), void * pram);
MyState_TypeDef PageBackTo(unsigned char index);
MyState_TypeDef GetCurrentPage(PageInfo ** pageinfo);
MyState_TypeDef PageResetToOrigin(DisplayType distype);

#endif

