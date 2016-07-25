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
static void PageUpData(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspNetPreSetPage(void *  parm)
{
	SysPage * myPage = GetSysPage();

	myPage->CurrentPage = DspNetPreSetPage;
	myPage->LCDInput = Input;
	myPage->PageUpData = PageUpData;
	myPage->ParentPage = DspSystemSetPage;
	myPage->ChildPage = NULL;

	SelectPage(78);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	unsigned short *pdata = NULL;
	SysPage * myPage = GetSysPage();
	
	pdata = MyMalloc((len/2)*sizeof(unsigned short));
	if(pdata == NULL)
		return;
	
	/*命令*/
	pdata[0] = pbuf[4];
	pdata[0] = (pdata[0]<<8) + pbuf[5];
	
	/*有线网设置*/
	if(pdata[0] == 0x2d00)
	{
		myPage->ChildPage = DspNetSetPage;
		myPage->ChildPage(NULL);
	}
	/*wifi设置*/
	else if(pdata[0] == 0x2d01)
	{
		myPage->ChildPage = DspWifiSetPage;
		myPage->ChildPage(NULL);
	}
	/*服务器设置*/
	else if(pdata[0] == 0x2d02)
	{
		myPage->ChildPage = DspServerSetPage;
		myPage->ChildPage(NULL);
	}
	/*返回*/
	else if(pdata[0] == 0x2d03)
	{
		myPage->ParentPage(NULL);
	}

	MyFree(pdata);
}

static void PageUpData(void)
{

}





