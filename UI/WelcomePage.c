/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"WelcomePage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"SelfCheck_Data.h"
#include	"PlaySong_Task.h"

#include	"LunchPage.h"

#include	"ReadBarCode_Task.h"
#include	"Test_Task.h"
#include 	"netconf.h"
#include	"NormalUpLoad_Task.h"
#include	"CodeScan_Task.h"
#include	"Paidui_Task.h"

#include	<string.h>
#include	"stdio.h"
/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static WelcomePageBuffer * S_WelcomePageBuffer = NULL;
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

/***************************************************************************************************
*FunctionName：DspWelcomePage
*Description：欢迎界面
*Input：parm -- 参数
*Output：None
*Author：xsx
*Data：2016年6月27日08:53:22
***************************************************************************************************/
unsigned char DspWelcomePage(void *  parm)
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

/***************************************************************************************************
*FunctionName：Input
*Description：接收lcd输入
*Input：pbuf -- 输入数据
*		len -- 输入数据长度
*Output：None
*Author：xsx
*Data：2016年6月27日08:54:10
***************************************************************************************************/
static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_WelcomePageBuffer)
	{
		if(0x81 == pbuf[3])
		{
			//页面id
			if(0x03 == pbuf[4])
			{
				S_WelcomePageBuffer->lcdinput[0] = pbuf[6];
				S_WelcomePageBuffer->lcdinput[0] = (S_WelcomePageBuffer->lcdinput[0]<<8) + pbuf[7];
				
				//动画播放到末尾81号页面,且自检完成
				if((81 == S_WelcomePageBuffer->lcdinput[0]) && (SelfCheck_None != GetGB_SelfCheckStatus()))
				{
					/*创建读取条码枪任务*/
					StartBarCodeTask();
					
					/*开启测试任务*/
					StartvTestTask();
					
					/*开启网络任务*/
					StartEthernet();

					/*上传任务*/
					StartvNormalUpLoadTask();
					
					/*开启读二维码任务*/
					StartCodeScanTask();
					
					//开始排队任务
					StartPaiduiTask();
					
					PageBufferFree();
					PageAdvanceTo(DspLunchPage, NULL);
				}
			}
		}
		else if(0x83 == pbuf[3])
		{
			
		}
	}
}

/***************************************************************************************************
*FunctionName：PageUpData
*Description：界面数据更新，10ms执行一次
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月27日08:55:02
***************************************************************************************************/
static void PageUpDate(void)
{
	if(TimeOut == timer_expired(&(S_WelcomePageBuffer->timer)))
	{
		ReadCurrentPageId();
		
		timer_reset(&(S_WelcomePageBuffer->timer));
	}
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
	if(My_Pass == PageBufferMalloc())
	{
		timer_set(&(S_WelcomePageBuffer->timer), 1);
	}
	
	SetLEDLight(100);
	
	SelectPage(0);
	
	AddNumOfSongToList(52, 0);
	
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
	if(NULL == S_WelcomePageBuffer)
	{
		S_WelcomePageBuffer = (WelcomePageBuffer *)MyMalloc(sizeof(WelcomePageBuffer));
			
		if(S_WelcomePageBuffer)
		{
			memset(S_WelcomePageBuffer, 0, sizeof(WelcomePageBuffer));
		
			return My_Pass;
			
		}
	}
	
	return My_Fail;
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
	MyFree(S_WelcomePageBuffer);
	S_WelcomePageBuffer = NULL;
	return My_Pass;
}


