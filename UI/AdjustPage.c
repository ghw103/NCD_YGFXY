/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"AdjustPage.h"
#include	"LCD_Driver.h"
#include	"Define.h"
#include	"MyMem.h"

#include	"SystemSetPage.h"
#include	"PlaySong_Task.h"
#include	"CardStatues_Data.h"
#include	"CodeScan_Task.h"
#include	"Test_Task.h"
#include	"SDFunction.h"
#include	"Motor_Fun.h"
#include	"SystemSet_Dao.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static AdjustPageBuffer * S_AdjustPageBuffer = NULL;
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

static void dspAdjStatus(char * str);
static void dspTestResult(void);
static void dspAdjResult(void);
static void CheckQRCode(void);
static void CheckPreTestCard(void);
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
MyState_TypeDef createAdjActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "AdjActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_AdjustPageBuffer)
	{
		//获取最新的系统参数
		copyGBSystemSetData(&(S_AdjustPageBuffer->systemSetData));
		
		//获取最新led亮度值
		S_AdjustPageBuffer->itemData.ledLight = getTestLedLightIntensity(&(S_AdjustPageBuffer->systemSetData));
		
		dspAdjStatus("Waitting\0");
	}

	SelectPage(118);
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
	if(S_AdjustPageBuffer)
	{
		/*命令*/
		S_AdjustPageBuffer->lcdinput[0] = pbuf[4];
		S_AdjustPageBuffer->lcdinput[0] = (S_AdjustPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*退出*/
		if(S_AdjustPageBuffer->lcdinput[0] == 0x2701)
		{
			if(S_AdjustPageBuffer->isAdjjing == true)
				SendKeyCode(3);
			else
				backToFatherActivity();
		}
		//开始测试
		else if(S_AdjustPageBuffer->lcdinput[0] == 0x270a)
		{
			if(GetCardState() == CardIN)
			{
				if(S_AdjustPageBuffer->isAdjjing == true)
					dspAdjStatus("Already Test\0");
				else
				{
					S_AdjustPageBuffer->isAdjjing = true;
					
					StartScanQRCode(&(S_AdjustPageBuffer->itemData.testdata.temperweima));
				
					dspAdjStatus("Scanning QR\0");
				}
			}
			else
			{
				dspAdjStatus("No Card\0");
			}
		}
		/*校准*/
		else if(S_AdjustPageBuffer->lcdinput[0] == 0x2702)
		{
			if((S_AdjustPageBuffer->itemData.testdata.testline.BasicResult != 0) && (S_AdjustPageBuffer->targetresult != 0))
			{
				S_AdjustPageBuffer->adjustData.parm = S_AdjustPageBuffer->targetresult / S_AdjustPageBuffer->itemData.testdata.testline.BasicResult;

				dspAdjResult();
				
				dspAdjStatus("Success\0");
			}
		}
		/*输入标准值*/
		else if(S_AdjustPageBuffer->lcdinput[0] == 0x2740)
		{
			memset(S_AdjustPageBuffer->buf, 0 , 100);
			memcpy(S_AdjustPageBuffer->buf, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_AdjustPageBuffer->targetresult = strtod(S_AdjustPageBuffer->buf, NULL);
		}
		//保存校准结果
		else if(S_AdjustPageBuffer->lcdinput[0] == 0x2700)
		{
			//获取最新的系统参数
			copyGBSystemSetData(&(S_AdjustPageBuffer->systemSetData));
			
			//修改系统参数副本中对于项目的校准值
			addAdjPram(&(S_AdjustPageBuffer->systemSetData), &(S_AdjustPageBuffer->adjustData));
			
			if(My_Pass == SaveSystemSetData(&(S_AdjustPageBuffer->systemSetData)))
				SendKeyCode(1);
			else
				SendKeyCode(2);
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
	if(S_AdjustPageBuffer)
	{
		/*是否插卡*/
		if(GetCardState() == NoCard)
		{
			if(S_AdjustPageBuffer->isAdjjing == false)
			{
				dspAdjStatus("Waitting\0");
			}
		}
		
		CheckQRCode();
		
		CheckPreTestCard();
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
	if(NULL == S_AdjustPageBuffer)
	{
		S_AdjustPageBuffer = MyMalloc(sizeof(AdjustPageBuffer));
		
		if(S_AdjustPageBuffer)
		{
			memset(S_AdjustPageBuffer, 0, sizeof(AdjustPageBuffer));
	
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
	MyFree(S_AdjustPageBuffer);
	S_AdjustPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static void dspAdjStatus(char * str)
{
	memset(S_AdjustPageBuffer->buf, 0, 20);
	sprintf(S_AdjustPageBuffer->buf, "%-15s", str);
	DisText(0x2710, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
}

static void dspTestResult(void)
{
	sprintf(S_AdjustPageBuffer->buf, "(%d,%d)", S_AdjustPageBuffer->itemData.testdata.testline.T_Point[1], S_AdjustPageBuffer->itemData.testdata.testline.T_Point[0]);
	DisText(0x2720, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
	
	sprintf(S_AdjustPageBuffer->buf, "(%d,%d)", S_AdjustPageBuffer->itemData.testdata.testline.C_Point[1], S_AdjustPageBuffer->itemData.testdata.testline.C_Point[0]);
	DisText(0x2728, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
	
	sprintf(S_AdjustPageBuffer->buf, "%.3f", S_AdjustPageBuffer->itemData.testdata.testline.BasicBili);
	DisText(0x2730, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
	
	sprintf(S_AdjustPageBuffer->buf, "%.*f", S_AdjustPageBuffer->itemData.testdata.temperweima.ItemPoint, S_AdjustPageBuffer->itemData.testdata.testline.BasicResult);
	DisText(0x2738, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
}

static void dspAdjResult(void)
{
	sprintf(S_AdjustPageBuffer->buf, "%.3f", S_AdjustPageBuffer->adjustData.parm);
	DisText(0x2748, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
	
	sprintf(S_AdjustPageBuffer->buf, "%.*f", S_AdjustPageBuffer->itemData.testdata.temperweima.ItemPoint, S_AdjustPageBuffer->adjResult);
	DisText(0x2750, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
}

static void CheckQRCode(void)
{
	if(My_Pass == TakeScanQRCodeResult(&(S_AdjustPageBuffer->scancode)))
	{
		//二维码读取失败
		if((S_AdjustPageBuffer->scancode == CardCodeScanFail) || (S_AdjustPageBuffer->scancode == CardCodeCardOut) ||
			(S_AdjustPageBuffer->scancode == CardCodeScanTimeOut) || (S_AdjustPageBuffer->scancode == CardCodeCRCError))
		{
			MotorMoveTo(MaxLocation, 1);
			S_AdjustPageBuffer->isAdjjing = false;
			dspAdjStatus("Scan Fail\0");
		}
		//过期
		else if(S_AdjustPageBuffer->scancode == CardCodeTimeOut)
		{
			MotorMoveTo(MaxLocation, 1);
			S_AdjustPageBuffer->isAdjjing = false;
			dspAdjStatus("Out Of Date\0");
		}
		//读取成功
		else if(S_AdjustPageBuffer->scancode == CardCodeScanOK)
		{
			memset(S_AdjustPageBuffer->buf, 0, 20);
			sprintf(S_AdjustPageBuffer->buf, "%s", S_AdjustPageBuffer->itemData.testdata.temperweima.ItemName);
			DisText(0x2718, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
			
			dspAdjStatus("Read Card\0");
			
			//保存项目名称的前AdjItemNameLen个字符
			memcpy(S_AdjustPageBuffer->adjustData.ItemName, S_AdjustPageBuffer->itemData.testdata.temperweima.ItemName, AdjItemNameLen);
			
			//测试结果
			StartTest(&(S_AdjustPageBuffer->itemData));
		}
	}
}

static void CheckPreTestCard(void)
{
	if(My_Pass == TakeTestResult(&(S_AdjustPageBuffer->cardTestResult)))
	{
		MotorMoveTo(MaxLocation, 1);
		S_AdjustPageBuffer->isAdjjing = false;
		dspTestResult();
	}
}


