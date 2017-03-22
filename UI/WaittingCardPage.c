/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"WaittingCardPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"

#include	"LunchPage.h"
#include	"SampleIDPage.h"
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
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createWaittingCardActivity
*Description: 创建等待插卡界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyState_TypeDef createWaittingCardActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "WaittingCardActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: activityStart
*Description: 显示主界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:32
***************************************************************************************************/
static void activityStart(void)
{
	if(S_WaitPageData)
	{
		MotorMoveTo(WaittingCardLocation, 1);
		
		S_WaitPageData->currenttestdata = GetCurrentTestItem();
		
		/*间隔一段时间提示插卡*/
		timer_set(&(S_WaitPageData->timer2), 20);
		
		//如果无卡，提示插卡
		if(GetCardState() == NoCard)
			AddNumOfSongToList(11, 0);
	}
	
	SelectPage(88);
}

/***************************************************************************************************
*FunctionName: activityInput
*Description: 界面输入
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:59
***************************************************************************************************/
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	if(S_WaitPageData)
	{
		/*命令*/
		S_WaitPageData->lcdinput[0] = pbuf[4];
		S_WaitPageData->lcdinput[0] = (S_WaitPageData->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_WaitPageData->lcdinput[0] == 0x1303)
		{
			if(S_WaitPageData->currenttestdata->statues == statues7)
				S_WaitPageData->currenttestdata->statues = statues4;
			
			backToFatherActivity();
		}
	}
}

/***************************************************************************************************
*FunctionName: activityFresh
*Description: 界面刷新
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:16
***************************************************************************************************/
static void activityFresh(void)
{
	/*是否插卡*/
	if(GetCardState() == CardIN)
	{
		startActivity(createPreReadCardActivity, NULL);	
	}
	/*时间到，未插卡，返回*/
	else 
	{
		/*提示插卡*/
		if(TimeOut == timer_expired(&(S_WaitPageData->timer2)))
		{
			AddNumOfSongToList(11, 0);
			timer_restart(&(S_WaitPageData->timer2));
		}
	}
}

/***************************************************************************************************
*FunctionName: activityHide
*Description: 隐藏界面时要做的事
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:40
***************************************************************************************************/
static void activityHide(void)
{

}

/***************************************************************************************************
*FunctionName: activityResume
*Description: 界面恢复显示时要做的事
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:58
***************************************************************************************************/
static void activityResume(void)
{
	SelectPage(88);
}

/***************************************************************************************************
*FunctionName: activityDestroy
*Description: 界面销毁
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:02:15
***************************************************************************************************/
static void activityDestroy(void)
{
	activityBufferFree();
}

/***************************************************************************************************
*FunctionName: activityBufferMalloc
*Description: 界面数据内存申请
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static MyState_TypeDef activityBufferMalloc(void)
{
	if(NULL == S_WaitPageData)
	{
		S_WaitPageData = MyMalloc(sizeof(WaitPageData));
		
		if(S_WaitPageData)
		{
			memset(S_WaitPageData, 0, sizeof(WaitPageData));
	
			return My_Pass;
		}
		else
			return My_Fail;
	}
	else
		return My_Pass;
}

/***************************************************************************************************
*FunctionName: activityBufferFree
*Description: 界面内存释放
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:03:10
***************************************************************************************************/
static void activityBufferFree(void)
{
	MyFree(S_WaitPageData);
	S_WaitPageData = NULL;
}

