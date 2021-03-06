/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SetDeviceIDPage.h"

#include	"LCD_Driver.h"
#include	"System_Data.h"
#include	"MyMem.h"
#include	"ShowDeviceInfoPage.h"
#include	"SleepPage.h"
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
static void dspDeviceId(char * idStr);

static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createSelectUserActivity
*Description: 创建选择操作人界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyState_TypeDef createSetDeviceIDActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "SetDeviceIDActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: activityStart
*Description: 显示主界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:32
***************************************************************************************************/
static void activityStart(void)
{
	if(S_SetDeviceIDPage)
	{
		copyGBSystemSetData(&(S_SetDeviceIDPage->systemSetData));
		
		while(ReadBarCodeFunction((char *)(S_SetDeviceIDPage->tempbuf), 100) > 0)
			;
		dspDeviceId(S_SetDeviceIDPage->systemSetData.deviceInfo.deviceid);
	}
	
	SelectPage(104);
}

/***************************************************************************************************
*FunctionName: activityInput
*Description: 界面输入
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:59
***************************************************************************************************/
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	if(S_SetDeviceIDPage)
	{
		/*命令*/
		S_SetDeviceIDPage->lcdinput[0] = pbuf[4];
		S_SetDeviceIDPage->lcdinput[0] = (S_SetDeviceIDPage->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_SetDeviceIDPage->lcdinput[0] == 0x1c00)
		{
			backToFatherActivity();
		}
		/*确认*/
		else if(S_SetDeviceIDPage->lcdinput[0] == 0x1C01)
		{
			if(S_SetDeviceIDPage->ismodify == 1)
			{
				//读取最新的系统参数
				copyGBSystemSetData(&(S_SetDeviceIDPage->systemSetData));
				
				//更新副本中的is
				memcpy(S_SetDeviceIDPage->systemSetData.deviceInfo.deviceid, S_SetDeviceIDPage->deviceId, MaxDeviceIDLen);
				S_SetDeviceIDPage->systemSetData.deviceInfo.isnew = true;
				if(My_Pass == SaveSystemSetData(&(S_SetDeviceIDPage->systemSetData)))
				{
					SendKeyCode(1);

					S_SetDeviceIDPage->ismodify = 0;
				}
				else
					SendKeyCode(2);
			}
		}
		/*id输入*/
		else if(S_SetDeviceIDPage->lcdinput[0] == 0x1C10)
		{
			memset(S_SetDeviceIDPage->deviceId, 0 , MaxDeviceIDLen);
			
			if(MaxDeviceIDLen >= GetBufLen(&pbuf[7] , 2*pbuf[6]))
				memcpy(S_SetDeviceIDPage->deviceId, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			else
				memcpy(S_SetDeviceIDPage->deviceId, &pbuf[7], MaxDeviceIDLen);
				
			S_SetDeviceIDPage->ismodify = 1;
		}
	}
}

/***************************************************************************************************
*FunctionName: activityFresh
*Description: 界面刷新
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:16
***************************************************************************************************/
static void activityFresh(void)
{
	//读取设备id条码
	if(ReadBarCodeFunction((char *)(S_SetDeviceIDPage->tempbuf), 100) > 0)
	{
		memcpy(S_SetDeviceIDPage->deviceId, S_SetDeviceIDPage->tempbuf, MaxDeviceIDLen);
			
		dspDeviceId(S_SetDeviceIDPage->deviceId);
			
		S_SetDeviceIDPage->ismodify = 1;
	}
}

/***************************************************************************************************
*FunctionName: activityHide
*Description: 隐藏界面时要做的事
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:40
***************************************************************************************************/
static void activityHide(void)
{

}

/***************************************************************************************************
*FunctionName: activityResume
*Description: 界面恢复显示时要做的事
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:58
***************************************************************************************************/
static void activityResume(void)
{
	SelectPage(104);
}

/***************************************************************************************************
*FunctionName: activityDestroy
*Description: 界面销毁
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:02:15
***************************************************************************************************/
static void activityDestroy(void)
{
	activityBufferFree();
}

/***************************************************************************************************
*FunctionName: activityBufferMalloc
*Description: 界面数据内存申请
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static MyState_TypeDef activityBufferMalloc(void)
{
	if(NULL == S_SetDeviceIDPage)
	{
		S_SetDeviceIDPage = MyMalloc(sizeof(SetDeviceIDPage));
		
		if(S_SetDeviceIDPage)
		{
			memset(S_SetDeviceIDPage, 0, sizeof(SetDeviceIDPage));
	
			return My_Pass;
		}
		else
			return My_Fail;
	}
	else
		return My_Pass;
}

/***************************************************************************************************
*FunctionName: activityBufferFree
*Description: 界面内存释放
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:03:10
***************************************************************************************************/
static void activityBufferFree(void)
{
	MyFree(S_SetDeviceIDPage);
	S_SetDeviceIDPage = NULL;
}


static void dspDeviceId(char * idStr)
{
	sprintf(S_SetDeviceIDPage->tempbuf, "%s\0", idStr);
	DisText(0x1C10, S_SetDeviceIDPage->tempbuf, strlen(S_SetDeviceIDPage->tempbuf)+1);
}
