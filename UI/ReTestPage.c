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
#include	"Timer_Data.h"
#include	"System_Data.h"
#include	"OutModel_Fun.h"
#include	"PlaySong_Task.h"
#include	"Ads8325_Driver.h"
#include	"TLV5617_Driver.h"
#include	"LEDCheck_Driver.h"
#include 	"MLX90614_Driver.h"
#include	"Motor_Fun.h"

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
static void CheckQRCode(void);
static void CheckTestCard(void);

static void dspReTestStatus(char * str);
static void EndOneReTest(char *result);
static void dspReTestLogs(void);
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
		
		dspReTestStatus("Stand By\0");
	}
	
	SelectPage(143);
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
		if(S_ReTestPageBuffer->lcdinput[0] == 0x2802)
		{
			if(S_ReTestPageBuffer->reTestData.reteststatus == 0)
				backToFatherActivity();
			else
				SendKeyCode(1);
		}
		/*开始老化*/
		else if(S_ReTestPageBuffer->lcdinput[0] == 0x2800)
		{
			if((S_ReTestPageBuffer->reTestData.retestcount == 0) || (S_ReTestPageBuffer->reTestData.reteststatus > 0))
				SendKeyCode(1);
			else
				StartReTest();
		}
		/*停止老化*/
		else if(S_ReTestPageBuffer->lcdinput[0] == 0x2801)
		{
			S_ReTestPageBuffer->reTestData.retestsurpluscount = 0;
		}
		/*获取老化次数*/
		else if(S_ReTestPageBuffer->lcdinput[0] == 0x2820)
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
	if(S_ReTestPageBuffer->reTestData.reteststatus > 0)
	{
		if(S_ReTestPageBuffer->reTestData.retestStep == 1)
		{
			if(GetCardState() == CardIN)
			{
				//检测到插卡，则保持温度和时间
				S_ReTestPageBuffer->reTestData.itemData.testdata.TestTemp.O_Temperature = GetCardTemperature();
				S_ReTestPageBuffer->reTestData.itemData.testdata.TestTemp.E_Temperature = GetGB_EnTemperature();
				GetGB_Time(&(S_ReTestPageBuffer->reTestData.itemData.testdata.TestTime));
				
				//老化AD,DA
				SetGB_LedValue(100);
				vTaskDelay(100 / portTICK_RATE_MS);
				S_ReTestPageBuffer->reTestData.advalue1 = ADS8325();
				
				SetGB_LedValue(200);
				vTaskDelay(100 / portTICK_RATE_MS);
				S_ReTestPageBuffer->reTestData.advalue2 = ADS8325();
				
				SetGB_LedValue(300);
				vTaskDelay(100 / portTICK_RATE_MS);
				S_ReTestPageBuffer->reTestData.advalue3 = ADS8325();
				
				S_ReTestPageBuffer->reTestData.ledstatus = ReadLEDStatus();
				
				S_ReTestPageBuffer->reTestData.retestStep = 2;
				StartScanQRCode(&(S_ReTestPageBuffer->reTestData.itemData.testdata.temperweima));
			}
			else if(TimeOut == timer_expired(&(S_ReTestPageBuffer->timer)))
			{
				dspReTestStatus("No Card\0");
				EndOneReTest("等待检测卡超时\0");
				S_ReTestPageBuffer->reTestData.retestsurpluscount = 0;
				S_ReTestPageBuffer->reTestData.retestStep = 0;
			}
		}
		else if(S_ReTestPageBuffer->reTestData.retestStep == 2)
			CheckQRCode();
		else if(S_ReTestPageBuffer->reTestData.retestStep == 3)
			CheckTestCard();

		//管理音频测试
		if(0 == getPlayStatus())
		{
			if(timerIsStartted(&(S_ReTestPageBuffer->reTestData.oneplaytimer)))
			{
				GetGB_Time(&(S_ReTestPageBuffer->reTestData.endplayTime));
					
				//保存当前播放信息
				if(My_Pass == SavereTestData(&(S_ReTestPageBuffer->reTestData), 1))
				{
					timer_stop(&(S_ReTestPageBuffer->reTestData.oneplaytimer));	
					
					//如果剩余测试次数等于0，则停止老化测试
					if(S_ReTestPageBuffer->reTestData.retestsurpluscount == 0)
						StopReTest();
				}
				else
				{
					dspReTestStatus("保存播放信息失败\0");

					StopReTest();
				}
			}
			else
			{
				S_ReTestPageBuffer->reTestData.playcount++;
				GetGB_Time(&(S_ReTestPageBuffer->reTestData.startplayTime));
				AddNumOfSongToList(55, 3);
				timer_set(&(S_ReTestPageBuffer->reTestData.oneplaytimer), 999999);
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
	if(S_ReTestPageBuffer->reTestData.reteststatus == 0)
	{
		memset(S_ReTestPageBuffer->buf, 0, 100);
		memcpy(S_ReTestPageBuffer->buf, str, len);
		S_ReTestPageBuffer->reTestData.retestcount = strtol(S_ReTestPageBuffer->buf, NULL, 10);
		if(S_ReTestPageBuffer->reTestData.retestcount > 60000)
			S_ReTestPageBuffer->reTestData.retestcount = 60000;
	}
		
	memset(S_ReTestPageBuffer->buf, 0, 100);
	sprintf(S_ReTestPageBuffer->buf, "%d", S_ReTestPageBuffer->reTestData.retestcount);
	DisText(0x2820, S_ReTestPageBuffer->buf, strlen(S_ReTestPageBuffer->buf));
}
static void StartReTest(void)
{	
	//更新老化工作状态
	dspReTestStatus("Testting\0");
		
	//更新已老化次数
	S_ReTestPageBuffer->reTestData.retestedcount = 0;
	//更新剩余老化次数
	S_ReTestPageBuffer->reTestData.retestsurpluscount = S_ReTestPageBuffer->reTestData.retestcount - S_ReTestPageBuffer->reTestData.retestedcount;

	//初始化测试总时长计时器
	timer_set(&(S_ReTestPageBuffer->reTestData.retesttimer), 999999);	
	//初始化当前测试的时长计时器
	timer_set(&(S_ReTestPageBuffer->reTestData.oneretesttimer), 999999);
	
	S_ReTestPageBuffer->reTestData.retestStep = 1;
	
	dspReTestLogs();
	
	//启动排队模块
	for(S_ReTestPageBuffer->buf[0]=0; S_ReTestPageBuffer->buf[0]<PaiDuiWeiNum; S_ReTestPageBuffer->buf[0]++)
		UpOneModelData(S_ReTestPageBuffer->buf[0], R_ON_G_OFF, 5);
			
	//启动音频测试
	timer_set(&(S_ReTestPageBuffer->reTestData.playtimer), 999999);
	S_ReTestPageBuffer->reTestData.playcount = 0;
	
	//启动所有测试
	S_ReTestPageBuffer->reTestData.reteststatus = 1;
	
}

static void StopReTest(void)
{
	//停止排队模块
	OutModel_Init();
	
	//停止所有测试
	S_ReTestPageBuffer->reTestData.reteststatus = 0;
}


static void EndOneReTest(char *result)
{
	memset(S_ReTestPageBuffer->reTestData.result, 0, 30);
	memcpy(S_ReTestPageBuffer->reTestData.result, result, strlen(result));
		
	//保存当前测试结果
	if(My_Pass == SavereTestData(&(S_ReTestPageBuffer->reTestData), 0))
	{
		//清除上次测试数据
		memset(&(S_ReTestPageBuffer->reTestData.itemData), 0, sizeof(ItemData));
			
		if(S_ReTestPageBuffer->reTestData.retestedcount < S_ReTestPageBuffer->reTestData.retestcount)
			S_ReTestPageBuffer->reTestData.retestedcount++;
		
		if(S_ReTestPageBuffer->reTestData.retestsurpluscount > 0)		
			S_ReTestPageBuffer->reTestData.retestsurpluscount = S_ReTestPageBuffer->reTestData.retestcount - S_ReTestPageBuffer->reTestData.retestedcount;
		
			
		dspReTestLogs();
			
		//测试次数到
		if(S_ReTestPageBuffer->reTestData.retestsurpluscount == 0)
		{
			dspReTestStatus("Finish\0");
		}
		else
			S_ReTestPageBuffer->reTestData.retestStep = 1;
			
		//初始化当前测试的时长计时器
		timer_restart(&(S_ReTestPageBuffer->reTestData.oneretesttimer));
			
		//初始化插卡计时
		timer_set(&(S_ReTestPageBuffer->timer), 10);
	}
	else
	{
		//状态
		dspReTestStatus("Test Data Save Failed\0");
			
		S_ReTestPageBuffer->reTestData.retestsurpluscount = 0;
		S_ReTestPageBuffer->reTestData.retestStep = 4;
	}
}

static void CheckQRCode(void)
{
	if(My_Pass == TakeScanQRCodeResult(&(S_ReTestPageBuffer->scancode)))
	{
		if(S_ReTestPageBuffer->scancode == CardUnsupported)
		{
			dspReTestStatus("Unsupported\0");
			EndOneReTest("不支持的试剂卡\0");
			S_ReTestPageBuffer->reTestData.retestsurpluscount = 0;
			S_ReTestPageBuffer->reTestData.retestStep = 4;
			MotorMoveTo(MaxLocation, 0);
		}
		//过期
		else if(S_ReTestPageBuffer->scancode == CardCodeTimeOut)
		{
			dspReTestStatus("Card Out Date\0");
			EndOneReTest("二维码读取过期\0");
			S_ReTestPageBuffer->reTestData.retestsurpluscount = 0;
			S_ReTestPageBuffer->reTestData.retestStep = 4;
			MotorMoveTo(MaxLocation, 0);
		}
		//读取成功
		else if(S_ReTestPageBuffer->scancode == CardCodeScanOK)
		{
			//读取校准参数
			memcpy(S_ReTestPageBuffer->reTestData.itemData.testdata.tempadjust.ItemName, S_ReTestPageBuffer->reTestData.itemData.testdata.temperweima.ItemName, AdjItemNameLen);
			getAdjPram(getGBSystemSetData(), &(S_ReTestPageBuffer->reTestData.itemData.testdata.tempadjust));

			S_ReTestPageBuffer->reTestData.retestStep = 3;
			S_ReTestPageBuffer->reTestData.itemData.ledLight = getTestLedLightIntensity(getGBSystemSetData());
			StartTest(&(S_ReTestPageBuffer->reTestData.itemData));
		}
		/*其他错误：CardCodeScanFail, CardCodeCardOut, CardCodeScanTimeOut, CardCodeCRCError*/
		else
		{
			dspReTestStatus("QR Read Fail\0");
			EndOneReTest("二维码读取失败\0");
			S_ReTestPageBuffer->reTestData.retestsurpluscount = 0;
			S_ReTestPageBuffer->reTestData.retestStep = 4;
			MotorMoveTo(MaxLocation, 0);
		}
	}
}

static void CheckTestCard(void)
{
	if(My_Pass == TakeTestResult(&(S_ReTestPageBuffer->cardpretestresult)))
	{
		EndOneReTest("测试正常\0");
		MotorMoveTo(MaxLocation, 0);
	}
}

static void dspReTestStatus(char * str)
{
	ClearText(0x2828, 30);
	memset(S_ReTestPageBuffer->buf, 0, 100);
	sprintf(S_ReTestPageBuffer->buf, "%s", str);
	DisText(0x2828, S_ReTestPageBuffer->buf, strlen(S_ReTestPageBuffer->buf));
}

static void dspReTestLogs(void)
{
	//已测
	DspNum(0x2810 , S_ReTestPageBuffer->reTestData.retestedcount, 4);
	//剩余
	DspNum(0x2815 , S_ReTestPageBuffer->reTestData.retestsurpluscount, 4);
	//已测时间
	DspNum(0x281a , timer_Count(&(S_ReTestPageBuffer->reTestData.retesttimer)), 4);
}
/****************************************end of file************************************************/
