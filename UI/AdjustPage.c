/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"AdjustPage.h"

#include	"LCD_Driver.h"
#include	"Define.h"
#include	"MyMem.h"

#include	"SystemSetPage.h"
#include	"PlaySong_Task.h"
#include	"UI_Data.h"
#include	"CardStatues_Data.h"
#include	"CodeScan_Task.h"
#include	"Test_Task.h"
#include	"SDFunction.h"

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
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);
static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void CheckQRCode(void);
static void CheckPreTestCard(void);
static void DspPage2Text(void);
static void DspPage3Text(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspAdjustPage(void *  parm)
{
	PageInfo * currentpage = NULL;
	
	if(My_Pass == GetCurrentPage(&currentpage))
	{
		currentpage->PageInit = PageInit;
		currentpage->PageUpDate = PageUpDate;
		currentpage->LCDInput = Input;
		currentpage->PageBufferMalloc = PageBufferMalloc;
		currentpage->PageBufferFree = PageBufferFree;
		
		currentpage->PageInit(currentpage->pram);
	}
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_AdjustPageBuffer)
	{
		/*命令*/
		S_AdjustPageBuffer->lcdinput[0] = pbuf[4];
		S_AdjustPageBuffer->lcdinput[0] = (S_AdjustPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*退出*/
		if(S_AdjustPageBuffer->lcdinput[0] == 0x2f30)
		{
			PageBufferFree();
			PageBackTo(ParentPage);
		}
		/*校准*/
		else if(S_AdjustPageBuffer->lcdinput[0] == 0x2f32)
		{
			if((S_AdjustPageBuffer->testdata.testdata.testline.BasicResult != 0) && (S_AdjustPageBuffer->targetresult != 0))
			{
				S_AdjustPageBuffer->tempadjust.parm = S_AdjustPageBuffer->targetresult / S_AdjustPageBuffer->testdata.testdata.testline.BasicResult;
				SaveAdjustData(&(S_AdjustPageBuffer->tempadjust));
				SelectPage(92);
				DspPage3Text();
			}
		}
		/*输入标准值*/
		else if(S_AdjustPageBuffer->lcdinput[0] == 0x2f48)
		{
			memset(S_AdjustPageBuffer->buf, 0 , 100);
			memcpy(S_AdjustPageBuffer->buf, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_AdjustPageBuffer->targetresult = strtod(S_AdjustPageBuffer->buf, NULL);
		}
	}
}

static void PageUpDate(void)
{
/*	if(TimeOut == timer_expired(&(S_AdjustPageBuffer->timer)))
	{
		PageBufferFree();
		PageBackTo(1, NULL);
	}*/
	
	if((S_AdjustPageBuffer) && (S_AdjustPageBuffer->step == 0) && (GetCardState() == CardIN))
	{
		S_AdjustPageBuffer->step = 1;
		SelectPage(90);
		DspPage2Text();
		StartScanQRCode(&(S_AdjustPageBuffer->testdata.testdata.temperweima));
	}
	
	if((S_AdjustPageBuffer) && (S_AdjustPageBuffer->step == 1))
		CheckQRCode();
	
	if((S_AdjustPageBuffer) && (S_AdjustPageBuffer->step == 2))
		CheckPreTestCard();
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(88);
	
	timer_set(&(S_AdjustPageBuffer->timer), 30);
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(NULL == S_AdjustPageBuffer)
	{
		S_AdjustPageBuffer = MyMalloc(sizeof(AdjustPageBuffer));
		if(NULL == S_AdjustPageBuffer)	
			return My_Fail;
	}
	
	memset(S_AdjustPageBuffer, 0, sizeof(AdjustPageBuffer));
	
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_AdjustPageBuffer);
	S_AdjustPageBuffer = NULL;
	return My_Pass;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static void DspPage2Text(void)
{
	if(S_AdjustPageBuffer)
	{
		if(S_AdjustPageBuffer->testdata.testdata.testline.BasicResult == 0)
		{
			memset(S_AdjustPageBuffer->buf, 0, 100);
			sprintf(S_AdjustPageBuffer->buf, "Wait...");
			DisText(0x2f40, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
			DisText(0x2f48, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
		}
		else
		{
			memset(S_AdjustPageBuffer->buf, 0, 100);
			sprintf(S_AdjustPageBuffer->buf, "%.2f", S_AdjustPageBuffer->testdata.testdata.testline.BasicResult);
			DisText(0x2f40, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
			ClearText(0x2f48, 10);
		}
	}
}

static void DspPage3Text(void)
{
	if(S_AdjustPageBuffer)
	{
		
		if(My_Fail == ReadAdjustData(&(S_AdjustPageBuffer->tempadjust)))
			memset(&(S_AdjustPageBuffer->tempadjust), 0, sizeof(AdjustData));
		
		memset(S_AdjustPageBuffer->buf, 0, 100);
		sprintf(S_AdjustPageBuffer->buf, "%.3f", S_AdjustPageBuffer->tempadjust.parm);
		DisText(0x2f50, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
		
		memset(S_AdjustPageBuffer->buf, 0, 100);
		S_AdjustPageBuffer->testdata.testdata.testline.AdjustResult = S_AdjustPageBuffer->tempadjust.parm * S_AdjustPageBuffer->testdata.testdata.testline.BasicResult;
		sprintf(S_AdjustPageBuffer->buf, "%.2f", S_AdjustPageBuffer->testdata.testdata.testline.AdjustResult);
		DisText(0x2f58, S_AdjustPageBuffer->buf, strlen(S_AdjustPageBuffer->buf));
	}
}

static void CheckQRCode(void)
{
	if((S_AdjustPageBuffer) && (My_Pass == TakeScanQRCodeResult(&(S_AdjustPageBuffer->scancode))))
	{
		if((S_AdjustPageBuffer->scancode == CardCodeScanFail) || (S_AdjustPageBuffer->scancode == CardCodeCardOut) ||
			(S_AdjustPageBuffer->scancode == CardCodeScanTimeOut) || (S_AdjustPageBuffer->scancode == CardCodeCRCError))
		{
			PageBufferFree();
			PageBackTo(ParentPage);
		}
		else
		{
			memcpy(S_AdjustPageBuffer->tempadjust.ItemName, S_AdjustPageBuffer->testdata.testdata.temperweima.ItemName, ItemNameLen);
			S_AdjustPageBuffer->step = 2;
			StartTest(&(S_AdjustPageBuffer->testdata.testdata));
		}
	}
}

static void CheckPreTestCard(void)
{
	if((S_AdjustPageBuffer) && (My_Pass == TakeTestResult(&(S_AdjustPageBuffer->cardpretestresult))))
	{

		if(S_AdjustPageBuffer->cardpretestresult != ResultIsOK)
		{
			PageBufferFree();
			PageBackTo(ParentPage);
		}
		else 
		{
			S_AdjustPageBuffer->step = 3;
			DspPage2Text();
		}
	}
}
