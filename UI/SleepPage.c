
/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SleepPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"System_Data.h"

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
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);

static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);

static void DspPageText(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createWelcomeActivity
*Description: 创建欢迎界面
*Input: thizActivity -- 当前界面
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月20日16:21:51
***************************************************************************************************/
MyState_TypeDef createSleepActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "SleepActivity", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

static void activityStart(void)
{
	SetLEDLight(10);
	
	SelectPage(142);
}
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	if(S_SleepPageBuffer)
	{
		/*命令*/
		S_SleepPageBuffer->lcdinput[0] = pbuf[4];
		S_SleepPageBuffer->lcdinput[0] = (S_SleepPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*设置*/
		if(S_SleepPageBuffer->lcdinput[0] == 0x1D70)
		{
			backToActivity(lunchActivityName);
		}
	}
}
static void activityFresh(void)
{
	if(S_SleepPageBuffer)
	{
		if((S_SleepPageBuffer->count % 10) == 0)
			DspPageText();
		
		S_SleepPageBuffer->count++;
	}
}
static void activityHide(void)
{

}
static void activityResume(void)
{

}
static void activityDestroy(void)
{
	SetLEDLight(100);
	
	activityBufferFree();
}

static MyState_TypeDef activityBufferMalloc(void)
{
	if(NULL == S_SleepPageBuffer)
	{
		S_SleepPageBuffer = MyMalloc(sizeof(SleepPageBuffer));
		
		if(S_SleepPageBuffer)
		{
			memset(S_SleepPageBuffer, 0, sizeof(SleepPageBuffer));
			
			return My_Pass;
		}
	}

	return My_Fail;
}

static void activityBufferFree(void)
{
	MyFree(S_SleepPageBuffer);
	S_SleepPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static void DspPageText(void)
{
/*	if(S_SleepPageBuffer)
	{
		GetGB_Time(&(S_SleepPageBuffer->time));
		
		memset(S_SleepPageBuffer->buf, 0, 50);
		sprintf(S_SleepPageBuffer->buf, "20%02d-%02d-%02d %02d:%02d:%02d ", S_SleepPageBuffer->time.year, S_SleepPageBuffer->time.month, S_SleepPageBuffer->time.day,
			S_SleepPageBuffer->time.hour, S_SleepPageBuffer->time.min, S_SleepPageBuffer->time.sec);
	
		DisText(0x2f20, S_SleepPageBuffer->buf, strlen(S_SleepPageBuffer->buf));
	}*/
}
