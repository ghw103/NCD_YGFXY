/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SetDeviceIDPage.h"

#include	"LCD_Driver.h"
#include	"System_Data.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"ShowDeviceInfoPage.h"
#include	"ReadBarCode_Fun.h"
#include	"SystemSet_Dao.h"

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
	if(S_SetDeviceIDPage)
	{
		/*命令*/
		S_SetDeviceIDPage->lcdinput[0] = pbuf[4];
		S_SetDeviceIDPage->lcdinput[0] = (S_SetDeviceIDPage->lcdinput[0]<<8) + pbuf[5];
		

		/*修改设备id*/
		/*返回*/
		if(S_SetDeviceIDPage->lcdinput[0] == 0x1c00)
		{
			PageBufferFree();
			PageBackTo(ParentPage);
		}
		/*确认*/
		else if(S_SetDeviceIDPage->lcdinput[0] == 0x1C01)
		{
			if(S_SetDeviceIDPage->ismodify == 1)
			{
				S_SetDeviceIDPage->systemSetData.deviceInfo.isnew = true;
				if(My_Pass == SaveSystemSetData(&(S_SetDeviceIDPage->systemSetData)))
				{
					SendKeyCode(1);
					//保存成功，更新内存中的数据
					setSystemSetData(&(S_SetDeviceIDPage->systemSetData));
					S_SetDeviceIDPage->ismodify = 0;
				}
				else
					SendKeyCode(2);
			}
		}
		/*id输入*/
		else if(S_SetDeviceIDPage->lcdinput[0] == 0x1C10)
		{
			memset(S_SetDeviceIDPage->systemSetData.deviceInfo.deviceid, 0 , MaxDeviceIDLen);
			
			if(MaxDeviceIDLen >= GetBufLen(&pbuf[7] , 2*pbuf[6]))
				memcpy(S_SetDeviceIDPage->systemSetData.deviceInfo.deviceid, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			else
				memcpy(S_SetDeviceIDPage->systemSetData.deviceInfo.deviceid, &pbuf[7], MaxDeviceIDLen);
				
			S_SetDeviceIDPage->ismodify = 1;
		}
	}
}

static void PageUpDate(void)
{
	if(S_SetDeviceIDPage)
	{
		if(ReadBarCodeFunction((char *)(S_SetDeviceIDPage->tempbuf), 100) > 0)
		{
			memcpy(S_SetDeviceIDPage->systemSetData.deviceInfo.deviceid, S_SetDeviceIDPage->tempbuf, MaxDeviceIDLen);
			
			DisText(0x1C10, S_SetDeviceIDPage->systemSetData.deviceInfo.deviceid, MaxDeviceIDLen);
		
			S_SetDeviceIDPage->ismodify = 1;
		}		
	}
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Pass == PageBufferMalloc())
	{
		getSystemSetData(&(S_SetDeviceIDPage->systemSetData));
	
		DisText(0x1C10, S_SetDeviceIDPage->systemSetData.deviceInfo.deviceid, strlen(S_SetDeviceIDPage->systemSetData.deviceInfo.deviceid));
	}
	
	SelectPage(104);
		
	return My_Fail;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(S_SetDeviceIDPage == NULL)
	{
		S_SetDeviceIDPage = (SetDeviceIDPage *)MyMalloc(sizeof(SetDeviceIDPage));
			
		if(S_SetDeviceIDPage)
		{
			memset(S_SetDeviceIDPage, 0, sizeof(SetDeviceIDPage));
	
			return My_Pass;
		}
		else
			return My_Fail;
	}
	
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_SetDeviceIDPage);
	S_SetDeviceIDPage = NULL;
	
	return My_Pass;
}

