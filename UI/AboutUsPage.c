/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"AboutUsPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"

#include	"SleepPage.h"
#include	"PlaySong_Task.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static AboutUsPageBuffer * S_AboutUsPageBuffer = NULL;
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

static void dspPageText(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createSelectUserActivity
*Description: 创建选择操作人界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyState_TypeDef createAboutUsActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "AboutUsActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_AboutUsPageBuffer)
	{
		//读取系统设置
		copyGBSystemSetData(&(S_AboutUsPageBuffer->systemSetData));
		
		timer_set(&(S_AboutUsPageBuffer->timer), S_AboutUsPageBuffer->systemSetData.ledSleepTime);
		
		dspPageText();
	}
		
	SelectPage(116);
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
	if(S_AboutUsPageBuffer)
	{
		/*命令*/
		S_AboutUsPageBuffer->lcdinput[0] = pbuf[4];
		S_AboutUsPageBuffer->lcdinput[0] = (S_AboutUsPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		//重置休眠时间
		timer_restart(&(S_AboutUsPageBuffer->timer));
		
		//返回
		if(S_AboutUsPageBuffer->lcdinput[0] == 0x2900)
		{
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
	if(S_AboutUsPageBuffer)
	{
		if(TimeOut == timer_expired(&(S_AboutUsPageBuffer->timer)))
			startActivity(createSleepActivity, NULL);
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
	if(S_AboutUsPageBuffer)
	{
		timer_restart(&(S_AboutUsPageBuffer->timer));
		
		dspPageText();
	}
	
	SelectPage(116);
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
	if(NULL == S_AboutUsPageBuffer)
	{
		S_AboutUsPageBuffer = MyMalloc(sizeof(AboutUsPageBuffer));
		
		if(S_AboutUsPageBuffer)
		{
			memset(S_AboutUsPageBuffer, 0, sizeof(AboutUsPageBuffer));
	
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
	MyFree(S_AboutUsPageBuffer);
	S_AboutUsPageBuffer = NULL;
}

static void dspPageText(void)
{
	ClearText(0x2910, 30);
	ClearText(0x2920, 30);
	ClearText(0x2930, 30);
	
	memset(S_AboutUsPageBuffer->buf, 0, 30);
	sprintf(S_AboutUsPageBuffer->buf, "V%d.%d.%02d", GB_SoftVersion_1, GB_SoftVersion_2, GB_SoftVersion_3);
	DisText(0x2910, S_AboutUsPageBuffer->buf, strlen(S_AboutUsPageBuffer->buf));
	
	memset(S_AboutUsPageBuffer->buf, 0, 30);
	sprintf(S_AboutUsPageBuffer->buf, "%s", GB_SoftVersion_Build);
	DisText(0x2920, S_AboutUsPageBuffer->buf, strlen(S_AboutUsPageBuffer->buf));
	
	memset(S_AboutUsPageBuffer->buf, 0, 30);
	sprintf(S_AboutUsPageBuffer->buf, "鄂1234567890");
	DisText(0x2930, S_AboutUsPageBuffer->buf, strlen(S_AboutUsPageBuffer->buf));
}

