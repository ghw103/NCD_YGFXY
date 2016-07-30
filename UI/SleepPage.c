/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SleepPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"Time_Data.h"

#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static SleepPageBuffer *S_SleepPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpData(void);
static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void DspLogo(void);
static void DspPageText(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspSleepPage(void *  parm)
{
	SysPage * myPage = GetSysPage();

	myPage->ParentPage = myPage->CurrentPage;
	myPage->CurrentPage = DspSleepPage;
	myPage->LCDInput = Input;
	myPage->PageUpData = PageUpData;
	myPage->ChildPage = NULL;
	myPage->PageInit = PageInit;
	myPage->PageBufferMalloc = PageBufferMalloc;
	myPage->PageBufferFree = PageBufferFree;
	
	myPage->PageInit(parm);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	unsigned short *pdata = NULL;
	unsigned char error = 0;
	SysPage * myPage = GetSysPage();
	
	pdata = MyMalloc((len/2)*sizeof(unsigned short));
	if(pdata == NULL)
		return;
	
	/*命令*/
	pdata[0] = pbuf[4];
	pdata[0] = (pdata[0]<<8) + pbuf[5];
	
	/*设置*/
	if(pdata[0] == 0x2f00)
	{
		SetLEDLight(100);
		myPage->PageBufferFree();
		myPage->ParentPage(NULL);
	}
	
	MyFree(pdata);
}

static void PageUpData(void)
{
	if(S_SleepPageBuffer)
	{
		if((S_SleepPageBuffer->count % 10) == 0)
			DspPageText();
		
		S_SleepPageBuffer->count++;
	}
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SetLEDLight(1);
	
	SelectPage(115);
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(NULL == S_SleepPageBuffer)
	{
		S_SleepPageBuffer = MyMalloc(sizeof(SleepPageBuffer));
		if(S_SleepPageBuffer)
		{
			memset(S_SleepPageBuffer, 0, sizeof(SleepPageBuffer));
			return My_Pass;
		}
	}
	
	return My_Fail;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_SleepPageBuffer);
	S_SleepPageBuffer = NULL;
	return My_Pass;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/************************************************************************
** 函数名:PageUpData
** 功  能:当前页面数据更新函数
** 输  入:无
** 输  出:无
** 返  回：无
** 备  注：无
** 时  间:  
** 作  者：xsx                                                 
************************************************************************/
static void DspLogo(void)
{
	if(S_SleepPageBuffer)
	{
		if(S_SleepPageBuffer->x_direct == 0)
		{
			S_SleepPageBuffer->x++;
			if(S_SleepPageBuffer->x >=  787)
				S_SleepPageBuffer->x_direct = 1;
		}
		else
		{
			S_SleepPageBuffer->x--;
			if(S_SleepPageBuffer->x <= 0)
				S_SleepPageBuffer->x_direct = 0;
		}
		
		if(S_SleepPageBuffer->y_direct == 0)
		{
			S_SleepPageBuffer->y++;
			if(S_SleepPageBuffer->y >=  440)
				S_SleepPageBuffer->y_direct = 1;
		}
		else
		{
			S_SleepPageBuffer->y--;
			if(S_SleepPageBuffer->y <= 0)
				S_SleepPageBuffer->y_direct = 0;
		}
		
		S_SleepPageBuffer->myBasicICO.ICO_ID = 29;
		S_SleepPageBuffer->myBasicICO.X = S_SleepPageBuffer->x;
		S_SleepPageBuffer->myBasicICO.Y = S_SleepPageBuffer->y;
	
		BasicUI(0x2f10 ,0x1907 , 1, &(S_SleepPageBuffer->myBasicICO) , sizeof(Basic_ICO));
	}
}

static void DspPageText(void)
{
	if(S_SleepPageBuffer)
	{
		GetGBTimeData(&(S_SleepPageBuffer->time));
		
		memset(S_SleepPageBuffer->buf, 0, 50);
		sprintf(S_SleepPageBuffer->buf, "20%02d-%02d-%02d %02d:%02d:%02d", S_SleepPageBuffer->time.year, S_SleepPageBuffer->time.month, S_SleepPageBuffer->time.day,
			S_SleepPageBuffer->time.hour, S_SleepPageBuffer->time.min, S_SleepPageBuffer->time.sec);
	
		DisText(0x2f20, S_SleepPageBuffer->buf, strlen(S_SleepPageBuffer->buf));
	}
}
