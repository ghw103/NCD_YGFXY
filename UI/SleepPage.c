/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SleepPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"Time_Data.h"

#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static SleepPageBuffer *S_SleepPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);
static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void DspPageText(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspSleepPage(void *  parm)
{
	SetGBParentPage(GetGBCurrentPage());
	SetGBCurrentPage(DspSleepPage);
	SetGBChildPage(NULL);
	SetGBPageUpDate(PageUpDate);
	SetGBGBPageInput(Input);
	SetGBPageInit(PageInit);
	SetGBPageBufferMalloc(PageBufferMalloc);
	SetGBPageBufferFree(PageBufferFree);
	
	GBPageInit(parm);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_SleepPageBuffer)
	{
		/*命令*/
		S_SleepPageBuffer->lcdinput[0] = pbuf[4];
		S_SleepPageBuffer->lcdinput[0] = (S_SleepPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*设置*/
		if(S_SleepPageBuffer->lcdinput[0] == 0x2f00)
		{
			SetLEDLight(100);
			GBPageBufferFree();
			GotoGBParentPage(NULL);
		}
	}
}

static void PageUpDate(void)
{
	if(S_SleepPageBuffer)
	{
		if((S_SleepPageBuffer->count % 10) == 0)
			DspPageText();
		
		S_SleepPageBuffer->count++;
	}
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SetLEDLight(10);
	
	SelectPage(115);
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(NULL == S_SleepPageBuffer)
	{
		S_SleepPageBuffer = MyMalloc(sizeof(SleepPageBuffer));
		if(NULL == S_SleepPageBuffer)
			return My_Fail;
	}
	memset(S_SleepPageBuffer, 0, sizeof(SleepPageBuffer));
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_SleepPageBuffer);
	S_SleepPageBuffer = NULL;
	return My_Pass;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static void DspPageText(void)
{
	if(S_SleepPageBuffer)
	{
		GetGBTimeData(&(S_SleepPageBuffer->time));
		
		memset(S_SleepPageBuffer->buf, 0, 50);
		sprintf(S_SleepPageBuffer->buf, "20%02d-%02d-%02d %02d:%02d:%02d", S_SleepPageBuffer->time.year, S_SleepPageBuffer->time.month, S_SleepPageBuffer->time.day,
			S_SleepPageBuffer->time.hour, S_SleepPageBuffer->time.min, S_SleepPageBuffer->time.sec);
	
		DisText(0x2f20, S_SleepPageBuffer->buf, strlen(S_SleepPageBuffer->buf));
	}
}
