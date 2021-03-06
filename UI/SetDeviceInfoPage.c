/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SetDeviceInfoPage.h"

#include	"LCD_Driver.h"
#include	"MyMem.h"
#include	"ShowDeviceInfoPage.h"
#include	"System_Data.h"
#include	"SystemSet_Dao.h"
#include	"SleepPage.h"

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
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);

static void showDeviceInfoText(void);
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
MyState_TypeDef createSetDeviceInfoActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "SetDeviceInfoActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_SetDeviceInfoPageBuffer)
	{
		copyGBSystemSetData(&(S_SetDeviceInfoPageBuffer->systemSetData));
		
		memcpy(&(S_SetDeviceInfoPageBuffer->user), &(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser), sizeof(User_Type));
		
		memcpy(S_SetDeviceInfoPageBuffer->deviceunit, S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceunit, MaxDeviceUnitLen);
		
		showDeviceInfoText();
	}
	
	SelectPage(102);
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
	if(S_SetDeviceInfoPageBuffer)
	{
		/*命令*/
		S_SetDeviceInfoPageBuffer->lcdinput[0] = pbuf[4];
		S_SetDeviceInfoPageBuffer->lcdinput[0] = (S_SetDeviceInfoPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B00)
		{
			backToFatherActivity();
		}
		/*确认*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B01)
		{			
			if(S_SetDeviceInfoPageBuffer->ismodify == 1)
			{
				copyGBSystemSetData(&(S_SetDeviceInfoPageBuffer->systemSetData));
		
				memcpy(&(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser), &(S_SetDeviceInfoPageBuffer->user), sizeof(User_Type));
		
				memcpy(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceunit, S_SetDeviceInfoPageBuffer->deviceunit, MaxDeviceUnitLen);
				
				S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.isnew = true;
				if(My_Pass == SaveSystemSetData(&(S_SetDeviceInfoPageBuffer->systemSetData)))
				{
					SendKeyCode(1);
					S_SetDeviceInfoPageBuffer->ismodify = 0;
				}
				else
					SendKeyCode(2);
			}
		}
		/*姓名*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B10)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_name, 0, MaxNameLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_name, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*年龄*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B20)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_age, 0, MaxAgeLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_age, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*性别*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B30)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_sex, 0, MaxSexLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_sex, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*联系方式*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B40)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_phone, 0, MaxPhoneLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_phone, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*职位*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B50)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_job, 0, MaxJobLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_job, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*备注*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B60)
		{
			memset(S_SetDeviceInfoPageBuffer->user.user_desc, 0, MaxDescLen);
			memcpy(S_SetDeviceInfoPageBuffer->user.user_desc, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*设备使用地址*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B70)
		{
			memset(S_SetDeviceInfoPageBuffer->deviceunit, 0, MaxDeviceUnitLen);
			memcpy(S_SetDeviceInfoPageBuffer->deviceunit, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
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
	if(S_SetDeviceInfoPageBuffer)
	{

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
	SelectPage(102);
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
	if(NULL == S_SetDeviceInfoPageBuffer)
	{
		S_SetDeviceInfoPageBuffer = MyMalloc(sizeof(SetDeviceInfoPageBuffer));
		
		if(S_SetDeviceInfoPageBuffer)
		{
			memset(S_SetDeviceInfoPageBuffer, 0, sizeof(SetDeviceInfoPageBuffer));
	
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
	MyFree(S_SetDeviceInfoPageBuffer);
	S_SetDeviceInfoPageBuffer = NULL;
}

static void showDeviceInfoText(void)
{
	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_name);
	DisText(0x1b10, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);

	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_age);
	DisText(0x1b20, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);

	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_sex);
	DisText(0x1b30, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);

	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_phone);
	DisText(0x1b40, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);
	
	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_job);
	DisText(0x1b50, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);

	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->user.user_desc);
	DisText(0x1b60, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);
	
	sprintf(S_SetDeviceInfoPageBuffer->tempBuf, "%s\0", S_SetDeviceInfoPageBuffer->deviceunit);
	DisText(0x1b70, S_SetDeviceInfoPageBuffer->tempBuf, strlen(S_SetDeviceInfoPageBuffer->tempBuf)+1);
}
