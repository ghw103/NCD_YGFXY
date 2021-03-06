/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SampleIDPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"System_Data.h"
#include	"MyMem.h"
#include	"MyTools.h"
#include	"MyTest_Data.h"

#include	"PaiDuiPage.h"
#include	"MyTest_Data.h"
#include	"SelectUserPage.h"
#include	"WaittingCardPage.h"
#include	"PlaySong_Task.h"
#include	"ReadBarCode_Fun.h"
#include	"Motor_Data.h"
#include	"Motor_Fun.h"

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
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);

static void RefreshSampleID(void);
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
MyState_TypeDef createSampleActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "SampleActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_SampleIDPage)
	{
		S_SampleIDPage->currenttestdata = GetCurrentTestItem();
		S_SampleIDPage->currenttestdata->statues = status_sample;
	
		while(ReadBarCodeFunction((char *)(S_SampleIDPage->tempbuf), 100) > 0)
			;
		
		RefreshSampleID();
		
		AddNumOfSongToList(10, 0);
	}
	
	SelectPage(86);
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
	if(S_SampleIDPage)
	{
		/*命令*/
		S_SampleIDPage->lcdinput[0] = pbuf[4];
		S_SampleIDPage->lcdinput[0] = (S_SampleIDPage->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_SampleIDPage->lcdinput[0] == 0x1300)
		{
			if(checkFatherActivityIs(paiduiActivityName))
			{
				MotorMoveTo(MaxLocation, 1);
				DeleteCurrentTest();
			}
			else
				S_SampleIDPage->currenttestdata->statues = status_user;
			
			backToFatherActivity();
		}
		
		/*确定*/
		else if(S_SampleIDPage->lcdinput[0] == 0x1301)
		{
			if(strlen(S_SampleIDPage->currenttestdata->testdata.sampleid) == 0)
			{
				SendKeyCode(1);
				AddNumOfSongToList(10, 0);
			}
			else
			{
				startActivity(createWaittingCardActivity, NULL);
			}
		}
		/*获取输入的id*/
		else if(S_SampleIDPage->lcdinput[0] == 0x1310)
		{
			memset(S_SampleIDPage->currenttestdata->testdata.sampleid, 0, MaxSampleIDLen);
			memcpy(S_SampleIDPage->currenttestdata->testdata.sampleid, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
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
	//从条码枪读取样品编号
	if(ReadBarCodeFunction((char *)(S_SampleIDPage->tempbuf), 100) > 0)
	{
		memcpy(S_SampleIDPage->currenttestdata->testdata.sampleid, S_SampleIDPage->tempbuf, MaxSampleIDLen);
		RefreshSampleID();
	}
	
	//如果排队中，有卡接近测试时间，则删除当前测试创建任务，返回
	if(GetMinWaitTime() < 40)
	{
		MotorMoveTo(MaxLocation, 1);
		DeleteCurrentTest();
		
		backToFatherActivity();
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
	if(S_SampleIDPage)
	{
		AddNumOfSongToList(10, 0);
		
		RefreshSampleID();
	}
	
	SelectPage(86);
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
	if(NULL == S_SampleIDPage)
	{
		S_SampleIDPage = MyMalloc(sizeof(SampleIDPage));
		
		if(S_SampleIDPage)
		{
			memset(S_SampleIDPage, 0, sizeof(SampleIDPage));
	
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
	MyFree(S_SampleIDPage);
	S_SampleIDPage = NULL;
}



static void RefreshSampleID(void)
{
	DisText(0x1310, S_SampleIDPage->currenttestdata->testdata.sampleid, MaxSampleIDLen);
}
