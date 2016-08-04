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

static MyState_TypeDef PageInit(void *  parm);
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspShowDeviceInfoPage(void *  parm)
{
	SetGBSysPage(DspShowDeviceInfoPage, DspSystemSetPage, DspSetDeviceInfoPage, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);
	
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
				SetGBChildPage(DspSetDeviceIDPage);
				GotoGBChildPage(gb_deviceinfo);
			}
			else
				SendKeyCode(2);
		}
	}
	/*返回*/
	else if(pdata[0] == 0x2207)
	{
		GBPageBufferFree();
		GotoGBParentPage(NULL);
	}
	/*修改*/
	else if(pdata[0] == 0x2208)
	{
		if(gb_deviceinfo)
		{
			SetGBChildPage(DspSetDeviceInfoPage);
			GotoGBChildPage(gb_deviceinfo);
		}
	}
	
	MyFree(pdata);
}

static void PageUpDate(void)
{

}

static MyState_TypeDef PageInit(void *  parm)
{
	PageBufferMalloc();
	
	SelectPage(70);
	
	ClearPageText();
	
	if(gb_deviceinfo)
	{
		ReadDeviceInfo(gb_deviceinfo);
		
		showDeviceInfo();
	}
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(gb_deviceinfo == NULL)
	{
		gb_deviceinfo = MyMalloc(sizeof(DeviceInfo));
		if(gb_deviceinfo)
			memset(gb_deviceinfo, 0, sizeof(DeviceInfo));
		else
			return My_Fail;
	}
	
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	if(gb_deviceinfo)
	{
		MyFree(gb_deviceinfo);
		gb_deviceinfo = NULL;
	}
	
	return My_Pass;
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



