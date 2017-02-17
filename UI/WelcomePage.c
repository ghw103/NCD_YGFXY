/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"WelcomePage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
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
		InitActivity(thizActivity, "WelcomeActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
		
		AddNumOfSongToList(0, 0);
	}

	SelectPage(1);
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
		/*命令*/
		S_WelcomePageBuffer->lcdinput[0] = pbuf[4];
		S_WelcomePageBuffer->lcdinput[0] = (S_WelcomePageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		S_WelcomePageBuffer->lcdinput[1] = pbuf[6];
		S_WelcomePageBuffer->lcdinput[1] = (S_WelcomePageBuffer->lcdinput[1]<<8) + pbuf[7];
		
		if(0x81 == pbuf[3])
		{
			//页面id
			if(0x03 == pbuf[4])
			{
				S_WelcomePageBuffer->currentPageId = S_WelcomePageBuffer->lcdinput[1];	
			}
		}
		else if(0x83 == pbuf[3])
		{
			if((S_WelcomePageBuffer->lcdinput[0] >= 0x1010) && (S_WelcomePageBuffer->lcdinput[0] <= 0x1014))
				while(1);
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
		if(My_Pass == readSelfTestStatus(&(S_WelcomePageBuffer->selfTestStatus)))
		{
			if(SystemData_OK == S_WelcomePageBuffer->selfTestStatus)
			{
				copyGBSystemSetData(&(S_WelcomePageBuffer->systemSetData));
				SetLEDLight(S_WelcomePageBuffer->systemSetData.ledLightIntensity);
			}
		}
		
		if(80 == S_WelcomePageBuffer->currentPageId)
		{
			//自检完成
			if(SelfTest_OK == S_WelcomePageBuffer->selfTestStatus)
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
				
				return;
			}
			//加载数据错误，说明sd异常
			else if(SystemData_ERROR == S_WelcomePageBuffer->selfTestStatus)
			{
				SelectPage(81);
				
				vTaskDelay(1000 / portTICK_RATE_MS);
				
				SendKeyCode(5);
				
				AddNumOfSongToList(5, 0);
			}
			//led异常，告警发光模块错误
			else if(Light_Error == S_WelcomePageBuffer->selfTestStatus)
			{
				SelectPage(81);
				vTaskDelay(1000 / portTICK_RATE_MS);
				SendKeyCode(4);
				AddNumOfSongToList(4, 0);
			}
			//采集异常，告警采集模块错误
			else if(AD_ERROR == S_WelcomePageBuffer->selfTestStatus)
			{
				SelectPage(81);
				vTaskDelay(1000 / portTICK_RATE_MS);
				SendKeyCode(3);
				AddNumOfSongToList(3, 0);
			}
			//传动异常，告警传动模块错误
			else if(Motol_ERROR == S_WelcomePageBuffer->selfTestStatus)
			{
				SelectPage(81);
				vTaskDelay(1000 / portTICK_RATE_MS);
				SendKeyCode(1);
				AddNumOfSongToList(1, 0);
			}
		}
		
		if((81 != S_WelcomePageBuffer->currentPageId) && (TimeOut == timer_expired(&(S_WelcomePageBuffer->timer))))
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


