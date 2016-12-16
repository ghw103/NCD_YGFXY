/***************************************************************************************************
*FileName:
*Description:
*Author:xsx
*Data:
***************************************************************************************************/


/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"UI_Data.h"

#include	"MyMem.h"

#include	<string.h>
#include	"stdio.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

static PageLinkStack GB_PageLinkStack = 
{
	.top = NULL
};								//页面信息链栈


/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static MyState_TypeDef PageLinkStackPush(PageLinkStack * s_linkstack, PageInfo * pageinfo);
static MyState_TypeDef PageLinkStackPop(PageLinkStack * s_linkstack, PageInfo ** pageinfo);
static MyState_TypeDef PageLinkStackTop(PageLinkStack * s_linkstack, PageInfo ** pageinfo);

static MyState_TypeDef PageDetroyBackNum(unsigned char index);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: PageAdvanceTo
*Description:  页面往前切换到某个页面，对于链栈入栈操作，注意此处是直接显示界面，并不是切换指针
*Input: page -- 前进页面地址
		pram -- 传递到前进页面参数
*Output: None
*Return: My_Fail -- 前进失败
*		My_Pass -- 前进成功
*Author: xsx
*Date: 2016年12月7日15:29:21
***************************************************************************************************/
MyState_TypeDef PageAdvanceTo(unsigned char (*page)(void * pram), void * pram)
{
	PageInfo * temppageinfo = NULL;
	
	//不允许跳转到无效界面
	if(NULL == page)
		return My_Fail;
	
	temppageinfo = MyMalloc(sizeof(PageInfo));
	
	if(temppageinfo)
	{
		memset(temppageinfo, 0, sizeof(PageInfo));
		temppageinfo->CurrentPage = page;
		
		if(My_Pass == PageLinkStackPush(&GB_PageLinkStack, temppageinfo))
		{
			temppageinfo->pram = pram;
			temppageinfo->CurrentPage(temppageinfo->pram);
			
			return My_Pass;
		}
		else
		{
			MyFree(temppageinfo);
			return My_Fail;
		}
	}
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName: PageDetroyBackNum
*Description: 往前销毁index个界面
*Input: index -- 往前销毁的界面数目
*Output: None
*Return: My_Fail -- 失败
*		My_Pass -- 成功
*Author: xsx
*Date: 2016年12月8日13:57:33
***************************************************************************************************/
static MyState_TypeDef PageDetroyBackNum(unsigned char index)
{
	PageInfo * temppageinfo = NULL;
	unsigned char i = index;
	
	//先出栈销毁一定深度的界面
	while(i)
	{
		if(My_Pass == PageLinkStackPop(&GB_PageLinkStack, &temppageinfo))
		{
			MyFree(temppageinfo);
			i--;
		}
		else
			break;
	}
	
	return My_Pass;
}
/***************************************************************************************************
*FunctionName: PageBackTo
*Description: 根据输入，页面返回一定的层数
*Input: index -- 返回层数，1表示返回上一个页面，2表示返回上上一个页面
*		pram -- 传入参数到返回的页面
*Output: 
*Return: My_Fail -- 失败
*		My_Pass -- 成功
*Author: xsx
*Date: 2016年12月7日15:58:31
***************************************************************************************************/
MyState_TypeDef PageBackTo(unsigned char index)
{
	PageInfo * temppageinfo = NULL;
	
	PageDetroyBackNum(index);
	
	//读取当前栈顶界面，且显示
	if(My_Pass == PageLinkStackTop(&GB_PageLinkStack, &temppageinfo))
	{	
		temppageinfo->CurrentPage(temppageinfo->pram);
		
		return My_Pass;
	}
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName: PageResetToOrigin
*Description: 回主界面
*Input: None
*Output: None
*Return: My_Fail -- 失败
*		My_Pass -- 成功
*Author: xsx
*Date: 2016年12月8日09:01:58
***************************************************************************************************/
MyState_TypeDef PageResetToOrigin(DisplayType distype)
{
	PageInfo * temppageinfo = NULL;
	
	PageDetroyBackNum(OriginPage);
	
	if(DisplayPage == distype)
	{
		//读取当前栈顶界面，且显示
		if(My_Pass == PageLinkStackTop(&GB_PageLinkStack, &temppageinfo))
		{
			temppageinfo->CurrentPage(temppageinfo->pram);
			
			return My_Pass;
		}
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: GetCurrentPage
*Description: 读取当前页面
*Input: pageinfo -- 当前页面数据存放地址
*Output: None
*Return: My_Fail -- 失败
*		My_Pass -- 成功
*Author: xsx
*Date: 2016年12月7日16:03:09
***************************************************************************************************/
MyState_TypeDef GetCurrentPage(PageInfo ** pageinfo)
{
	//读取当前栈顶界面
	if(My_Pass == PageLinkStackTop(&GB_PageLinkStack, pageinfo))
		return My_Pass;
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName: GetParentPage
*Description: 读取当前页面的父页面
*Input: pageinfo -- 当前页面数据存放地址
*Output: None
*Return: My_Fail -- 失败
*		My_Pass -- 成功
*Author: xsx
*Date: 2016年12月7日16:03:09
***************************************************************************************************/
unsigned char (*GetParentPage(void))(void* pram)
{
	PageStackNode * tempnode = GB_PageLinkStack.top;
	
	if(tempnode)
	{
		if(tempnode->lastpagenode)
		{
			return tempnode->lastpagenode->pageinfo->CurrentPage;
		}
	}
	
	return NULL;
}

/*###################################################################################################
#########################################链栈的基本操作##############################################
####################################################################################################*/

/***************************************************************************************************
*FunctionName: InitPageLinkStack
*Description: 初始化链栈
*Input: s_linkstack - 链栈地址
*Output: None
*Return: None
*Author: xsx
*Date: 2016年12月7日14:47:29
***************************************************************************************************/
void InitPageLinkStack(PageLinkStack * s_linkstack)
{
	s_linkstack->top = NULL;
}

/***************************************************************************************************
*FunctionName: PageLinkStackIsEmpty
*Description: 判断链栈是否为空
*Input: s_linkstack - 链栈地址
*Output: None
*Return: 	true -- 链栈空
*			false -- 链栈不为空
*Author: xsx
*Date: 2016年12月7日14:49:37
***************************************************************************************************/
bool PageLinkStackIsEmpty(PageLinkStack * s_linkstack)
{
	if(NULL == s_linkstack->top)
		return true;
	else
		return false;
}

/***************************************************************************************************
*FunctionName: PageLinkStackLength
*Description: 获得链栈的长度
*Input: s_linkstack - 链栈地址
*Output: None
*Return: len -- 链栈长度
*Author: xsx
*Date: 2016年12月7日14:55:19
***************************************************************************************************/
unsigned char PageLinkStackLength(PageLinkStack * s_linkstack)
{
	unsigned char len = 0;
	PageStackNode * tempnode = s_linkstack->top;
	
	while(NULL != tempnode)
	{
		len++;
		tempnode = tempnode->lastpagenode;
	}
	
	return len;
}

/***************************************************************************************************
*FunctionName: PageLinkStackPush
*Description: 入栈操作
*Input: s_linkstack -- 链栈地址
*		pageinfo -- 入栈的值
*Output: None
*Return: 	My_Pass -- 入栈成功
*			My_Fail -- 入栈失败
*Author: xsx
*Date: 2016年12月7日15:03:34
***************************************************************************************************/
static MyState_TypeDef PageLinkStackPush(PageLinkStack * s_linkstack, PageInfo * pageinfo)
{
	PageStackNode * pagenode = NULL;
	
	//不允许入栈空值
	if(NULL == pageinfo)
		return My_Fail;
	
	pagenode = MyMalloc(sizeof(PageStackNode));
	
	if(pagenode)
	{		
		pagenode->pageinfo = pageinfo;
		pagenode->lastpagenode = s_linkstack->top;
		
		s_linkstack->top = pagenode;
		
		return My_Pass;
	}
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName: PageLinkStackPop
*Description: 链栈出栈操作
*Input: s_linkstack -- 链栈地址
*		pageinfo -- 出栈数据存放地址
*Output: None
*Return: My_Fail -- 出栈失败
*		My_Pass -- 出栈成功
*Author: xsx
*Date: 2016年12月7日15:10:31
***************************************************************************************************/
static MyState_TypeDef PageLinkStackPop(PageLinkStack * s_linkstack, PageInfo ** pageinfo)
{
	PageStackNode * pagenode = NULL;
	
	//栈空或者只有1个节点，则不允许出栈，系统设计最底层节点为主界面
	if(PageLinkStackLength(s_linkstack) <= OriginPageIndex)
		return My_Fail;

	//不空
	else
	{
		//删除当前页面的数据空间
		if(s_linkstack->top)
		{
			if(s_linkstack->top->pageinfo->PageBufferFree)
				s_linkstack->top->pageinfo->PageBufferFree();
		}
		
		pagenode = s_linkstack->top;
		*pageinfo = pagenode->pageinfo;
		
		s_linkstack->top = pagenode->lastpagenode;
		
		MyFree(pagenode);
		
		return My_Pass;
	}
}

/***************************************************************************************************
*FunctionName: PageLinkStackTop
*Description: 读取栈顶节点中的值，但是不删除栈顶节点
*Input: s_linkstack -- 链栈
*		pageinfo -- 读取数据的存放地址
*Output: None
*Return: My_Fail -- 读取失败
*		My_Pass -- 读取成功
*Author: xsx
*Date: 2016年12月7日15:13:48
***************************************************************************************************/
static MyState_TypeDef PageLinkStackTop(PageLinkStack * s_linkstack, PageInfo ** pageinfo)
{	
	//栈空
	if(PageLinkStackIsEmpty(s_linkstack))
		return My_Fail;

	//不空
	else
	{
		*pageinfo = s_linkstack->top->pageinfo;

		return My_Pass;
	}
}


