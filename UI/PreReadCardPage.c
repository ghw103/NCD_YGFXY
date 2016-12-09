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
#include	"MyTools.h"
#include	"CRC16.h"

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
	PageInfo * currentpage = NULL;
	
	if(My_Pass == GetCurrentPage(&currentpage))
	{
		currentpage->PageInit = PageInit;
		currentpage->PageUpDate = PageUpDate;
		currentpage->LCDInput = Input;
		currentpage->PageBufferMalloc = PageBufferMalloc;
		currentpage->PageBufferFree = PageBufferFree;
		currentpage->tempP = &S_PreReadPageBuffer;
		
		currentpage->PageInit(currentpage->pram);
	}
	
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
		if((S_PreReadPageBuffer->lcdinput[0] == 0x1500) || (S_PreReadPageBuffer->lcdinput[0] == 0x1501) || (S_PreReadPageBuffer->lcdinput[0] == 0x1502)
			|| (S_PreReadPageBuffer->lcdinput[0] == 0x1504) || (S_PreReadPageBuffer->lcdinput[0] == 0x1505))
		{
			/*数据*/
			S_PreReadPageBuffer->lcdinput[1] = pbuf[7];
			S_PreReadPageBuffer->lcdinput[1] = (S_PreReadPageBuffer->lcdinput[1]<<8) + pbuf[8];
			
			/*更换检测卡*/
			if(S_PreReadPageBuffer->lcdinput[1] == 0x0001)
			{
				PageBufferFree();
				PageBackTo(ParentPage);
			}
			/*退出*/
			else if(S_PreReadPageBuffer->lcdinput[1] == 0x0000)
			{
				DeleteCurrentTest();
				
				PageBufferFree();

				PageBackTo(ParentPage);
			}
		}
		/*排队模式，与之前注册的卡id不同*/
		else if(S_PreReadPageBuffer->lcdinput[0] == 0x1503)
		{
			/*数据*/
			S_PreReadPageBuffer->lcdinput[1] = pbuf[7];
			S_PreReadPageBuffer->lcdinput[1] = (S_PreReadPageBuffer->lcdinput[1]<<8) + pbuf[8];
			
			/*取消测试*/
			if(S_PreReadPageBuffer->lcdinput[1] == 0x0000)
			{
				DeleteCurrentTest();
				
				PageBufferFree();
				PageBackTo(ParentPage);
			}
			/*继续测试*/
			else if(S_PreReadPageBuffer->lcdinput[1] == 0x0001)
			{
				DeleteCurrentTest();
				
				PageBufferFree();
				PageBackTo(ParentPage);
			}
		}
	}
}

static void PageUpDate(void)
{
	CheckQRCode();
	
	CheckPreTestCard();
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Pass == PageBufferMalloc())
	{
		/*清除界面*/
		ClearText(0x1520, 30);
		ClearText(0x1530, 30);
		ClearText(0x1540, 10);
		ClearText(0x1545, 20);
		
		S_PreReadPageBuffer->currenttestdata = GetCurrentTestItem();
		
		AddNumOfSongToList(44, 0);
		
		StartScanQRCode(&(S_PreReadPageBuffer->temperweima));
	}
	
	SelectPage(92);
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	if(NULL == S_PreReadPageBuffer)
	{
		S_PreReadPageBuffer = (PreReadPageBuffer *)MyMalloc(sizeof(PreReadPageBuffer));
			
		if(S_PreReadPageBuffer)
		{
			memset(S_PreReadPageBuffer, 0, sizeof(PreReadPageBuffer));
			
			return My_Pass;
		}
	}
	return My_Fail;
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
			//获取检测卡温度
			S_PreReadPageBuffer->currenttestdata->testdata.TestTemp.O_Temperature = GetCardTemperature();
			
			ShowCardInfo();
			
			//如果测试数据包中的二维码crc校验错误，则表明是第一次读取二维码
			if(S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CRC16 == 0)
			{
				//将读取的二维码数据拷贝到测试数据包中
				memcpy(&(S_PreReadPageBuffer->currenttestdata->testdata.temperweima), &(S_PreReadPageBuffer->temperweima), sizeof(CardCodeInfo));
				
				//设置倒计时时间
				timer_set(&(S_PreReadPageBuffer->currenttestdata->timer), S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardWaitTime*4);
			
				//读取校准参数
				memcpy(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust.ItemName, S_PreReadPageBuffer->currenttestdata->testdata.temperweima.ItemName, ItemNameLen);
				if(My_Fail == ReadAdjustData(&(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust)))
					memset(&(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust), 0, sizeof(AdjustData));
				
				StartTest(&(S_PreReadPageBuffer->currenttestdata->testdata));
			}
			else
			{
				//校验试机卡编号
				if(pdFAIL == CheckStrIsSame(S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardPiCi, S_PreReadPageBuffer->temperweima.CardPiCi, CardPiCiHaoLen))
					SendKeyCode(4);
				else
				{
					PageBufferFree();

					PageAdvanceTo(DspTimeDownNorPage, NULL);
				}
			}
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
	/*	else if(S_PreReadPageBuffer->cardpretestresult == NoSample)
		{
			SendKeyCode(6);
			MotorMoveTo(GetGB_MotorMaxLocation(), 1);
			AddNumOfSongToList(22, 0);
		}*/
		else //if(S_PreReadPageBuffer->cardpretestresult == PeakError)
		{
			//如果是排队模式，则进入排队界面
			if(S_PreReadPageBuffer->currenttestdata->testlocation > 0)
			{
				MotorMoveTo(MaxLocation, 1);
				
				S_PreReadPageBuffer->currenttestdata->statues = startpaidui;
				timer_restart(&(S_PreReadPageBuffer->currenttestdata->timer));

				PageBufferFree();
				PageAdvanceTo(DspPaiDuiPage, NULL);
			}
			else
			{		
				timer_restart(&(S_PreReadPageBuffer->currenttestdata->timer));
				
				PageBufferFree();

				PageAdvanceTo(DspTimeDownNorPage, NULL);
			}
		}
/*		else if(S_PreReadPageBuffer->cardpretestresult == ResultIsOK)
		{
			
			
			SendKeyCode(3);
			MotorMoveTo(GetGB_MotorMaxLocation(), 1);
			AddNumOfSongToList(22, 0);
		}*/
	}
}

static void ShowCardInfo(void)
{
	sprintf(S_PreReadPageBuffer->buf, "%s", S_PreReadPageBuffer->temperweima.ItemName);
	DisText(0x1520, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
			
	sprintf(S_PreReadPageBuffer->buf, "%s", S_PreReadPageBuffer->temperweima.CardPiCi);
	DisText(0x1530, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
			
	sprintf(S_PreReadPageBuffer->buf, "%d", S_PreReadPageBuffer->temperweima.CardWaitTime*60);
	DisText(0x1540, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
			
	sprintf(S_PreReadPageBuffer->buf, "20%02d年%02d月%02d日", S_PreReadPageBuffer->temperweima.CardBaoZhiQi.year, S_PreReadPageBuffer->temperweima.CardBaoZhiQi.month,
		S_PreReadPageBuffer->temperweima.CardBaoZhiQi.day);
	DisText(0x1545, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
	
	sprintf(S_PreReadPageBuffer->buf, "%2.1f", S_PreReadPageBuffer->currenttestdata->testdata.TestTemp.O_Temperature);
	DisText(0x1550, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
}


