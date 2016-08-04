/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"WaittingCardPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"

#include	"LunchPage.h"
#include	"SampleIDPage.h"
#include	"OperGuidePage.h"
#include	"PreReadCardPage.h"
#include	"CardStatues_Data.h"
#include	"Motor_Data.h"
#include	"Motor_Fun.h"
#include	"PlaySong_Task.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static WaitPageData * S_WaitPageData = NULL;
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

unsigned char DspWaittingCardPage(void *  parm)
{
	SetGBSysPage(DspWaittingCardPage, DspSampleIDPage, DspOperGuidePage, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
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
	
	/*返回*/
	if(pdata[0] == 0x1e00)
	{
		GBPageBufferFree();
		SetGBChildPage(DspSampleIDPage);
		GotoGBChildPage(NULL);
	}
	
	/*查看操作规程*/
	else if(pdata[0] == 0x1e01)
	{
		GBPageBufferFree();
		SetGBChildPage(DspOperGuidePage);
		GotoGBChildPage(NULL);
	}

	MyFree(pdata);
}

static void PageUpDate(void)
{
	/*是否插卡*/
	if(GetCardState() == CardIN)
	{
		GBPageBufferFree();
		SetGBChildPage(DspPreReadCardPage);
		GotoGBChildPage(NULL);
	}
	/*时间到，未插卡，返回*/
	else if(TimeOut == timer_expired(&(S_WaitPageData->timer)))
	{
		AddNumOfSongToList(8, 0);
		
		GBPageBufferFree();
		SetGBParentPage(DspLunchPage);
		GotoGBParentPage(NULL);
		return;
	}
	
	/*提示插卡*/
	if(TimeOut == timer_expired(&(S_WaitPageData->timer2)))
	{
		AddNumOfSongToList(43, 0);
		timer_restart(&(S_WaitPageData->timer2));
	}
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(58);
	
	MotorMoveTo(WaittingCardLocation, 1);
	
	S_WaitPageData->currenttestdata = GetCurrentTestItem();
	
	/*等待时间，超时则取消测试*/
	timer_set(&(S_WaitPageData->timer), 55);
	
	/*间隔一段时间提示插卡*/
	timer_set(&(S_WaitPageData->timer2), 10);
	
	AddNumOfSongToList(43, 0);
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	S_WaitPageData = (WaitPageData *)MyMalloc(sizeof(WaitPageData));
			
	if(S_WaitPageData)
	{
		memset(S_WaitPageData, 0, sizeof(WaitPageData));
		
		return My_Pass;
	}
	else
		return My_Fail;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_WaitPageData);
	S_WaitPageData = NULL;
	
	return My_Pass;
}
