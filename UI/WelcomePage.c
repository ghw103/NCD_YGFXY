/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"WelcomePage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"SelfCheck_Data.h"
#include	"PlaySong_Task.h"

#include	"LunchPage.h"

#include	"Test_Task.h"
#include 	"netconf.h"
#include	"NormalUpLoad_Task.h"
#include	"CodeScan_Task.h"
#include	"Paidui_Task.h"

#include	<string.h>
#include	"stdio.h"
/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static WelcomePageBuffer * S_WelcomePageBuffer = NULL;
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
*FunctionName: createWelcomeActivity
*Description: 创建欢迎界面
*Input: thizActivity -- 当前界面
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月20日16:21:51
***************************************************************************************************/
MyState_TypeDef createWelcomeActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "WelcomeActivity", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: activityStart
*Description: 显示当前界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月20日16:22:23
***************************************************************************************************/
static void activityStart(void)
{
	if(S_WelcomePageBuffer)
	{
		timer_set(&(S_WelcomePageBuffer->timer), 1);
	}
	
	SetLEDLight(100);
	
	SelectPage(0);
	
	AddNumOfSongToList(52, 0);
}

/***************************************************************************************************
*FunctionName: activityInput
*Description: 当前界面输入
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月20日16:22:42
***************************************************************************************************/
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	if(S_WelcomePageBuffer)
	{
		if(0x81 == pbuf[3])
		{
			//页面id
			if(0x03 == pbuf[4])
			{
				S_WelcomePageBuffer->lcdinput[0] = pbuf[6];
				S_WelcomePageBuffer->lcdinput[0] = (S_WelcomePageBuffer->lcdinput[0]<<8) + pbuf[7];
				
				//动画播放到末尾81号页面,且自检完成
				if((81 == S_WelcomePageBuffer->lcdinput[0]) && (SelfCheck_None != GetGB_SelfCheckStatus()))
				{					
					/*开启测试任务*/
					StartvTestTask();
					
					/*开启网络任务*/
					StartEthernet();

					/*上传任务*/
					StartvNormalUpLoadTask();
					
					/*开启读二维码任务*/
					StartCodeScanTask();
					
					//开始排队任务
					StartPaiduiTask();
					
					destroyTopActivity();
					startActivity(createLunchActivity, NULL);
				}
			}
		}
		else if(0x83 == pbuf[3])
		{
			
		}
	}
}

/***************************************************************************************************
*FunctionName: activityFresh
*Description: 当前界面刷新
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月20日16:22:56
***************************************************************************************************/
static void activityFresh(void)
{
	if(S_WelcomePageBuffer)
	{
		if(TimeOut == timer_expired(&(S_WelcomePageBuffer->timer)))
		{
			ReadCurrentPageId();

			timer_reset(&(S_WelcomePageBuffer->timer));
		}
	}
}


static void activityHide(void)
{

}

static void activityResume(void)
{
	
}

/***************************************************************************************************
*FunctionName: activityFinish
*Description: 
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static void activityDestroy(void)
{
	activityBufferFree();
}


/***************************************************************************************************
*FunctionName：PageBufferMalloc
*Description：当前界面临时缓存申请
*Input：None
*Output：MyState_TypeDef -- 返回成功与否
*Author：xsx
*Data：2016年6月27日08:56:02
***************************************************************************************************/
static MyState_TypeDef activityBufferMalloc(void)
{
	if(NULL == S_WelcomePageBuffer)
	{
		S_WelcomePageBuffer = (WelcomePageBuffer *)MyMalloc(sizeof(WelcomePageBuffer));
			
		if(S_WelcomePageBuffer)
		{
			memset(S_WelcomePageBuffer, 0, sizeof(WelcomePageBuffer));
		
			return My_Pass;
			
		}
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName：PageBufferFree
*Description：当前界面临时缓存释放
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月27日08:56:21
***************************************************************************************************/
static void activityBufferFree(void)
{
	MyFree(S_WelcomePageBuffer);
	S_WelcomePageBuffer = NULL;
}


