/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"PihaoPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"System_Data.h"
#include	"MyMem.h"

#include	"MyTest_Data.h"
#include	"SelectUserPage.h"
#include	"SampleIDPage.h"
#include	"BackDoorData.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static PihaoPage *S_PihaoPage = NULL;
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

unsigned char DspPihaoPage(void *  parm)
{
	
	SetGBSysPage(DspPihaoPage, DspSelectUserPage, DspSampleIDPage, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);

	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_PihaoPage)
	{
		/*命令*/
		S_PihaoPage->lcdinput[0] = pbuf[4];
		S_PihaoPage->lcdinput[0] = (S_PihaoPage->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_PihaoPage->lcdinput[0] == 0x3100)
		{
			GBPageBufferFree();
			GotoGBParentPage(NULL);
		}
		
		/*确定*/
		else if(S_PihaoPage->lcdinput[0] == 0x3101)
		{
			GBPageBufferFree();
			GotoGBChildPage(NULL);
		}
		/*获取输入的id*/
		else if(S_PihaoPage->lcdinput[0] == 0x3110)
		{
			memset(S_PihaoPage->currenttestdata->testdata.pihao, 0, 20);
			memcpy(S_PihaoPage->currenttestdata->testdata.pihao, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
		}
	}
}

static void PageUpDate(void)
{

}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	ClearText(0x3110, 20);
	
	SelectPage(119);
	
	S_PihaoPage->currenttestdata = GetCurrentTestItem();
	
	
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	S_PihaoPage = (PihaoPage *)MyMalloc(sizeof(PihaoPage));
			
	if(S_PihaoPage)
	{
		memset(S_PihaoPage, 0, sizeof(PihaoPage));
		
		return My_Pass;
	}
	else
		return My_Fail;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_PihaoPage);
	S_PihaoPage = NULL;
	
	return My_Pass;
}

