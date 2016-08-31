/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SetDeviceIDPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"ShowDeviceInfoPage.h"
#include	"GetSampleIDFun.h"
#include	"SDFunction.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static SetDeviceIDPage * S_SetDeviceIDPage = NULL;
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

unsigned char DspSetDeviceIDPage(void *  parm)
{
	SetGBSysPage(DspSetDeviceIDPage, DspShowDeviceInfoPage, NULL, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_SetDeviceIDPage)
	{
		/*命令*/
		S_SetDeviceIDPage->lcdinput[0] = pbuf[4];
		S_SetDeviceIDPage->lcdinput[0] = (S_SetDeviceIDPage->lcdinput[0]<<8) + pbuf[5];
		

		/*修改设备id*/
		/*返回*/
		if(S_SetDeviceIDPage->lcdinput[0] == 0x2390)
		{
			GBPageBufferFree();
			GotoGBParentPage(NULL);
		}
		/*确认*/
		else if(S_SetDeviceIDPage->lcdinput[0] == 0x2391)
		{
			if(S_SetDeviceIDPage->ismodify == 1)
			{
				S_SetDeviceIDPage->temp_deviceinfo.isfresh = 1;
				if(My_Pass == SaveDeviceInfo(&(S_SetDeviceIDPage->temp_deviceinfo)))
					SendKeyCode(2);
				else
					SendKeyCode(1);
			}
		}
		/*id输入*/
		else if(S_SetDeviceIDPage->lcdinput[0] == 0x23a0)
		{
			memset(S_SetDeviceIDPage->temp_deviceinfo.deviceid, 0 , MaxDeviceIDLen);
			
			if(MaxDeviceIDLen >= GetBufLen(&pbuf[7] , 2*pbuf[6]))
				memcpy(S_SetDeviceIDPage->temp_deviceinfo.deviceid, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			else
				memcpy(S_SetDeviceIDPage->temp_deviceinfo.deviceid, &pbuf[7], MaxDeviceIDLen);
				
			S_SetDeviceIDPage->ismodify = 1;
		}
	}
}

static void PageUpDate(void)
{
	if((S_SetDeviceIDPage) && (My_Pass == TakeSampleIDData(&(S_SetDeviceIDPage->tempbuf))))
	{
		memcpy(S_SetDeviceIDPage->temp_deviceinfo.deviceid, S_SetDeviceIDPage->tempbuf, MaxDeviceIDLen);
		MyFree(S_SetDeviceIDPage->tempbuf);
		
		DisText(0x23a0, S_SetDeviceIDPage->temp_deviceinfo.deviceid, strlen(S_SetDeviceIDPage->temp_deviceinfo.deviceid));
		
		S_SetDeviceIDPage->ismodify = 1;
	}
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(74);
	
	if(parm)
	{
		memcpy(&(S_SetDeviceIDPage->temp_deviceinfo), parm, sizeof(DeviceInfo));
		DisText(0x23a0, S_SetDeviceIDPage->temp_deviceinfo.deviceid, strlen(S_SetDeviceIDPage->temp_deviceinfo.deviceid));
		return My_Pass;
	}
	
	return My_Fail;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(S_SetDeviceIDPage == NULL)
	{
		S_SetDeviceIDPage = (SetDeviceIDPage *)MyMalloc(sizeof(SetDeviceIDPage));
			
		if(NULL == S_SetDeviceIDPage)
			return My_Fail;
	}
	
	memset(S_SetDeviceIDPage, 0, sizeof(SetDeviceIDPage));
	
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_SetDeviceIDPage);
	S_SetDeviceIDPage = NULL;
	
	return My_Pass;
}

