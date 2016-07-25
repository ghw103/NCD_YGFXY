/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"LunchPage.h"

#include	"LCD_Driver.h"

#include	"Define.h"
#include	"MyMem.h"

#include	"SystemSetPage.h"
#include	"SelectUserPage.h"
#include	"PaiDuiPage.h"
#include	"WifiFunction.h"
#include	"Net_Data.h"
#include	"PlaySong_Task.h"
#include	"UI_Data.h"
#include	"MyTest_Data.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static unsigned char presscount = 0;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpData(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspLunchPage(void *  parm)
{
	SysPage * myPage = GetSysPage();

	myPage->CurrentPage = DspLunchPage;
	myPage->LCDInput = Input;
	myPage->PageUpData = PageUpData;
	myPage->ParentPage = NULL;
	myPage->ChildPage = NULL;
	
	SelectPage(52);
	
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
	if(pdata[0] == 0x1b04)
	{
		myPage->ChildPage = DspSystemSetPage;
		myPage->ChildPage(NULL);
	}
	/*常规测试*/
	else if(pdata[0] == 0x1b00)
	{	
		error = CreateANewTest(0);
		/*创建成功*/
		if(0 == error)
		{
			DspSelectUserPage(NULL);
		}
		/*禁止常规测试*/
		else if(1 == error)
		{
			SendKeyCode(2);
			AddNumOfSongToList(45, 0);
		}
		/*创建失败*/
		else if(3 == error)
		{
			SendKeyCode(3);
			AddNumOfSongToList(41, 0);
		}
	}
	else if(pdata[0] == 0x1b01)
	{
		presscount = 0;
	}
	else if(pdata[0] == 0x1b02)
	{
		presscount++;
	}
	else if(pdata[0] == 0x1b03)
	{
		/*查看排队状态*/
		if(presscount > 20)
		{
			DspPaiDuiPage(NULL);
		}
		/*批量测试*/
		else
		{
			error = CreateANewTest(111);
			/*创建成功*/
			if(0 == error)
			{
				DspSelectUserPage(NULL);
			}
			/*排队位置满，不允许*/
			else if(1 == error)
			{
				SendKeyCode(1);
				AddNumOfSongToList(40, 0);
			}
			/*创建失败*/
			else if(2 == error)
			{
				SendKeyCode(3);
				AddNumOfSongToList(41, 0);
			}
			/*有卡即将测试*/
			else if(3 == error)
			{
				SendKeyCode(4);
				AddNumOfSongToList(61, 0);
			}
		}
	}
	
	MyFree(pdata);
}

static void PageUpData(void)
{

}





