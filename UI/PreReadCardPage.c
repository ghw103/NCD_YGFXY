/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"PreReadCardPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"MyTest_Data.h"
#include 	"MLX90614_Driver.h"
#include	"LunchPage.h"
#include	"CodeScan_Task.h"
#include	"WaittingCardPage.h"
#include	"TimeDownNorPage.h"
#include	"CodeScanFunction.h"
#include	"PaiDuiPage.h"
#include	"CardStatues_Data.h"
#include	"Motor_Fun.h"
#include	"PlaySong_Task.h"
#include	"Test_Task.h"
#include	"SDFunction.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static PreReadPageBuffer * S_PreReadPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);
static void ShowCardInfo(void);
static void ShowTemp(void);

static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void CheckQRCode(void);
static void CheckPreTestCard(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspPreReadCardPage(void *  parm)
{
	SetGBSysPage(DspPreReadCardPage, DspWaittingCardPage, DspTimeDownNorPage, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_PreReadPageBuffer)
	{
		/*命令*/
		S_PreReadPageBuffer->lcdinput[0] = pbuf[4];
		S_PreReadPageBuffer->lcdinput[0] = (S_PreReadPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*二维码读取失败，过期，已使用*/
		if((S_PreReadPageBuffer->lcdinput[0] == 0x1f10) || (S_PreReadPageBuffer->lcdinput[0] == 0x1f11) || (S_PreReadPageBuffer->lcdinput[0] == 0x1f12))
		{
			/*数据*/
			S_PreReadPageBuffer->lcdinput[1] = pbuf[7];
			S_PreReadPageBuffer->lcdinput[1] = (S_PreReadPageBuffer->lcdinput[1]<<8) + pbuf[8];
			
			/*更换检测卡*/
			if(S_PreReadPageBuffer->lcdinput[1] == 0x0002)
			{
				GBPageBufferFree();
				GotoGBParentPage(NULL);
			}
			/*退出*/
			else if(S_PreReadPageBuffer->lcdinput[1] == 0x0001)
			{
				DeleteCurrentTest();
				
				GBPageBufferFree();
				SetGBParentPage(DspLunchPage);
				GotoGBParentPage(NULL);
			}
		}
		/*排队模式，与之前注册的卡id不同*/
		else if(S_PreReadPageBuffer->lcdinput[0] == 0x1f13)
		{
			/*数据*/
			S_PreReadPageBuffer->lcdinput[1] = pbuf[7];
			S_PreReadPageBuffer->lcdinput[1] = (S_PreReadPageBuffer->lcdinput[1]<<8) + pbuf[8];
			
			/*取消测试*/
			if(S_PreReadPageBuffer->lcdinput[1] == 0x0001)
			{
				DeleteCurrentTest();
				
				GBPageBufferFree();
				SetGBParentPage(DspLunchPage);
				GotoGBParentPage(NULL);
			}
			/*继续测试*/
			else if(S_PreReadPageBuffer->lcdinput[1] == 0x0002)
			{
				DeleteCurrentTest();
				
				GBPageBufferFree();
				SetGBParentPage(DspLunchPage);
				GotoGBParentPage(NULL);
			}
		}
	}
}

static void PageUpDate(void)
{
	CheckQRCode();
	
	CheckPreTestCard();
	
	if((S_PreReadPageBuffer)&&(TimeOut == timer_expired(&(S_PreReadPageBuffer->timer2))))
	{	
		timer_restart(&(S_PreReadPageBuffer->timer2));
		ShowTemp();
	}
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(62);
	
	/*清除界面*/
	ClearText(0x1f20, 30);
	ClearText(0x1f30, 30);
	ClearText(0x1f40, 10);
	ClearText(0x1f45, 20);
	
	S_PreReadPageBuffer->currenttestdata = GetCurrentTestItem();
	
	AddNumOfSongToList(44, 0);
	
	/*更新数据*/
	timer_set(&(S_PreReadPageBuffer->timer2), 1);
	
	StartScanQRCode(&(S_PreReadPageBuffer->currenttestdata->testdata.temperweima));
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	if(S_PreReadPageBuffer == NULL)
	{
		S_PreReadPageBuffer = (PreReadPageBuffer *)MyMalloc(sizeof(PreReadPageBuffer));
			
		if(S_PreReadPageBuffer)
		{
			memset(S_PreReadPageBuffer, 0, sizeof(PreReadPageBuffer));
			
			return My_Pass;
		}
		else
			return My_Fail;
	}
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_PreReadPageBuffer);
	S_PreReadPageBuffer = NULL;
	
	return My_Pass;
}

static void CheckQRCode(void)
{
	if((S_PreReadPageBuffer) && (My_Pass == TakeScanQRCodeResult(&(S_PreReadPageBuffer->scancode))))
	{
		if((S_PreReadPageBuffer->scancode == CardCodeScanFail) || (S_PreReadPageBuffer->scancode == CardCodeCardOut) ||
			(S_PreReadPageBuffer->scancode == CardCodeScanTimeOut) || (S_PreReadPageBuffer->scancode == CardCodeCRCError))
		{
			SendKeyCode(1);
			MotorMoveTo(GetGB_MotorMaxLocation(), 1);
			AddNumOfSongToList(23, 0);
		}
		else if(S_PreReadPageBuffer->scancode == CardCodeTimeOut)
		{
			SendKeyCode(2);
			MotorMoveTo(GetGB_MotorMaxLocation(), 1);
			AddNumOfSongToList(21, 0);
		}
		else if(S_PreReadPageBuffer->scancode == CardCodeScanOK)
		{
			//获取检测卡温度
			S_PreReadPageBuffer->currenttestdata->testdata.TestTemp.O_Temperature = GetCardTemperature();
			
			//设置倒计时时间
			timer_set(&(S_PreReadPageBuffer->currenttestdata->timer), S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardWaitTime*1);
			
			//读取校准参数
			memcpy(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust.ItemName, S_PreReadPageBuffer->currenttestdata->testdata.temperweima.ItemName, ItemNameLen);
			if(My_Fail == ReadAdjustData(&(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust)))
				memset(&(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust), 0, sizeof(AdjustData));
			
			ShowCardInfo();
			
			StartTest(&(S_PreReadPageBuffer->currenttestdata->testdata));
		}
	}
}

static void CheckPreTestCard(void)
{
	if((S_PreReadPageBuffer) && (My_Pass == TakeTestResult(&(S_PreReadPageBuffer->cardpretestresult))))
	{
		if(S_PreReadPageBuffer->cardpretestresult == TestInterrupt)
		{
			SendKeyCode(5);
			MotorMoveTo(GetGB_MotorMaxLocation(), 1);
			AddNumOfSongToList(22, 0);
		}

	/*	else if(S_PreReadPageBuffer->cardpretestresult == PeakNumError)
		{
			SendKeyCode(3);
			MotorMoveTo(GetGB_MotorMaxLocation(), 1);
			AddNumOfSongToList(22, 0);
		}*/
		else if(S_PreReadPageBuffer->cardpretestresult == ResultIsOK)
		{
			//如果是排队模式，则进入排队界面
			if((S_PreReadPageBuffer->currenttestdata->testlocation > 0)&&(S_PreReadPageBuffer->currenttestdata->testlocation < PaiDuiWeiNum))
			{
				MotorMoveTo(GetGB_MotorMaxLocation(), 1);
				S_PreReadPageBuffer->currenttestdata->statues = statuesNull;
				timer_restart(&(S_PreReadPageBuffer->currenttestdata->timer));
				
				ClearCardStatuesQueue();
				PageBufferFree();
				DspPaiDuiPage(NULL);
			}
			else
			{
				S_PreReadPageBuffer->currenttestdata->statues = statues7;			
				timer_restart(&(S_PreReadPageBuffer->currenttestdata->timer));
				
				PageBufferFree();
				DspTimeDownNorPage(NULL);
			}
		}
	}
}

static void ShowCardInfo(void)
{
	sprintf(S_PreReadPageBuffer->buf, "%s", S_PreReadPageBuffer->currenttestdata->testdata.temperweima.ItemName);
	DisText(0x1f20, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
			
	sprintf(S_PreReadPageBuffer->buf, "%s", S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardPiCi);
	DisText(0x1f30, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
			
	sprintf(S_PreReadPageBuffer->buf, "%d", S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardWaitTime*60);
	DisText(0x1f40, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
			
	sprintf(S_PreReadPageBuffer->buf, "20%02d年%02d月%02d日", S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardBaoZhiQi.year, S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardBaoZhiQi.month,
		S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardBaoZhiQi.day);
	DisText(0x1f45, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
}
static void ShowTemp(void)
{
	sprintf(S_PreReadPageBuffer->buf, "%2.1f", GetCardTemperature());
	DisText(0x1f50, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
}

