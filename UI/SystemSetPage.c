/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SystemSetPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"LunchPage.h"
#include	"ShowDeviceInfoPage.h"
#include	"AdjustPage.h"
#include	"ReTestPage.h"
#include	"UserMPage.h"
#include	"NetPreSetPage.h"
#include	"RecordPage.h"
#include	"OtherSetPage.h"
#include	"MyTools.h"

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

unsigned char DspSystemSetPage(void *  parm)
{
	SetGBSysPage(DspSystemSetPage, DspLunchPage, NULL, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
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
	
	/*基本信息*/
	if(pdata[0] == 0x2200)
	{
		SetGBChildPage(DspShowDeviceInfoPage);
		GotoGBChildPage(NULL);
	}
	/*操作人管理*/
	else if(pdata[0] == 0x2201)
	{
		SetGBChildPage(DspUserMPage);
		GotoGBChildPage(NULL);
	}
	/*网络设置*/
	else if(pdata[0] == 0x2202)
	{
		SetGBChildPage(DspNetPreSetPage);
		GotoGBChildPage(NULL);
	}
	/*数据管理*/
	else if(pdata[0] == 0x2203)
	{
		SetGBChildPage(DspRecordPage);
		GotoGBChildPage(NULL);
	}
	/*校准功能*/
	else if(pdata[0] == 0x2204)
	{
		SetGBChildPage(DspAdjustPage);
		GotoGBChildPage(NULL);
	}
	else if(pdata[0] == 0x2230)
	{
		if(GetBufLen(&pbuf[7] , 2*pbuf[6]) == 6)
		{
			if(pdPASS == CheckStrIsSame(&pbuf[7] , AdjustPassWord ,GetBufLen(&pbuf[7] , 2*pbuf[6])))
			{
				SetGBChildPage(DspAdjustPage);
				GotoGBChildPage(NULL);
			}
			else if(pdPASS == CheckStrIsSame(&pbuf[7] , TestPassWord ,GetBufLen(&pbuf[7] , 2*pbuf[6])))
			{
				SetGBChildPage(DspReTestPage);
				GotoGBChildPage(NULL);
			}
			else
				SendKeyCode(1);
		}
		else
				SendKeyCode(1);
	}
	/*其他设置*/
	else if(pdata[0] == 0x2205)
	{
		SetGBChildPage(DspOtherSetPage);
		GotoGBChildPage(NULL);
	}
	/*返回*/
	else if(pdata[0] == 0x2206)
	{
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
	SelectPage(68);
	
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



