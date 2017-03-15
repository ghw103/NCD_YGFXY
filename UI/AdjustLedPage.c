
/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"AdjustLedPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"System_Data.h"
#include	"CardStatues_Data.h"
#include	"Motor_Fun.h"
#include	"MyMem.h"
#include	"Test_Task.h"
#include	"SystemSet_Data.h"
#include	"SystemSet_Dao.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static AdjustLedPageBuffer *S_AdjustLedPageBuffer = NULL;
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

static void clearPageText(void);
static void dspTestStatus(char * str);
static void DspPageText(void);
static void analysisTestData(void);
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
MyState_TypeDef createAdjustLedActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "AdjustLedActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

static void activityStart(void)
{
	if(S_AdjustLedPageBuffer)
	{
		//获取最新的系统参数
		copyGBSystemSetData(&(S_AdjustLedPageBuffer->systemSetData));
		
		clearPageText();
		dspTestStatus("Waitting\0");
		
		//校准设置为1通道
		S_AdjustLedPageBuffer->itemData.testdata.temperweima.ChannelNum = 0;
	}

	SelectPage(140);
}
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	if(S_AdjustLedPageBuffer)
	{
		/*命令*/
		S_AdjustLedPageBuffer->lcdinput[0] = pbuf[4];
		S_AdjustLedPageBuffer->lcdinput[0] = (S_AdjustLedPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		//开始校准
		if(S_AdjustLedPageBuffer->lcdinput[0] == 0x2602)
		{
			if(S_AdjustLedPageBuffer->isTestting == false)
			{
				//初始状态从200开始查找
				S_AdjustLedPageBuffer->itemData.ledLight = 200;
				
				S_AdjustLedPageBuffer->isTestting = true;
				S_AdjustLedPageBuffer->testCnt = 1;
				StartTest(&(S_AdjustLedPageBuffer->itemData));
				
				dspTestStatus("Testting\0");
				
				DspNum(0x2605, S_AdjustLedPageBuffer->testCnt, 2);
				DspNum(0x2604, S_AdjustLedPageBuffer->itemData.ledLight, 2);
			}
		}
		//读取目标值
		else if(S_AdjustLedPageBuffer->lcdinput[0] == 0x2610)
		{
			memset(S_AdjustLedPageBuffer->buf, 0, 20);
			memcpy(S_AdjustLedPageBuffer->buf, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			
			S_AdjustLedPageBuffer->targetValue = strtol(S_AdjustLedPageBuffer->buf , NULL, 10);
		}
		//读取误差值
		else if(S_AdjustLedPageBuffer->lcdinput[0] == 0x2640)
		{
			memset(S_AdjustLedPageBuffer->buf, 0, 20);
			memcpy(S_AdjustLedPageBuffer->buf, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			
			S_AdjustLedPageBuffer->targetRange = strtol(S_AdjustLedPageBuffer->buf , NULL, 10);
		}
		//保存校准值
		else if(S_AdjustLedPageBuffer->lcdinput[0] == 0x2600)
		{
			if((S_AdjustLedPageBuffer->itemData.ledLight >= 100) && (S_AdjustLedPageBuffer->itemData.ledLight <= 300))
			{
				//获取最新的系统参数
				copyGBSystemSetData(&(S_AdjustLedPageBuffer->systemSetData));
				
				//修改系统参数副本中值
				setTestLedLightIntensity(&(S_AdjustLedPageBuffer->systemSetData), S_AdjustLedPageBuffer->itemData.ledLight);
				
				if(My_Pass == SaveSystemSetData(&(S_AdjustLedPageBuffer->systemSetData)))
				{
					SendKeyCode(1);
					
					//修改成功，更新内存中的值
					setSystemSetData(&(S_AdjustLedPageBuffer->systemSetData));
				}
				else
					SendKeyCode(2);
			}
		}
		//取消返回
		else if(S_AdjustLedPageBuffer->lcdinput[0] == 0x2601)
		{
			if(S_AdjustLedPageBuffer->isTestting == false)
				backToFatherActivity();
			else
				SendKeyCode(3);
		}
	}
}

static void activityFresh(void)
{
	if(S_AdjustLedPageBuffer)
	{
		if(GetCardState() == NoCard)
		{
			if(S_AdjustLedPageBuffer->isTestting == false)
			{
				dspTestStatus("Waitting\0");
			}
		}
		
		if(My_Pass == TakeTestResult(&(S_AdjustLedPageBuffer->cardpretestresult)))
		{
			analysisTestData();
		}
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
	activityBufferFree();
}

static MyState_TypeDef activityBufferMalloc(void)
{
	if(NULL == S_AdjustLedPageBuffer)
	{
		S_AdjustLedPageBuffer = MyMalloc(sizeof(AdjustLedPageBuffer));
		
		if(S_AdjustLedPageBuffer)
		{
			memset(S_AdjustLedPageBuffer, 0, sizeof(AdjustLedPageBuffer));
			
			return My_Pass;
		}
	}

	return My_Fail;
}

static void activityBufferFree(void)
{
	MyFree(S_AdjustLedPageBuffer);
	S_AdjustLedPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static void clearPageText(void)
{
	ClearText(0x2610, 15);
	ClearText(0x2630, 15);
	ClearText(0x2640, 15);
	
	DspNum(0x2603, 0, 2);
	DspNum(0x2604, 0, 2);
	DspNum(0x2605, 0, 2);
	
	//显示当前校准值
	DspNum(0x2609, S_AdjustLedPageBuffer->systemSetData.testLedLightIntensity, 2);
}

static void dspTestStatus(char * str)
{
	memset(S_AdjustLedPageBuffer->buf, 0, 20);
	
	sprintf(S_AdjustLedPageBuffer->buf, "%-15s", str);
	
	DisText(0x2620, S_AdjustLedPageBuffer->buf, 20);
}

static void DspPageText(void)
{
	sprintf(S_AdjustLedPageBuffer->buf, "%-10d", S_AdjustLedPageBuffer->maxPoint[0]);
	DisText(0x2630, S_AdjustLedPageBuffer->buf, strlen(S_AdjustLedPageBuffer->buf));
	
	DspNum(0x2603, S_AdjustLedPageBuffer->maxPoint[1], 2);
}

static void analysisTestData(void)
{
	//找最大值
	S_AdjustLedPageBuffer->maxPoint[0] = S_AdjustLedPageBuffer->itemData.testdata.testline.TestPoint[0];

	for(S_AdjustLedPageBuffer->i=0; S_AdjustLedPageBuffer->i<MaxPointLen; S_AdjustLedPageBuffer->i++)
	{
		if(S_AdjustLedPageBuffer->maxPoint[0] < S_AdjustLedPageBuffer->itemData.testdata.testline.TestPoint[S_AdjustLedPageBuffer->i])
		{
			S_AdjustLedPageBuffer->maxPoint[0] = S_AdjustLedPageBuffer->itemData.testdata.testline.TestPoint[S_AdjustLedPageBuffer->i];
			S_AdjustLedPageBuffer->maxPoint[1] = S_AdjustLedPageBuffer->i;
		}
	}
	
	DspPageText();
	
	if(abs(S_AdjustLedPageBuffer->targetValue - S_AdjustLedPageBuffer->maxPoint[0]) < S_AdjustLedPageBuffer->targetRange)
	{
		dspTestStatus("Success\0");
		S_AdjustLedPageBuffer->isTestting = false;
		MotorMoveTo(MaxLocation, 1);
		return;
	}
	else if(S_AdjustLedPageBuffer->targetValue > S_AdjustLedPageBuffer->maxPoint[0])
	{
		//继续调节
		if(S_AdjustLedPageBuffer->itemData.ledLight < 300)
		{
			S_AdjustLedPageBuffer->itemData.ledLight += 10;
			S_AdjustLedPageBuffer->testCnt++;
			
			if(S_AdjustLedPageBuffer->testCnt <= 11)
			{
				DspNum(0x2605, S_AdjustLedPageBuffer->testCnt, 2);
				DspNum(0x2604, S_AdjustLedPageBuffer->itemData.ledLight, 2);
					
				StartTest(&(S_AdjustLedPageBuffer->itemData));
				
				return;
			}
		}
	}
	else
	{
		//继续调节
		if(S_AdjustLedPageBuffer->itemData.ledLight > 100)
		{
			S_AdjustLedPageBuffer->itemData.ledLight -= 10;
			S_AdjustLedPageBuffer->testCnt++;
			
			if(S_AdjustLedPageBuffer->testCnt <= 10)
			{
				DspNum(0x2605, S_AdjustLedPageBuffer->testCnt, 2);
				DspNum(0x2604, S_AdjustLedPageBuffer->itemData.ledLight, 2);
					
				StartTest(&(S_AdjustLedPageBuffer->itemData));
				
				return;
			}
		}
	}
	
	memset(S_AdjustLedPageBuffer->buf, 0, 20);
	sprintf(S_AdjustLedPageBuffer->buf, "Fail - %d", S_AdjustLedPageBuffer->cardpretestresult);
	DisText(0x2620, S_AdjustLedPageBuffer->buf, strlen(S_AdjustLedPageBuffer->buf));
	S_AdjustLedPageBuffer->isTestting = false;
	
	MotorMoveTo(MaxLocation, 1);
}
