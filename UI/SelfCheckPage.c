/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SelfCheckPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"LunchPage.h"
#include	"MyMem.h"
#include	"PlaySong_Task.h"
#include	"SelfTest_Fun.h"
#include	"SelfCheck_Data.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
const char selfchecktextinfo[19][50] = {
	"加载服务器数据\0",
	"服务器数据加载失败\0",
	"服务器数据加载成功\0",
	"正在检测二维码模块\0", 
	"二维码模块错误\0", 
	"二维码模块正常\0" ,
	"正在检测存储模块\0", 
	"存储模块错误\0", 
	"存储模块正常\0" ,
	"正在检测WIFI模块\0",
	"WIFI模块错误\0",
	"WIFI模块正常\0",
	"正在检测采集模块\0",
	"采集模块错误\0",
	"采集模块正常\0",
	"正在检测传动模块\0",
	"传动模块错误\0",
	"传动模块正常\0",
	
	"自检结束",
};
const char selfcheckpicinfo[19] = {5, 10, 10, 15, 20, 20,25,30,30, 35, 45, 45, 55, 75, 75, 80,85, 85,100};
	
static SelCheckPage * S_SelCheckPage = NULL;								//自检页面缓存

/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);
static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void PageClear(void);
static void RefreshPageValue(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspSelfCheckPage(void *  parm)
{
	SetGBSysPage(DspSelfCheckPage, NULL, DspLunchPage, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_SelCheckPage)
	{
		/*命令*/
		S_SelCheckPage->lcdinput[0] = pbuf[4];
		S_SelCheckPage->lcdinput[0] = (S_SelCheckPage->lcdinput[0]<<8) + pbuf[5];
		
		/*如果是错误，弹出菜单*/
		if(S_SelCheckPage->lcdinput[0] == 0x1a30)
		{
			/*数据*/
			S_SelCheckPage->lcdinput[1] = pbuf[7];
			S_SelCheckPage->lcdinput[1] = (S_SelCheckPage->lcdinput[1]<<8) + pbuf[8];
			
			/*重启*/
			if(S_SelCheckPage->lcdinput[1] == 0x0001)
				while(1);
		}
		/*如果是告警，弹出菜单*/
		else if(S_SelCheckPage->lcdinput[0] == 0x1a31)
		{
			/*数据*/
			S_SelCheckPage->lcdinput[1] = pbuf[7];
			S_SelCheckPage->lcdinput[1] = (S_SelCheckPage->lcdinput[1]<<8) + pbuf[8];
			
			/*重启*/
			if(S_SelCheckPage->lcdinput[1] == 0x0001)
				while(1);
			/*忽略*/
			else if(S_SelCheckPage->lcdinput[1] == 0x0002)
			{
				SetGB_SelfCheckStatus(SelfCheck_OK);
				GotoGBChildPage(NULL);
			}
		}
	}
}

static void PageUpDate(void)
{
	RefreshPageValue();
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(51);
	
	/*清空当前页面*/
	PageClear();
	
	/*提示开始初始化*/
	AddNumOfSongToList(46, 0);
	
	/*开始初始化任务*/
	SetGB_SelfCheckStatus(SelfChecking);
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(NULL == S_SelCheckPage)
	{
		S_SelCheckPage = MyMalloc(sizeof(SelCheckPage));
		if(NULL == S_SelCheckPage)
			return My_Fail;
	}
	
	memset(S_SelCheckPage, 0, sizeof(SelCheckPage));
	
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_SelCheckPage);
	S_SelCheckPage = NULL;
	
	return My_Pass;
}


/********************************************************************************************************/
/********************************************************************************************************/
/********************************************************************************************************/
/********************************************************************************************************/

static void PageClear(void)
{
	/*清除上次的文字*/
	ClearText(0x1a00, 60);
	
	/*更新进度条*/
	BasicPic(0x1a20, 0, 100, 19, 295, 19+4*selfcheckpicinfo[0],302, 316, 357);
}

static void RefreshPageValue(void)
{
	unsigned char recvdat = 0;
	if(pdPASS == GetSelfCheckStatus(&recvdat, 10*portTICK_RATE_MS))
	{		
		/*清除上次的文字*/
		ClearText(0x1a00, 60);
		
		/*更新文本*/
		DisText(0x1a00, (void *)selfchecktextinfo[recvdat], strlen(selfchecktextinfo[recvdat]));				
		
		/*更新进度条*/
		BasicPic(0x1a20, 1, 100, 19, 295, 19+4*selfcheckpicinfo[recvdat],302, 316, 357);
		
		/*有不可忽略的错误*/
		if((recvdat == DataBasesError)||(recvdat == ErWeiMaError)||(recvdat == ADDAError)||(recvdat == MotorError))
			SetGB_SelfCheckStatus(SelfCheck_Error);
		
		if(recvdat == SelfCheckOver)
		{
			vTaskDelay(1000 / portTICK_RATE_MS);
			if(GetGB_SelfCheckStatus() == SelfCheck_Error)
			{
				SendKeyCode(1);
				AddNumOfSongToList(49, 0);
			}
/*			else if(GetGB_SelfCheckResult() == SelfCheck_Alam)
			{
				SendKeyCode(2);
				AddNumOfSongToList(48, 0);
			}*/
			else
			{
				SetGB_SelfCheckStatus(SelfCheck_OK);
				DspLunchPage(NULL);
				AddNumOfSongToList(50, 0);
			}
		}
	}
}

