/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"NetPreSetPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"

#include	"SystemSetPage.h"
#include	"NetSetPage.h"
#include	"WifiSetPage.h"
#include	"ServerSetPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/

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

unsigned char DspNetPreSetPage(void *  parm)
{
	SetGBSysPage(DspNetPreSetPage, DspSystemSetPage, NULL, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);

	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	unsigned short *pdata = NULL;
	
	pdata = MyMalloc((len/2)*sizeof(unsigned short));
	if(pdata == NULL)
		return;
	
	/*命令*/
	pdata[0] = pbuf[4];
	pdata[0] = (pdata[0]<<8) + pbuf[5];
	
	/*有线网设置*/
	if(pdata[0] == 0x2d00)
	{
		GBPageBufferFree();
		SetGBChildPage(DspNetSetPage);
		GotoGBChildPage(NULL);
	}
	/*wifi设置*/
	else if(pdata[0] == 0x2d01)
	{
		GBPageBufferFree();
		SetGBChildPage(DspWifiSetPage);
		GotoGBChildPage(NULL);
	}
	/*服务器设置*/
	else if(pdata[0] == 0x2d02)
	{
		GBPageBufferFree();
		SetGBChildPage(DspServerSetPage);
		GotoGBChildPage(NULL);
	}
	/*返回*/
	else if(pdata[0] == 0x2d03)
	{
		GBPageBufferFree();
		GotoGBParentPage(NULL);
	}

	MyFree(pdata);
}

static void PageUpDate(void)
{

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
	SelectPage(78);
	
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



