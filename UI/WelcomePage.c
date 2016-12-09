/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"WelcomePage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"

#include	"PlaySong_Task.h"
#include	"SelfCheckPage.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static unsigned short timecount = 0;								//动画时间
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);
static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName：DspWelcomePage
*Description：欢迎界面
*Input：parm -- 参数
*Output：None
*Author：xsx
*Data：2016年6月27日08:53:22
***************************************************************************************************/
unsigned char DspWelcomePage(void *  parm)
{
	PageInfo * currentpage = NULL;
	
	if(My_Pass == GetCurrentPage(&currentpage))
	{
		currentpage->PageInit = PageInit;
		currentpage->PageUpDate = PageUpDate;
		
		currentpage->PageInit(currentpage->pram);
	}
	
	return 0;
}

/***************************************************************************************************
*FunctionName：Input
*Description：接收lcd输入
*Input：pbuf -- 输入数据
*		len -- 输入数据长度
*Output：None
*Author：xsx
*Data：2016年6月27日08:54:10
***************************************************************************************************/
static void Input(unsigned char *pbuf , unsigned short len)
{
	

}

/***************************************************************************************************
*FunctionName：PageUpData
*Description：界面数据更新，10ms执行一次
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月27日08:55:02
***************************************************************************************************/
static void PageUpDate(void)
{
	timecount++;
	
	if(timecount >= 800)
	{
		timecount = 0;
		PageAdvanceTo(DspSelfCheckPage, NULL);
	}
}

/***************************************************************************************************
*FunctionName：PageInit
*Description：当前界面初始化
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月27日08:55:25
***************************************************************************************************/
static MyState_TypeDef PageInit(void *  parm)
{
	SetLEDLight(100);
	
	SelectPage(0);
	
	AddNumOfSongToList(52, 0);
	
	return My_Pass;
}

/***************************************************************************************************
*FunctionName：PageBufferMalloc
*Description：当前界面临时缓存申请
*Input：None
*Output：MyState_TypeDef -- 返回成功与否
*Author：xsx
*Data：2016年6月27日08:56:02
***************************************************************************************************/
static MyState_TypeDef PageBufferMalloc(void)
{
	return My_Pass;
}

/***************************************************************************************************
*FunctionName：PageBufferFree
*Description：当前界面临时缓存释放
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月27日08:56:21
***************************************************************************************************/
static MyState_TypeDef PageBufferFree(void)
{
	return My_Pass;
}


