/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SetDeviceInfoPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"ShowDeviceInfoPage.h"
#include	"SDFunction.h"
#include	"System_Data.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static SetDeviceInfoPageBuffer * S_SetDeviceInfoPageBuffer = NULL;
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

unsigned char DspSetDeviceInfoPage(void *  parm)
{
	SetGBSysPage(DspSetDeviceInfoPage, DspShowDeviceInfoPage, NULL, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_SetDeviceInfoPageBuffer)
	{
		/*命令*/
		S_SetDeviceInfoPageBuffer->lcdinput[0] = pbuf[4];
		S_SetDeviceInfoPageBuffer->lcdinput[0] = (S_SetDeviceInfoPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x2300)
		{
			GBPageBufferFree();
			GotoGBParentPage(NULL);
		}
		/*确认*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x2301)
		{
			if(S_SetDeviceInfoPageBuffer->ismodify == 1)
			{
				if(My_Pass == SaveDeviceInfo(&(S_SetDeviceInfoPageBuffer->temp_deviceinfo)))
				{
					SendKeyCode(2);
					SetGB_DeviceInfo(&(S_SetDeviceInfoPageBuffer->temp_deviceinfo));
					SetDeviceInIsFresh(true);
				}
				else
					SendKeyCode(1);
			}
		}
		/*姓名*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x2310)
		{
			memset(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_name, 0, MaxNameLen);
			memcpy(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_name, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*年龄*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x2318)
		{
			memset(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_age, 0, MaxAgeLen);
			memcpy(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_age, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*性别*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x231b)
		{
			memset(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_sex, 0, MaxSexLen);
			memcpy(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_sex, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*联系方式*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x2320)
		{
			memset(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_phone, 0, MaxPhoneLen);
			memcpy(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_phone, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*职位*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x2330)
		{
			memset(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_job, 0, MaxJobLen);
			memcpy(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_job, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*备注*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x2350)
		{
			memset(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_desc, 0, MaxDescLen);
			memcpy(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceuser.user_desc, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*设备使用地址*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x2370)
		{
			memset(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceunit, 0, MaxDeviceUnitLen);
			memcpy(S_SetDeviceInfoPageBuffer->temp_deviceinfo.deviceunit, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
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
	
	SelectPage(72);
	
	if(parm)
	{
		memcpy(&(S_SetDeviceInfoPageBuffer->temp_deviceinfo), parm, sizeof(DeviceInfo));
		return My_Pass;
	}
	
	return My_Fail;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(S_SetDeviceInfoPageBuffer == NULL)
	{
		S_SetDeviceInfoPageBuffer = MyMalloc(sizeof(DeviceInfo));
		if(NULL == S_SetDeviceInfoPageBuffer)
			return My_Fail;
	}

	memset(S_SetDeviceInfoPageBuffer, 0, sizeof(DeviceInfo));
	
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	if(S_SetDeviceInfoPageBuffer)
	{
		MyFree(S_SetDeviceInfoPageBuffer);
		S_SetDeviceInfoPageBuffer = NULL;
	}
	
	return My_Pass;
}


