/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"LunchPage.h"

#include	"LCD_Driver.h"
#include	"Define.h"
#include	"MyMem.h"

#include	"RecordPage.h"
#include	"SystemSetPage.h"
#include	"SelectUserPage.h"
#include	"PaiDuiPage.h"
#include	"SleepPage.h"
#include	"PlaySong_Task.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static LunchPageBuffer * S_LunchPageBuffer = NULL;
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
*FunctionName: createLunchActivity
*Description: 创建主界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyState_TypeDef createLunchActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, lunchActivityName, activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_LunchPageBuffer)
	{
		//读取系统设置
		getSystemSetData(&(S_LunchPageBuffer->systemSetData));
		
		timer_set(&(S_LunchPageBuffer->timer), S_LunchPageBuffer->systemSetData.ledSleepTime);
	
		DspPageText();
	}
	
	SelectPage(82);

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
	if(S_LunchPageBuffer)
	{
		/*命令*/
		S_LunchPageBuffer->lcdinput[0] = pbuf[4];
		S_LunchPageBuffer->lcdinput[0] = (S_LunchPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		//重置休眠时间
		timer_restart(&(S_LunchPageBuffer->timer));
		
		//设置
		if(S_LunchPageBuffer->lcdinput[0] == 0x1103)
		{
			startActivity(createSystemSetActivity, NULL);
		}
		//查看数据
		else if(S_LunchPageBuffer->lcdinput[0] == 0x1102)
		{	
			startActivity(createRecordActivity, NULL);
		}
		//常规测试
		else if(S_LunchPageBuffer->lcdinput[0] == 0x1100)
		{	
			S_LunchPageBuffer->error = CreateANewTest(NormalTestType);
			//创建成功
			if(Error_OK == S_LunchPageBuffer->error)
			{
				startActivity(createSelectUserActivity, NULL);
			}
			//禁止常规测试
			else if(Error_StopNormalTest == S_LunchPageBuffer->error)
			{
				SendKeyCode(1);
				AddNumOfSongToList(8, 0);
			}
			//创建失败
			else if(Error_Mem == S_LunchPageBuffer->error)
			{
				SendKeyCode(2);
				AddNumOfSongToList(7, 0);
			}
		}
		//批量测试
		else if(S_LunchPageBuffer->lcdinput[0] == 0x1101)
		{
			//有卡排队，则进入排队界面
			if(true == IsPaiDuiTestting())
			{
				startActivity(createPaiDuiActivity, NULL);
			}
			//无卡排队则开始创建
			else
			{
				S_LunchPageBuffer->error = CreateANewTest(PaiDuiTestType);
				//创建成功
				if(Error_OK == S_LunchPageBuffer->error)
				{
					startActivity(createSelectUserActivity, NULL);
				}
				//创建失败
				else if(Error_Mem == S_LunchPageBuffer->error)
				{
					SendKeyCode(2);
					AddNumOfSongToList(7, 0);
				}
			}
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
	if( S_LunchPageBuffer && (TimeOut == timer_expired(&(S_LunchPageBuffer->timer))))
	{
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
	if(S_LunchPageBuffer)
	{
		timer_restart(&(S_LunchPageBuffer->timer));
	}
	
	SelectPage(82);
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
	if(NULL == S_LunchPageBuffer)
	{
		S_LunchPageBuffer = MyMalloc(sizeof(LunchPageBuffer));
		if(S_LunchPageBuffer)	
		{
			memset(S_LunchPageBuffer, 0, sizeof(LunchPageBuffer));
	
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
	MyFree(S_LunchPageBuffer);
	S_LunchPageBuffer = NULL;
}

/***************************************************************************************************
*FunctionName: DspPageText
*Description: 当前界面内容显示
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:03:42
***************************************************************************************************/
static void DspPageText(void)
{
	if(S_LunchPageBuffer)
	{
		memset(S_LunchPageBuffer->buf, 0, 100);
		sprintf(S_LunchPageBuffer->buf, "V%d.%d.%02d", GB_SoftVersion_1, GB_SoftVersion_2, GB_SoftVersion_3);
		DisText(0x1110, S_LunchPageBuffer->buf, 30);
	}
}

