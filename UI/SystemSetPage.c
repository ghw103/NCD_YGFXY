/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SystemSetPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"MyMem.h"
#include	"LunchPage.h"
#include	"ShowDeviceInfoPage.h"
#include	"AdjustPage.h"
#include	"ReTestPage.h"
#include	"UserMPage.h"
#include	"NetPreSetPage.h"
#include	"RecordPage.h"
#include	"OtherSetPage.h"
#include	"MyTools.h"
#include	"SleepPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static SysSetPageBuffer * S_SysSetPageBuffer = NULL;
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
*FunctionName: createSelectUserActivity
*Description: 创建选择操作人界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyState_TypeDef createSystemSetActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "SystemSetActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_SysSetPageBuffer)
	{
		//读取系统设置
		getSystemSetData(&(S_SysSetPageBuffer->systemSetData));
		
		timer_set(&(S_SysSetPageBuffer->timer), S_SysSetPageBuffer->systemSetData.ledSleepTime);
	}
		
	SelectPage(98);
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
	if(S_SysSetPageBuffer)
	{
		/*命令*/
		S_SysSetPageBuffer->lcdinput[0] = pbuf[4];
		S_SysSetPageBuffer->lcdinput[0] = (S_SysSetPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		//重置休眠时间
		timer_restart(&(S_SysSetPageBuffer->timer));
		
		//基本信息
		if(S_SysSetPageBuffer->lcdinput[0] == 0x1900)
		{
			startActivity(createDeviceInfoActivity, NULL);
		}
		//操作人管理
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1901)
		{
			startActivity(createUserManagerActivity, NULL);
		}
		//网络设置
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1902)
		{
			startActivity(createNetPreActivity, NULL);
		}
		//数据管理
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1903)
		{
			startActivity(createRecordActivity, NULL);
		}
		//校准功能
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1910)
		{
			if(GetBufLen(&pbuf[7] , 2*pbuf[6]) == 6)
			{
				if(pdPASS == CheckStrIsSame(&pbuf[7] , AdjustPassWord ,GetBufLen(&pbuf[7] , 2*pbuf[6])))
				{
					startActivity(createAdjActivity, NULL);
				}
				else if(pdPASS == CheckStrIsSame(&pbuf[7] , TestPassWord ,GetBufLen(&pbuf[7] , 2*pbuf[6])))
				{
					startActivity(createReTestActivity, NULL);
				}
				else
					SendKeyCode(1);
			}
			else
					SendKeyCode(1);
		}
		//其他设置
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1904)
		{
			startActivity(createOtherSetActivity, NULL);
		}
		//返回
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1906)
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
	if(S_SysSetPageBuffer)
	{
		if(TimeOut == timer_expired(&(S_SysSetPageBuffer->timer)))
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
	if(S_SysSetPageBuffer)
	{
		timer_restart(&(S_SysSetPageBuffer->timer));
	}
	
	SelectPage(98);
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
	if(NULL == S_SysSetPageBuffer)
	{
		S_SysSetPageBuffer = MyMalloc(sizeof(SysSetPageBuffer));
		
		if(S_SysSetPageBuffer)
		{
			memset(S_SysSetPageBuffer, 0, sizeof(SysSetPageBuffer));
	
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
	MyFree(S_SysSetPageBuffer);
	S_SysSetPageBuffer = NULL;
}


