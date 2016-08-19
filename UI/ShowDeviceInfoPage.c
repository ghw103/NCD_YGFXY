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
static ShowDeviceInfoPageBuffer * S_ShowDeviceInfoPageBuffer = NULL;
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
	if(S_ShowDeviceInfoPageBuffer)
	{
		/*命令*/
		S_ShowDeviceInfoPageBuffer->lcdinput[0] = pbuf[4];
		S_ShowDeviceInfoPageBuffer->lcdinput[0] = (S_ShowDeviceInfoPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*基本信息*/
		if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x220a)
		{
			S_ShowDeviceInfoPageBuffer->presscount = 0;
		}
		else if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x220b)
		{
			S_ShowDeviceInfoPageBuffer->presscount++;
		}
		else if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x220c)
		{
			if(S_ShowDeviceInfoPageBuffer->presscount > 20)
			{
				SendKeyCode(1);
			}
		}
		/*获取密码*/
		else if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x2230)
		{
			if(pdPASS == CheckStrIsSame(&pbuf[7] , AdminPassWord ,GetBufLen(&pbuf[7] , 2*pbuf[6])))
			{
				SetGBChildPage(DspSetDeviceIDPage);
				GotoGBChildPage(&(S_ShowDeviceInfoPageBuffer->s_deviceinfo));
			}
			else
				SendKeyCode(2);
		}
		/*返回*/
		else if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x2207)
		{
			GBPageBufferFree();
			GotoGBParentPage(NULL);
		}
		/*修改*/
		else if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x2208)
		{
			SetGBChildPage(DspSetDeviceInfoPage);
			GotoGBChildPage(&(S_ShowDeviceInfoPageBuffer->s_deviceinfo));
		}
	}
}

static void PageUpDate(void)
{

}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(70);
	
	ClearPageText();
	
	ReadDeviceInfo(&(S_ShowDeviceInfoPageBuffer->s_deviceinfo));
		
	showDeviceInfo();
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(S_ShowDeviceInfoPageBuffer == NULL)
	{
		S_ShowDeviceInfoPageBuffer = MyMalloc(sizeof(ShowDeviceInfoPageBuffer));
		if(NULL == S_ShowDeviceInfoPageBuffer)
			return My_Fail;
	}
	
	memset(S_ShowDeviceInfoPageBuffer, 0, sizeof(ShowDeviceInfoPageBuffer));
	
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_ShowDeviceInfoPageBuffer);
	S_ShowDeviceInfoPageBuffer = NULL;
	
	return My_Pass;
}

static void showDeviceInfo(void)
{
	if(S_ShowDeviceInfoPageBuffer)
	{
		/*显示设备id*/
		DisText(0x2240, S_ShowDeviceInfoPageBuffer->s_deviceinfo.deviceid, strlen(S_ShowDeviceInfoPageBuffer->s_deviceinfo.deviceid));
		
		/*显示设备名称*/
		DisText(0x2250, S_ShowDeviceInfoPageBuffer->s_deviceinfo.devicename, strlen(S_ShowDeviceInfoPageBuffer->s_deviceinfo.devicename));
		
		/*显示使用单位*/
		DisText(0x2270, S_ShowDeviceInfoPageBuffer->s_deviceinfo.deviceunit, strlen(S_ShowDeviceInfoPageBuffer->s_deviceinfo.deviceunit));
		/*显示设备制造商*/
		DisText(0x2290, S_ShowDeviceInfoPageBuffer->s_deviceinfo.devicemaker, strlen(S_ShowDeviceInfoPageBuffer->s_deviceinfo.devicemaker));
		
		/*显示设备制造商联系方式*/
		DisText(0x22b0, S_ShowDeviceInfoPageBuffer->s_deviceinfo.devicemakerphone, strlen(S_ShowDeviceInfoPageBuffer->s_deviceinfo.devicemakerphone));
		
		/*显示责任人*/
		DisText(0x22c0, S_ShowDeviceInfoPageBuffer->s_deviceinfo.deviceuser.user_name, strlen(S_ShowDeviceInfoPageBuffer->s_deviceinfo.deviceuser.user_name));
	}
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



