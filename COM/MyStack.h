/****************************************file start****************************************************/
#ifndef	MYSTACK_H_H
#define	MYSTACK_H_H

#include	"Define.h"
#include	"Intent.h"

//保存一个页面信息

typedef struct Activity_Tag
{
	char * pageName;												//activity名字，指向一个唯一地址
	MyState_TypeDef (* pageCreate)(void * thizactivity, Intent * pram);					//配置每个页面具体的函数，申请页面内存
	void (* pageStart)(void);										//显示页面，初始化页面数据
	void (* pageInput)(unsigned char *pbuf , unsigned short len);		//获取页面输入
	void (* pageFresh)(void);											//页面刷新
	void (* pageHide)(void);											//页面隐藏，切换到子页面
	void (* pageResume)(void);											//从隐藏状态返回，重新显示
	void (* pageDestroy)(void);										//页面销毁，显示上一个页面
	
	Intent * pram;													//上一个页面传进来的参数地址，读取完需要释放
}Activity;


typedef struct _stacknode_
{
    Activity * activity;									//当前节点的页面信息
    struct _stacknode_ * lastStackNode;						//上一节点地址
}StackNode;


typedef struct
{
	StackNode * top;										//栈顶节点
}LinkStack;


void InitLinkStack(LinkStack * linkStack);
MyState_TypeDef StackPush(LinkStack * linkStack, Activity * nodeData);
MyState_TypeDef StackPop(LinkStack * linkStack, bool isResume);
MyState_TypeDef StackTop(LinkStack * linkStack, Activity ** activity);

#endif

/****************************************end of file************************************************/
