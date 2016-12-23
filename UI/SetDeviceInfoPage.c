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
		getSystemSetData(&(S_SetDeviceInfoPageBuffer->systemSetData));
		
		timer_set(&(S_SetDeviceInfoPageBuffer->timer), S_SetDeviceInfoPageBuffer->systemSetData.ledSleepTime);
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
		
		//重置休眠时间
		timer_restart(&(S_SetDeviceInfoPageBuffer->timer));
		
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
				S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.isnew = true;
				if(My_Pass == SaveSystemSetData(&(S_SetDeviceInfoPageBuffer->systemSetData)))
				{
					SendKeyCode(1);
					//保存成功，更新内存中的数据
					setSystemSetData(&(S_SetDeviceInfoPageBuffer->systemSetData));
					S_SetDeviceInfoPageBuffer->ismodify = 0;
				}
				else
					SendKeyCode(2);
			}
		}
		/*姓名*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B10)
		{
			memset(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_name, 0, MaxNameLen);
			memcpy(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_name, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*年龄*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B20)
		{
			memset(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_age, 0, MaxAgeLen);
			memcpy(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_age, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*性别*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B30)
		{
			memset(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_sex, 0, MaxSexLen);
			memcpy(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_sex, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*联系方式*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B40)
		{
			memset(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_phone, 0, MaxPhoneLen);
			memcpy(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_phone, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*职位*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B50)
		{
			memset(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_job, 0, MaxJobLen);
			memcpy(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_job, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*备注*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B60)
		{
			memset(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_desc, 0, MaxDescLen);
			memcpy(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceuser.user_desc, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_SetDeviceInfoPageBuffer->ismodify = 1;
		}
		/*设备使用地址*/
		else if(S_SetDeviceInfoPageBuffer->lcdinput[0] == 0x1B70)
		{
			memset(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceunit, 0, MaxDeviceUnitLen);
			memcpy(S_SetDeviceInfoPageBuffer->systemSetData.deviceInfo.deviceunit, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
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
		//休眠
		if(TimeOut == timer_expired(&(S_SetDeviceInfoPageBuffer->timer)))
			startActivity(createSleepActivity, NULL);
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
	if(S_SetDeviceInfoPageBuffer)
	{
		timer_restart(&(S_SetDeviceInfoPageBuffer->timer));
	}
	
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


