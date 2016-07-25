/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SampleIDPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"

#include	"MyTest_Data.h"
#include	"SelectUserPage.h"
#include	"GetSampleIDFun.h"
#include	"WaittingCardPage.h"
#include	"PlaySong_Task.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static SampleIDPage *S_SampleIDPage = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpData(void);

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
	SysPage * myPage = GetSysPage();

	myPage->CurrentPage = DspSampleIDPage;
	myPage->LCDInput = Input;
	myPage->PageUpData = PageUpData;
	myPage->ParentPage = DspSelectUserPage;
	myPage->ChildPage = DspWaittingCardPage;
	myPage->PageInit = PageInit;
	myPage->PageBufferMalloc = PageBufferMalloc;
	myPage->PageBufferFree = PageBufferFree;
	
	SelectPage(56);

	myPage->PageInit(parm);

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
	if(pdata[0] == 0x1d00)
	{
		DspSelectUserPage(NULL);
	}
	
	/*确定*/
	else if(pdata[0] == 0x1d01)
	{
		if(S_SampleIDPage)
		{
			if(strlen(S_SampleIDPage->tempid) == 0)
			{
				SendKeyCode(1);
				AddNumOfSongToList(26, 0);
			}
			else
			{
				memcpy(S_SampleIDPage->currenttestdata->testdata.sampleid, S_SampleIDPage->tempid, MaxSampleIDLen);
				PageBufferFree();
				DspWaittingCardPage(NULL);
			}
		}
	}
	
	else if(pdata[0] == 0x1d10)
	{
		if(S_SampleIDPage)
		{
			memset(S_SampleIDPage->tempid, 0 , MaxSampleIDLen);
		
			if(MaxSampleIDLen >= GetBufLen(&pbuf[7] , 2*pbuf[6]))
				memcpy(S_SampleIDPage->tempid, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			else
				memcpy(S_SampleIDPage->tempid, &pbuf[7], MaxSampleIDLen);
		}
	}

	MyFree(pdata);
}

static void PageUpData(void)
{
	if(My_Pass == TakeSampleIDData(&(S_SampleIDPage->tempbuf)))
	{
		memcpy(S_SampleIDPage->tempid, S_SampleIDPage->tempbuf, MaxSampleIDLen);
		MyFree(S_SampleIDPage->tempbuf);
		
		RefreshSampleID();
	}
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	S_SampleIDPage->currenttestdata = GetCurrentTestItem();
	/*清空文本*/
	ClearText(0x1d10, 20);
	
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
		DisText(0x1d10, S_SampleIDPage->tempid, MaxSampleIDLen);
	}
}
