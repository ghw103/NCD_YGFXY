/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SystemSetPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"LunchPage.h"
#include	"ShowDeviceInfoPage.h"
#include	"AdjustPage.h"
#include	"ReTestPage.h"
#include	"UserMPage.h"
#include	"NetPreSetPage.h"
#include	"RecordPage.h"
#include	"OtherSetPage.h"
#include	"MyTools.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static SysSetPageBuffer * S_SysSetPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);
static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspSystemSetPage(void *  parm)
{
	PageInfo * currentpage = NULL;
	
	if(My_Pass == GetCurrentPage(&currentpage))
	{
		currentpage->PageInit = PageInit;
		currentpage->PageUpDate = PageUpDate;
		currentpage->LCDInput = Input;
		currentpage->PageBufferMalloc = PageBufferMalloc;
		currentpage->PageBufferFree = PageBufferFree;
		currentpage->tempP = &S_SysSetPageBuffer;
		
		currentpage->PageInit(currentpage->pram);
	}
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_SysSetPageBuffer)
	{
		/*命令*/
		S_SysSetPageBuffer->lcdinput[0] = pbuf[4];
		S_SysSetPageBuffer->lcdinput[0] = (S_SysSetPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		//基本信息
		if(S_SysSetPageBuffer->lcdinput[0] == 0x1901)
		{
			PageBufferFree();
			PageAdvanceTo(DspShowDeviceInfoPage, NULL);
		}
		//操作人管理
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1902)
		{
			PageBufferFree();
			PageAdvanceTo(DspUserMPage, NULL);
		}
		//网络设置
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1903)
		{
			PageBufferFree();
			PageAdvanceTo(DspNetPreSetPage, NULL);
		}
		//数据管理
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1904)
		{
			PageBufferFree();
			PageAdvanceTo(DspRecordPage, NULL);
		}
		//校准功能
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1908)
		{
			if(GetBufLen(&pbuf[7] , 2*pbuf[6]) == 6)
			{
				if(pdPASS == CheckStrIsSame(&pbuf[7] , AdjustPassWord ,GetBufLen(&pbuf[7] , 2*pbuf[6])))
				{
					PageBufferFree();
					PageAdvanceTo(DspAdjustPage, NULL);
				}
				else if(pdPASS == CheckStrIsSame(&pbuf[7] , TestPassWord ,GetBufLen(&pbuf[7] , 2*pbuf[6])))
				{
					PageBufferFree();
					PageAdvanceTo(DspReTestPage, NULL);
				}
				else
					SendKeyCode(1);
			}
			else
					SendKeyCode(1);
		}
		//其他设置
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1905)
		{
			PageBufferFree();
			PageAdvanceTo(DspOtherSetPage, NULL);
		}
		//返回
		else if(S_SysSetPageBuffer->lcdinput[0] == 0x1900)
		{
			PageBackTo(ParentPage);
		}
	}
}

static void PageUpDate(void)
{

}

/***************************************************************************************************
*FunctionName：PageInit
*Description：当前界面初始化
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月27日08:55:25
***************************************************************************************************/
static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(98);
	
	return My_Pass;
}

/***************************************************************************************************
*FunctionName：PageBufferMalloc
*Description：当前界面临时缓存申请
*Input：None
*Output：MyState_TypeDef -- 返回成功与否
*Author：xsx
*Data：2016年6月27日08:56:02
***************************************************************************************************/
static MyState_TypeDef PageBufferMalloc(void)
{
	if(NULL == S_SysSetPageBuffer)
	{
		S_SysSetPageBuffer = (SysSetPageBuffer *)MyMalloc(sizeof(SysSetPageBuffer));
		
		if(NULL == S_SysSetPageBuffer)
			return My_Fail;
	}
	
	memset(S_SysSetPageBuffer, 0, sizeof(SysSetPageBuffer));
	return My_Pass;
}

/***************************************************************************************************
*FunctionName：PageBufferFree
*Description：当前界面临时缓存释放
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月27日08:56:21
***************************************************************************************************/
static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_SysSetPageBuffer);
	S_SysSetPageBuffer = NULL;
	
	return My_Pass;
}



