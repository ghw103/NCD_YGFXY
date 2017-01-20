/***************************************************************************************************
*FileName:ReTestPage
*Description:老化测试界面
*Author: xsx_kair
*Data:2016年8月15日17:36:56
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"ReTestPage.h"
#include	"LCD_Driver.h"
#include	"Define.h"
#include	"MyMem.h"

#include	"SystemSetPage.h"
#include	"PlaySong_Task.h"
#include	"CardStatues_Data.h"
#include	"CodeScan_Task.h"
#include	"Test_Task.h"
#include	"SDFunction.h"
#include	"System_Data.h"
#include	"OutModel_Fun.h"
#include	"PlaySong_Task.h"
#include	"Ads8325_Driver.h"
#include	"TLV5617_Driver.h"
#include	"LEDCheck_Driver.h"
#include 	"MLX90614_Driver.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static ReTestPageBuffer * S_ReTestPageBuffer = NULL;
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);

static void SetReTestCount(char *str, unsigned char len);
static void StartReTest(void);
static void StopReTest(void);
static void EndOneReTest(char *result, unsigned char len);
static void CheckQRCode(void);
static void CheckTestCard(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************
*FunctionName: createSelectUserActivity
*Description: 创建选择操作人界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyState_TypeDef createReTestActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "ReTestActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_ReTestPageBuffer)
	{
		
		timer_set(&(S_ReTestPageBuffer->timer), 10);
		
		memset(S_ReTestPageBuffer->buf, 0, 100);
		sprintf(S_ReTestPageBuffer->buf, "Stopped");
		DisText(0x300c, S_ReTestPageBuffer->buf, strlen(S_ReTestPageBuffer->buf));
	}
	
	SelectPage(117);
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
	if(S_ReTestPageBuffer)
	{
		/*命令*/
		S_ReTestPageBuffer->lcdinput[0] = pbuf[4];
		S_ReTestPageBuffer->lcdinput[0] = (S_ReTestPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*退出*/
		if(S_ReTestPageBuffer->lcdinput[0] == 0x3012)
		{
			if(S_ReTestPageBuffer->retestdata.reteststatus == 0)
			{
				backToFatherActivity();
			}
			else
				SendKeyCode(1);
		}
		/*开始老化*/
		else if(S_ReTestPageBuffer->lcdinput[0] == 0x3010)
		{
			if(S_ReTestPageBuffer->retestdata.reteststatus == 0)
				StartReTest();
			else
				SendKeyCode(1);
		}
		/*停止老化*/
		else if(S_ReTestPageBuffer->lcdinput[0] == 0x3011)
		{
			if(S_ReTestPageBuffer->retestdata.reteststatus > 0)
				StopReTest();
		}
		/*获取老化次数*/
		else if(S_ReTestPageBuffer->lcdinput[0] == 0x3000)
		{
			SetReTestCount((char *)(&pbuf[7]), GetBufLen(&pbuf[7] , 2*pbuf[6]));
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
	if(S_ReTestPageBuffer)
	{
		if(S_ReTestPageBuffer->retestdata.reteststatus == 1)
		{
			if(GetCardState() == CardIN)
			{
				//检测到插卡，则保持温度和时间
				S_ReTestPageBuffer->retestdata.testdata.TestTemp.O_Temperature = GetCardTemperature();
					
				GetGB_Time(&(S_ReTestPageBuffer->retestdata.testdata.TestTime));
				
				//老化AD,DA
				SetGB_LedValue(100);
				vTaskDelay(100 / portTICK_RATE_MS);
				S_ReTestPageBuffer->retestdata.advalue1 = ADS8325();
				
				SetGB_LedValue(200);
				vTaskDelay(100 / portTICK_RATE_MS);
				S_ReTestPageBuffer->retestdata.advalue2 = ADS8325();
				
				SetGB_LedValue(300);
				vTaskDelay(100 / portTICK_RATE_MS);
				S_ReTestPageBuffer->retestdata.advalue3 = ADS8325();
				
				S_ReTestPageBuffer->retestdata.ledstatus = ReadLEDStatus();
				
				S_ReTestPageBuffer->retestdata.reteststatus = 2;
				StartScanQRCode(&(S_ReTestPageBuffer->retestdata.testdata.temperweima));
			}
			else if(TimeOut == timer_expired(&(S_ReTestPageBuffer->timer)))
				EndOneReTest("等待检测卡超时", 14);
		}
		else if(S_ReTestPageBuffer->retestdata.reteststatus == 2)
			CheckQRCode();
		else if(S_ReTestPageBuffer->retestdata.reteststatus == 3)
			CheckTestCard();

		
		S_ReTestPageBuffer->playstatus = getPlayStatus();
		if(S_ReTestPageBuffer->playstatus == 1)
		{
			//播放次数+1
			S_ReTestPageBuffer->retestdata.playcount++;
					
			//初始化当前播放时长
			timer_set(&(S_ReTestPageBuffer->retestdata.oneplaytimer), 999999);
					
			GetGB_Time(&(S_ReTestPageBuffer->retestdata.startplayTime));
		}
		else if(S_ReTestPageBuffer->playstatus == 0)
		{
			GetGB_Time(&(S_ReTestPageBuffer->retestdata.endplayTime));
					
			//保存当前播放信息
			if(My_Pass == SaveReTestData(&(S_ReTestPageBuffer->retestdata), 1))
			{
				if(S_ReTestPageBuffer->retestdata.reteststatus > 0)
					AddNumOfSongToList(55, 3);
			}
			else
			{
				//状态
				memset(S_ReTestPageBuffer->buf, 0, 100);
				sprintf(S_ReTestPageBuffer->buf, "Failed");
				DisText(0x300c, S_ReTestPageBuffer->buf, strlen(S_ReTestPageBuffer->buf));
					
				StopReTest();
			}
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
	if(NULL == S_ReTestPageBuffer)
	{
		S_ReTestPageBuffer = MyMalloc(sizeof(ReTestPageBuffer));
		
		if(S_ReTestPageBuffer)
		{
			memset(S_ReTestPageBuffer, 0, sizeof(ReTestPageBuffer));
	
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
	MyFree(S_ReTestPageBuffer);
	S_ReTestPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void SetReTestCount(char *str, unsigned char len)
{
	if(S_ReTestPageBuffer)
	{
		if(S_ReTestPageBuffer->retestdata.reteststatus == 0)
		{
			memset(S_ReTestPageBuffer->buf, 0, 100);
			memcpy(S_ReTestPageBuffer->buf, str, len);
			S_ReTestPageBuffer->retestdata.retestcount = strtol(S_ReTestPageBuffer->buf, NULL, 10);
			if(S_ReTestPageBuffer->retestdata.retestcount > 60000)
				S_ReTestPageBuffer->retestdata.retestcount = 60000;
		}
		
		memset(S_ReTestPageBuffer->buf, 0, 100);
		sprintf(S_ReTestPageBuffer->buf, "%d", S_ReTestPageBuffer->retestdata.retestcount);
		DisText(0x3000, S_ReTestPageBuffer->buf, strlen(S_ReTestPageBuffer->buf));
	}
}
static void StartReTest(void)
{
	if(S_ReTestPageBuffer)
	{
		if(S_ReTestPageBuffer->retestdata.retestcount == 0)
			return;
		
		//清除SD卡的老化数据

		
		//更新老化工作状态
		memset(S_ReTestPageBuffer->buf, 0, 100);
		sprintf(S_ReTestPageBuffer->buf, "Testing");
		DisText(0x300c, S_ReTestPageBuffer->buf, strlen(S_ReTestPageBuffer->buf));
		
		//更新已老化次数
		S_ReTestPageBuffer->retestdata.retestedcount = 0;
		DspNum(0x3006 , S_ReTestPageBuffer->retestdata.retestedcount, 4);
		
		//更新剩余老化次数
		S_ReTestPageBuffer->retestdata.retestsurpluscount = S_ReTestPageBuffer->retestdata.retestcount - S_ReTestPageBuffer->retestdata.retestedcount;
		DspNum(0x3008 , S_ReTestPageBuffer->retestdata.retestsurpluscount, 4);
		
		//初始化测试总时长计时器
		timer_set(&(S_ReTestPageBuffer->retestdata.retesttimer), 999999);
		//更新老化时间
		DspNum(0x300a , timer_Count(&(S_ReTestPageBuffer->retestdata.retesttimer)), 4);
		
		//初始化当前测试的时长计时器
		timer_set(&(S_ReTestPageBuffer->retestdata.oneretesttimer), 999999);
		
		S_ReTestPageBuffer->retestdata.reteststatus = 1;
		
		for(S_ReTestPageBuffer->buf[0]=0; S_ReTestPageBuffer->buf[0]<PaiDuiWeiNum; S_ReTestPageBuffer->buf[0]++)
			UpOneModelData(S_ReTestPageBuffer->buf[0], R_ON_G_OFF, 5);
			
		//开始测试音频
		AddNumOfSongToList(55, 3);
		//初始化播放总时长
		timer_set(&(S_ReTestPageBuffer->retestdata.playtimer), 999999);
	}
}

static void StopReTest(void)
{
	if(S_ReTestPageBuffer)
	{
		S_ReTestPageBuffer->retestdata.reteststatus = 0;

		memset(S_ReTestPageBuffer->buf, 0, 100);
		sprintf(S_ReTestPageBuffer->buf, "%d", S_ReTestPageBuffer->retestdata.retestcount);
		DisText(0x3000, S_ReTestPageBuffer->buf, strlen(S_ReTestPageBuffer->buf));
		
		OutModel_Init();
	}
}


static void EndOneReTest(char *result, unsigned char len)
{
	if(S_ReTestPageBuffer)
	{
		memset(S_ReTestPageBuffer->retestdata.result, 0, 30);
		memcpy(S_ReTestPageBuffer->retestdata.result, result, len);
		
		//保存当前测试结果
		if(My_Pass == SaveReTestData(&(S_ReTestPageBuffer->retestdata), 0))
		{
			//清除上次测试数据
			memset(&(S_ReTestPageBuffer->retestdata.testdata), 0, sizeof(TestData));
			
			if(S_ReTestPageBuffer->retestdata.retestedcount < S_ReTestPageBuffer->retestdata.retestcount)
				S_ReTestPageBuffer->retestdata.retestedcount++;
			
			S_ReTestPageBuffer->retestdata.retestsurpluscount = S_ReTestPageBuffer->retestdata.retestcount - S_ReTestPageBuffer->retestdata.retestedcount;
			S_ReTestPageBuffer->retestdata.reteststatus = 1;
			
			//已测
			DspNum(0x3006 , S_ReTestPageBuffer->retestdata.retestedcount, 4);
			//剩余
			DspNum(0x3008 , S_ReTestPageBuffer->retestdata.retestsurpluscount, 4);
			//已测时间
			DspNum(0x300a , timer_Count(&(S_ReTestPageBuffer->retestdata.retesttimer)), 4);
			//状态
			memset(S_ReTestPageBuffer->buf, 0, 100);
			if(S_ReTestPageBuffer->retestdata.retestsurpluscount == 0)
				sprintf(S_ReTestPageBuffer->buf, "Stopped");
			else
				sprintf(S_ReTestPageBuffer->buf, "Testing");
			DisText(0x300c, S_ReTestPageBuffer->buf, strlen(S_ReTestPageBuffer->buf));
			
			if(S_ReTestPageBuffer->retestdata.retestedcount >= S_ReTestPageBuffer->retestdata.retestcount)
				StopReTest();
			
			//初始化当前测试的时长计时器
			timer_set(&(S_ReTestPageBuffer->retestdata.oneretesttimer), 999999);
			
			//初始化插卡计时
			timer_set(&(S_ReTestPageBuffer->timer), 10);

		}
		else
		{
			//状态
			memset(S_ReTestPageBuffer->buf, 0, 100);
			sprintf(S_ReTestPageBuffer->buf, "Failed");
			DisText(0x300c, S_ReTestPageBuffer->buf, strlen(S_ReTestPageBuffer->buf));
			
			StopReTest();
		}
	}
}

static void CheckQRCode(void)
{
	if((S_ReTestPageBuffer) && (My_Pass == TakeScanQRCodeResult(&(S_ReTestPageBuffer->scancode))))
	{
		if(S_ReTestPageBuffer->scancode == CardCodeScanFail)
		{
			EndOneReTest("二维码读取失败", 14);
		}
		else if(S_ReTestPageBuffer->scancode == CardCodeCardOut)
		{
			EndOneReTest("二维码检测卡拔出", 16);
		}
		else if(S_ReTestPageBuffer->scancode == CardCodeScanTimeOut)
		{
			EndOneReTest("二维码读取超时", 14);
		}
		else if(S_ReTestPageBuffer->scancode == CardCodeCRCError)
		{
			EndOneReTest("二维码校验错误", 14);
		}
		else
		{
			//读取校准参数
			memcpy(S_ReTestPageBuffer->retestdata.testdata.tempadjust.ItemName, S_ReTestPageBuffer->retestdata.testdata.temperweima.ItemName, ItemNameLen);
			if(My_Fail == ReadAdjustData(&(S_ReTestPageBuffer->retestdata.testdata.tempadjust)))
				memset(&(S_ReTestPageBuffer->retestdata.testdata.tempadjust), 0, sizeof(AdjustData));

			S_ReTestPageBuffer->retestdata.reteststatus = 3;
			StartTest(&(S_ReTestPageBuffer->retestdata.testdata));
		}
	}
}

static void CheckTestCard(void)
{
	if((S_ReTestPageBuffer) && (My_Pass == TakeTestResult(&(S_ReTestPageBuffer->cardpretestresult))))
	{
		if(S_ReTestPageBuffer->cardpretestresult == ResultIsOK)
		{
			EndOneReTest("测试正常", 8);
		}
		else if(S_ReTestPageBuffer->cardpretestresult == NoSample)
		{
			EndOneReTest("未加样", 6);
		}
		else
		{
			EndOneReTest("检测卡拔出", 10);
		}
	}
}
/****************************************end of file************************************************/
