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




/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

static SysPage GB_SysPage;												//系统界面数据



/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/







/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：SetGBSysPage
*Description：一次设置页面所有数据
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日17:13:18
***************************************************************************************************/
void SetGBSysPage(
	unsigned char (*CurrentPage)(void * pram), 								//当前页面
	unsigned char (*ParentPage)(void *  parm),								//父页面
	unsigned char (*ChildPage)(void *  parm),								//子页面
	void (*LCDInput)(unsigned char *pbuf , unsigned short len),				//页面输入
	void (*PageUpDate)(void),												//页面更新
	MyState_TypeDef (*PageInit)(void * pram),								//页面初始化
	MyState_TypeDef (*PageBufferMalloc)(void),								//页面缓存申请
	MyState_TypeDef (*PageBufferFree)(void)									//页面缓存释放
	)
{
	GB_SysPage.CurrentPage = CurrentPage;
	GB_SysPage.ParentPage = ParentPage;
	GB_SysPage.ChildPage = ChildPage;
	GB_SysPage.LCDInput = LCDInput;
	GB_SysPage.PageUpDate = PageUpDate;
	GB_SysPage.PageInit = PageInit;
	GB_SysPage.PageBufferMalloc = PageBufferMalloc;
	GB_SysPage.PageBufferFree = PageBufferFree;
}

/***************************************************************************************************
*FunctionName：SetGBCurrentPage, GetGBCurrentPage
*Description: 设置，获取当前页面
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日19:36:13
***************************************************************************************************/
void SetGBCurrentPage(unsigned char (*CurrentPage)(void * pram))
{
	GB_SysPage.CurrentPage = CurrentPage;
}
unsigned char (*GetGBCurrentPage(void))(void* pram)
{
	return GB_SysPage.CurrentPage;
}

/***************************************************************************************************
*FunctionName：SetParentPage
*Description：设置父页面地址
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日17:18:36
***************************************************************************************************/
void SetGBParentPage(unsigned char (*ParentPage)(void *  parm))
{
	GB_SysPage.ParentPage = ParentPage;
}

/***************************************************************************************************
*FunctionName：GotoParentPage
*Description：切换页面到父页面
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日17:19:02
***************************************************************************************************/
unsigned char GotoGBParentPage(void *  parm)
{
	return GB_SysPage.ParentPage(parm);
}

/***************************************************************************************************
*FunctionName：SetChildPage
*Description：设置子页面
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日17:19:58
***************************************************************************************************/
void SetGBChildPage(unsigned char (*ChildPage)(void *  parm))
{
	GB_SysPage.ChildPage = ChildPage;
}

/***************************************************************************************************
*FunctionName：GotoChildPage
*Description：切换到子页面
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日17:20:24
***************************************************************************************************/
unsigned char GotoGBChildPage(void *  parm)
{
	return GB_SysPage.ChildPage(parm);
}

/***************************************************************************************************
*FunctionName：GBPageUpDate
*Description：刷新当前页面
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日17:25:20
***************************************************************************************************/
void SetGBPageUpDate(void (*PageUpDate)(void))
{
	GB_SysPage.PageUpDate = PageUpDate;
}
void GBPageUpDate(void)
{
	GB_SysPage.PageUpDate();
}

/***************************************************************************************************
*FunctionName：GBPageInput
*Description：lcd输入
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日19:44:56
***************************************************************************************************/
void SetGBGBPageInput(void (*LCDInput)(unsigned char *pbuf , unsigned short len))
{
	GB_SysPage.LCDInput = LCDInput;
}
void GBPageInput(unsigned char *pbuf , unsigned short len)
{
	GB_SysPage.LCDInput(pbuf, len);
}

/***************************************************************************************************
*FunctionName：GBPageInit
*Description：初始化当前页面
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日17:26:42
***************************************************************************************************/
void SetGBPageInit(MyState_TypeDef (*PageInit)(void * pram))
{
	GB_SysPage.PageInit = PageInit;
}
MyState_TypeDef GBPageInit(void * parm)
{
	return GB_SysPage.PageInit(parm);
}

/***************************************************************************************************
*FunctionName：GBPageBufferMalloc
*Description：当前页面缓存申请
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日17:28:16
***************************************************************************************************/
void SetGBPageBufferMalloc(MyState_TypeDef (*PageBufferMalloc)(void))
{
	GB_SysPage.PageBufferMalloc = PageBufferMalloc;
}
MyState_TypeDef GBPageBufferMalloc(void)
{
	return GB_SysPage.PageBufferMalloc();
}

/***************************************************************************************************
*FunctionName：GBPageBufferFree
*Description：当前页面缓存释放
*Input：None
*Output：None
*Author：xsx
*Data：2016年8月3日17:28:35
***************************************************************************************************/
void SetGBPageBufferFree(MyState_TypeDef (*PageBufferFree)(void))
{
	GB_SysPage.PageBufferFree = PageBufferFree;
}
MyState_TypeDef GBPageBufferFree(void)
{
	return GB_SysPage.PageBufferFree();
}
