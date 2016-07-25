/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"ShowDeviceInfoPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"SetDeviceInfoPage.h"
#include	"SetDeviceIDPage.h"
#include	"SystemSetPage.h"
#include	"MyMem.h"
#include	"SDFunction.h"
#include	"MyTools.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static unsigned char presscount = 0;
const char *GB_Password	= "123456";		//密码,进入校准界面密码
static DeviceInfo *gb_deviceinfo = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void showDeviceInfo(void);
static void ClearPageText(void);

static void PageInit(void);
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpData(void);
static void PageBufferMalloc(void);
static void PageBufferFree(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspShowDeviceInfoPage(void *  parm)
{
	SysPage * myPage = GetSysPage();

	myPage->CurrentPage = DspShowDeviceInfoPage;
	myPage->LCDInput = Input;
	myPage->PageUpData = PageUpData;
	myPage->ParentPage = DspSystemSetPage;
	myPage->ChildPage = DspSetDeviceInfoPage;

	PageInit();
	
	SelectPage(70);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	unsigned short *pdata = NULL;
	SysPage * myPage = GetSysPage();
	
	pdata = MyMalloc((len/2)*sizeof(unsigned short));
	if(pdata == NULL)
		return;
	
	/*命令*/
	pdata[0] = pbuf[4];
	pdata[0] = (pdata[0]<<8) + pbuf[5];
	
	/*基本信息*/
	if(pdata[0] == 0x220a)
	{
		presscount = 0;
	}
	else if(pdata[0] == 0x220b)
	{
		presscount++;
	}
	else if(pdata[0] == 0x220c)
	{
		if(presscount > 20)
		{
			SendKeyCode(1);
		}
	}
	/*获取密码*/
	else if(pdata[0] == 0x2230)
	{
		if(gb_deviceinfo)
		{
			if(pdPASS == CheckStrIsSame(&pbuf[7] , (void *)GB_Password ,GetBufLen(&pbuf[7] , 2*pbuf[6])))
			{
				myPage->ChildPage = DspSetDeviceIDPage;
				myPage->ChildPage(gb_deviceinfo);
			}
			else
				SendKeyCode(2);
		}
	}
	/*返回*/
	else if(pdata[0] == 0x2207)
	{
		PageBufferFree();
		myPage->ParentPage(NULL);
	}
	/*修改*/
	else if(pdata[0] == 0x2208)
	{
		if(gb_deviceinfo)
		{
			myPage->ChildPage = DspSetDeviceInfoPage;
			myPage->ChildPage(gb_deviceinfo);
		}
	}
	
	MyFree(pdata);
}

static void PageUpData(void)
{

}

static void PageInit(void)
{
	PageBufferMalloc();
	
	ClearPageText();
	
	if(gb_deviceinfo)
	{
		ReadDeviceInfo(gb_deviceinfo);
		
		showDeviceInfo();
	}
}

static void PageBufferMalloc(void)
{
	if(gb_deviceinfo == NULL)
	{
		gb_deviceinfo = MyMalloc(sizeof(DeviceInfo));
		if(gb_deviceinfo)
			memset(gb_deviceinfo, 0, sizeof(DeviceInfo));
	}
}

static void PageBufferFree(void)
{
	if(gb_deviceinfo)
	{
		MyFree(gb_deviceinfo);
		gb_deviceinfo = NULL;
	}
}

static void showDeviceInfo(void)
{
	
	/*显示设备id*/
	DisText(0x2240, gb_deviceinfo->deviceid, strlen(gb_deviceinfo->deviceid));
	
	/*显示设备名称*/
	DisText(0x2250, gb_deviceinfo->devicename, strlen(gb_deviceinfo->devicename));
	
	/*显示使用单位*/
	DisText(0x2270, gb_deviceinfo->deviceunit, strlen(gb_deviceinfo->deviceunit));
	/*显示设备制造商*/
	DisText(0x2290, gb_deviceinfo->devicemaker, strlen(gb_deviceinfo->devicemaker));
	
	/*显示设备制造商联系方式*/
	DisText(0x22b0, gb_deviceinfo->devicemakerphone, strlen(gb_deviceinfo->devicemakerphone));
	
	/*显示责任人*/
	DisText(0x22c0, gb_deviceinfo->deviceuser.user_name, strlen(gb_deviceinfo->deviceuser.user_name));
	
}

static void ClearPageText(void)
{
	/*显示设备id*/
	ClearText(0x2240, 20);
	
	/*显示设备名称*/
	ClearText(0x2250, 50);
	
	/*显示设备制造商*/
	ClearText(0x2270, 50);
	
	/*显示设备制造商联系方式*/
	ClearText(0x2290, 20);
	
	/*显示使用单位*/
	ClearText(0x22b0, 50);
	
	/*显示责任人*/
	ClearText(0x22c0, 10);
}



