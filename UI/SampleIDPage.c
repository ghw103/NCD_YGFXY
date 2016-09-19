/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SampleIDPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"System_Data.h"
#include	"MyMem.h"

#include	"MyTest_Data.h"
#include	"SelectUserPage.h"
#include	"WaittingCardPage.h"
#include	"PlaySong_Task.h"
#include	"ReadBarCode_Fun.h"

#include	"BackDoorData.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static SampleIDPage *S_SampleIDPage = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);

static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void RefreshSampleID(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspSampleIDPage(void *  parm)
{
	
	SetGBSysPage(DspSampleIDPage, DspSelectUserPage, DspWaittingCardPage, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);

	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_SampleIDPage)
	{
		/*命令*/
		S_SampleIDPage->lcdinput[0] = pbuf[4];
		S_SampleIDPage->lcdinput[0] = (S_SampleIDPage->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_SampleIDPage->lcdinput[0] == 0x1d00)
		{
			GBPageBufferFree();
			GotoGBParentPage(NULL);
		}
		
		/*确定*/
		else if(S_SampleIDPage->lcdinput[0] == 0x1d01)
		{
			if(strlen(S_SampleIDPage->currenttestdata->testdata.sampleid) == 0)
			{
				SendKeyCode(1);
				AddNumOfSongToList(26, 0);
			}
			else
			{
				GBPageBufferFree();
				GotoGBChildPage(NULL);
			}
		}
		/*获取输入的id*/
		else if(S_SampleIDPage->lcdinput[0] == 0x1d10)
		{
			memset(S_SampleIDPage->currenttestdata->testdata.sampleid, 0, MaxSampleIDLen);
			memcpy(S_SampleIDPage->currenttestdata->testdata.sampleid, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			
			SetS_TestIndex(strtol(S_SampleIDPage->currenttestdata->testdata.sampleid , NULL , 10));
		}
		//选择第一个测试项目
		else if((S_SampleIDPage->lcdinput[0] >= 0x1d03) && (S_SampleIDPage->lcdinput[0] <= 0x1d07))
		{
			SetS_CategoryIndex(S_SampleIDPage->lcdinput[0] - 0x1d03);
		}
	}
}

static void PageUpDate(void)
{
	if(NULL != S_SampleIDPage)
	{
		if(My_Pass == CheckBarCodeHasRead())
		{
			GetGB_BarCode(S_SampleIDPage->tempbuf);
			
			memset(S_SampleIDPage->currenttestdata->testdata.sampleid, 0, MaxSampleIDLen);
			memcpy(S_SampleIDPage->currenttestdata->testdata.sampleid, S_SampleIDPage->tempbuf, MaxSampleIDLen);
			RefreshSampleID();
		}	
	}
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	//清除之前的条码数据，只有在当前页面扫码才有效
	CheckBarCodeHasRead();
	
	SelectPage(56);
	
	S_SampleIDPage->currenttestdata = GetCurrentTestItem();
	
	RefreshSampleID();
	
	timer_set(&(S_SampleIDPage->timer), 60);
	
	/*播放语音*/
	AddNumOfSongToList(26, 0);

	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	S_SampleIDPage = (SampleIDPage *)MyMalloc(sizeof(SampleIDPage));
			
	if(S_SampleIDPage)
	{
		memset(S_SampleIDPage, 0, sizeof(SampleIDPage));
		
		return My_Pass;
	}
	else
		return My_Fail;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_SampleIDPage);
	S_SampleIDPage = NULL;
	
	return My_Pass;
}


static void RefreshSampleID(void)
{
	if(S_SampleIDPage)
	{
		DisText(0x1d10, S_SampleIDPage->currenttestdata->testdata.sampleid, MaxSampleIDLen);
	}
}
