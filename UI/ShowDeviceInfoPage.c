/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"ShowDeviceInfoPage.h"

#include	"SetDeviceInfoPage.h"
#include	"SetDeviceIDPage.h"
#include	"SystemSetPage.h"

#include	"LCD_Driver.h"

#include	"UI_Data.h"

#include	"MyMem.h"
#include	"SDFunction.h"
#include	"DeviceDao.h"
#include	"MyTools.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static ShowDeviceInfoPageBuffer * S_ShowDeviceInfoPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void showDeviceInfo(void);

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
	if(S_ShowDeviceInfoPageBuffer)
	{
		/*命令*/
		S_ShowDeviceInfoPageBuffer->lcdinput[0] = pbuf[4];
		S_ShowDeviceInfoPageBuffer->lcdinput[0] = (S_ShowDeviceInfoPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*基本信息*/
		if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x1a03)
			S_ShowDeviceInfoPageBuffer->presscount = 0;
		
		else if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x1a04)
			S_ShowDeviceInfoPageBuffer->presscount++;
		
		else if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x1a05)
		{
			if(S_ShowDeviceInfoPageBuffer->presscount > 15)
				SendKeyCode(2);
		}
		/*获取密码*/
		else if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x1a10)
		{
			if(pdPASS == CheckStrIsSame(&pbuf[7] , AdminPassWord ,GetBufLen(&pbuf[7] , 2*pbuf[6])))
			{
				PageAdvanceTo(DspSetDeviceIDPage, &(S_ShowDeviceInfoPageBuffer->s_deviceinfo));
			}
			else
				SendKeyCode(1);
		}
		/*返回*/
		else if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x1a00)
		{
			PageBufferFree();
			PageBackTo(ParentPage);
		}
		/*修改*/
		else if(S_ShowDeviceInfoPageBuffer->lcdinput[0] == 0x1a01)
		{
			PageAdvanceTo(DspSetDeviceInfoPage, &(S_ShowDeviceInfoPageBuffer->s_deviceinfo));
		}
	}
}

static void PageUpDate(void)
{

}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Pass == PageBufferMalloc())
	{
		ReadDeviceInfo(&(S_ShowDeviceInfoPageBuffer->s_deviceinfo));
		
		showDeviceInfo();
	}
	
	SelectPage(100);

	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(S_ShowDeviceInfoPageBuffer == NULL)
	{
		S_ShowDeviceInfoPageBuffer = MyMalloc(sizeof(ShowDeviceInfoPageBuffer));
		if(S_ShowDeviceInfoPageBuffer)
		{
			memset(S_ShowDeviceInfoPageBuffer, 0, sizeof(ShowDeviceInfoPageBuffer));
		
			return My_Pass;
		}
	}

	return My_Fail;
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
		DisText(0x1a40, S_ShowDeviceInfoPageBuffer->s_deviceinfo.deviceid, MaxDeviceIDLen);
			
		/*显示使用单位*/
		DisText(0x1a60, S_ShowDeviceInfoPageBuffer->s_deviceinfo.deviceunit, MaxDeviceUnitLen);

		/*显示责任人*/
		DisText(0x1a90, S_ShowDeviceInfoPageBuffer->s_deviceinfo.deviceuser.user_name, MaxNameLen);
	}
}

