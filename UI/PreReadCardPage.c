/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"PreReadCardPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"MyTest_Data.h"

#include	"LunchPage.h"
#include	"CodeScan_Task.h"
#include	"WaittingCardPage.h"
#include	"TimeDownNorPage.h"
#include	"Temperature_Data.h"
#include	"CodeScanFunction.h"
#include	"PaiDuiPage.h"
#include	"CardStatues_Data.h"
#include	"Motor_Fun.h"
#include	"PlaySong_Task.h"
#include	"Test_Task.h"

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
static void PageUpData(void);
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
	SysPage * myPage = GetSysPage();

	myPage->CurrentPage = DspPreReadCardPage;
	myPage->LCDInput = Input;
	myPage->PageUpData = PageUpData;
	myPage->ParentPage = DspWaittingCardPage;
	myPage->ChildPage = DspTimeDownNorPage;
	myPage->PageInit = PageInit;
	myPage->PageBufferMalloc = PageBufferMalloc;
	myPage->PageBufferFree = PageBufferFree;
	
	SelectPage(62);
	
	myPage->PageInit(parm);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	unsigned short *pdata = NULL;
	
	pdata = MyMalloc((len/2)*sizeof(unsigned short));
	if(pdata == NULL)
		return;
	
	/*命令*/
	pdata[0] = pbuf[4];
	pdata[0] = (pdata[0]<<8) + pbuf[5];
	
	/*二维码读取失败，过期，已使用*/
	if((pdata[0] == 0x1f10) || (pdata[0] == 0x1f11) || (pdata[0] == 0x1f12))
	{
		/*数据*/
		pdata[1] = pbuf[7];
		pdata[1] = (pdata[1]<<8) + pbuf[8];
		
		/*更换检测卡*/
		if(pdata[1] == 0x0002)
		{
			PageBufferFree();
			DspWaittingCardPage(NULL);
		}
		/*退出*/
		else if(pdata[1] == 0x0001)
		{
			PageBufferFree();
			DeleteCurrentTest();
			DspLunchPage(NULL);
		}
	}
	/*排队模式，与之前注册的卡id不同*/
	else if(pdata[0] == 0x1f13)
	{
		/*数据*/
		pdata[1] = pbuf[7];
		pdata[1] = (pdata[1]<<8) + pbuf[8];
		
		/*取消测试*/
		if(pdata[1] == 0x0001)
		{
			PageBufferFree();
			DeleteCurrentTest();
			DspLunchPage(NULL);
		}
		/*继续测试*/
		else if(pdata[1] == 0x0002)
		{
			PageBufferFree();
			DeleteCurrentTest();
			DspLunchPage(NULL);
		}
	}

	MyFree(pdata);
}

static void PageUpData(void)
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
		if(S_PreReadPageBuffer->scancode == CardCodeScanFail)
		{
			SendKeyCode(1);
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(23, 0);
		}
		else if(S_PreReadPageBuffer->scancode == CardCodeTimeOut)
		{
			SendKeyCode(2);
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(21, 0);
		}
		else if(S_PreReadPageBuffer->scancode == CardCodeScanOK)
		{
			//设置倒计时时间
			timer_set(&(S_PreReadPageBuffer->currenttestdata->timer), S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardWaitTime*2);
			
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
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(22, 0);
		}
		else if((S_PreReadPageBuffer->cardpretestresult == PeakNumError)||(S_PreReadPageBuffer->cardpretestresult == ResultIsOK))
		{
			SendKeyCode(3);
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(22, 0);
		}
		else if(S_PreReadPageBuffer->cardpretestresult == PeakNumZero)
		{
			//如果是排队模式，则进入排队界面
			if((S_PreReadPageBuffer->currenttestdata->testlocation > 0)&&(S_PreReadPageBuffer->currenttestdata->testlocation < PaiDuiWeiNum))
			{
				MotorMoveTo(MaxLocation, 1);
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
			
	sprintf(S_PreReadPageBuffer->buf, "20%02d年%02d月%02d日", S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardBaoZhiQi.RTC_Year, S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardBaoZhiQi.RTC_Month,
		S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardBaoZhiQi.RTC_Date);
	DisText(0x1f45, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
}
static void ShowTemp(void)
{
	sprintf(S_PreReadPageBuffer->buf, "%2.1f", GetCardTemperature());
	DisText(0x1f50, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
		
	sprintf(S_PreReadPageBuffer->buf, "%2.1f", GetEnviTemperature());
	DisText(0x1f58, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
}

