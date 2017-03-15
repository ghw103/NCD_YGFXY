/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"OtherSetPage.h"
#include	"SystemSetPage.h"
#include	"SleepPage.h"

#include	"SystemSet_Dao.h"
#include	"LCD_Driver.h"
#include	"RTC_Driver.h"
#include	"Define.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static OtherSetPageBuffer *S_OtherSetPageBuffer = NULL;
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

static void SetGB_Time(char *buf, unsigned char len);
static void showPrintfIco(void);
static void showMuteIco(void);
static void showLcdLightNum(void);
static void showLcdSleepTime(void);
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
MyState_TypeDef createOtherSetActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "OtherSetActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_OtherSetPageBuffer)
	{
		copyGBSystemSetData(&(S_OtherSetPageBuffer->systemSetData));

		showPrintfIco();
		showMuteIco();
		showLcdLightNum();
		showLcdSleepTime();
	}

	SelectPage(122);
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
	if(S_OtherSetPageBuffer)
	{
		/*命令*/
		S_OtherSetPageBuffer->lcdinput[0] = pbuf[4];
		S_OtherSetPageBuffer->lcdinput[0] = (S_OtherSetPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*重启*/
		if(S_OtherSetPageBuffer->lcdinput[0] == 0x2400)
		{
			while(1);
		}
		/*返回*/
		else if(S_OtherSetPageBuffer->lcdinput[0] == 0x2401)
		{
			backToFatherActivity();
		}
		/*设置时间*/
		else if(S_OtherSetPageBuffer->lcdinput[0] == 0x2410)
		{
			SetGB_Time((char *)(&pbuf[7]), GetBufLen(&pbuf[7] , 2*pbuf[6]));
		}
		//自动打印设置
		else if(S_OtherSetPageBuffer->lcdinput[0] == 0x2402)
		{
			//如果当前是自动打印，则禁止，否则打开
			if(S_OtherSetPageBuffer->systemSetData.isAutoPrint)
				S_OtherSetPageBuffer->systemSetData.isAutoPrint = false;
			else
				S_OtherSetPageBuffer->systemSetData.isAutoPrint = true;
			
			if(My_Pass == SaveSystemSetData(&(S_OtherSetPageBuffer->systemSetData)))
			{
				SendKeyCode(1);
				//保存成功，更新内存中的数据
				setSystemSetData(&(S_OtherSetPageBuffer->systemSetData));
			}
			else
			{
				SendKeyCode(2);
				copyGBSystemSetData(&(S_OtherSetPageBuffer->systemSetData));
			}
			
			showPrintfIco();
		}
		//静音
		else if(S_OtherSetPageBuffer->lcdinput[0] == 0x2403)
		{
			//如果当前是自动打印，则禁止，否则打开
			if(S_OtherSetPageBuffer->systemSetData.isMute)
				S_OtherSetPageBuffer->systemSetData.isMute = false;
			else
				S_OtherSetPageBuffer->systemSetData.isMute = true;
			
			if(My_Pass == SaveSystemSetData(&(S_OtherSetPageBuffer->systemSetData)))
			{
				SendKeyCode(1);
				//保存成功，更新内存中的数据
				setSystemSetData(&(S_OtherSetPageBuffer->systemSetData));
			}
			else
			{
				SendKeyCode(2);
				copyGBSystemSetData(&(S_OtherSetPageBuffer->systemSetData));
			}
			
			showMuteIco();
		}
		//进入休眠时间
		else if((S_OtherSetPageBuffer->lcdinput[0] >= 0x2407) && (S_OtherSetPageBuffer->lcdinput[0] <= 0x240a))
		{
			S_OtherSetPageBuffer->tempvalue = S_OtherSetPageBuffer->lcdinput[0] - 0x2407;
			
			if(S_OtherSetPageBuffer->tempvalue == 0)
				S_OtherSetPageBuffer->systemSetData.ledSleepTime = 60;
			else if(S_OtherSetPageBuffer->tempvalue == 1)
				S_OtherSetPageBuffer->systemSetData.ledSleepTime = 180;
			else if(S_OtherSetPageBuffer->tempvalue == 2)
				S_OtherSetPageBuffer->systemSetData.ledSleepTime = 300;
			else
				S_OtherSetPageBuffer->systemSetData.ledSleepTime = 60000;
			
			if(My_Pass == SaveSystemSetData(&(S_OtherSetPageBuffer->systemSetData)))
			{
				SendKeyCode(1);
				//保存成功，更新内存中的数据
				setSystemSetData(&(S_OtherSetPageBuffer->systemSetData));
			}
			else
			{
				SendKeyCode(2);
				copyGBSystemSetData(&(S_OtherSetPageBuffer->systemSetData));
			}
			
			showLcdSleepTime();
		}	
		//进入屏幕亮度
		else if(S_OtherSetPageBuffer->lcdinput[0] == 0x240b)
		{	
			S_OtherSetPageBuffer->systemSetData.ledLightIntensity = pbuf[8];
	
			if(My_Pass == SaveSystemSetData(&(S_OtherSetPageBuffer->systemSetData)))
			{
				SetLEDLight(S_OtherSetPageBuffer->systemSetData.ledLightIntensity);

				//保存成功，更新内存中的数据
				setSystemSetData(&(S_OtherSetPageBuffer->systemSetData));
			}
			else
			{
				SendKeyCode(2);
				copyGBSystemSetData(&(S_OtherSetPageBuffer->systemSetData));
				
				showLcdLightNum();
			}
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
	if(S_OtherSetPageBuffer)
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
	if(S_OtherSetPageBuffer)
	{

	}
	
	SelectPage(122);
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
	if(NULL == S_OtherSetPageBuffer)
	{
		S_OtherSetPageBuffer = MyMalloc(sizeof(OtherSetPageBuffer));
		
		if(S_OtherSetPageBuffer)
		{
			memset(S_OtherSetPageBuffer, 0, sizeof(OtherSetPageBuffer));
	
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
	MyFree(S_OtherSetPageBuffer);
	S_OtherSetPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static void SetGB_Time(char *buf, unsigned char len)
{
	short temp = 0;
	
	if(S_OtherSetPageBuffer)
	{
		if(len < 14)
		{
			SendKeyCode(3);
			return;
		}
		
		memset(S_OtherSetPageBuffer->buf, 0, 50);
		memcpy(S_OtherSetPageBuffer->buf, buf, 4);
		temp = strtol(S_OtherSetPageBuffer->buf, NULL, 10);
		if((temp < 2000)||(temp > 2100))
		{
			SendKeyCode(3);
			return;
		}
		S_OtherSetPageBuffer->temptime.year = temp - 2000;
		
		memset(S_OtherSetPageBuffer->buf, 0, 50);
		memcpy(S_OtherSetPageBuffer->buf, buf+4, 2);
		temp = strtol(S_OtherSetPageBuffer->buf, NULL, 10);
		if((temp < 1)||(temp > 12))
		{
			SendKeyCode(3);
			return;
		}
		S_OtherSetPageBuffer->temptime.month = temp;
		
		memset(S_OtherSetPageBuffer->buf, 0, 50);
		memcpy(S_OtherSetPageBuffer->buf, buf+6, 2);
		temp = strtol(S_OtherSetPageBuffer->buf, NULL, 10);
		if((temp < 1)||(temp > 31))
		{
			SendKeyCode(3);
			return;
		}
		S_OtherSetPageBuffer->temptime.day = temp;
		
		memset(S_OtherSetPageBuffer->buf, 0, 50);
		memcpy(S_OtherSetPageBuffer->buf, buf+8, 2);
		temp = strtol(S_OtherSetPageBuffer->buf, NULL, 10);
		if((temp < 0)||(temp > 23))
		{
			SendKeyCode(3);
			return;
		}
		S_OtherSetPageBuffer->temptime.hour = temp;
		
		memset(S_OtherSetPageBuffer->buf, 0, 50);
		memcpy(S_OtherSetPageBuffer->buf, buf+10, 2);
		temp = strtol(S_OtherSetPageBuffer->buf, NULL, 10);
		if((temp < 0)||(temp > 59))
		{
			SendKeyCode(3);
			return;
		}
		S_OtherSetPageBuffer->temptime.min = temp;
		
		memset(S_OtherSetPageBuffer->buf, 0, 50);
		memcpy(S_OtherSetPageBuffer->buf, buf+12, 2);
		temp = strtol(S_OtherSetPageBuffer->buf, NULL, 10);
		if((temp < 0)||(temp > 59))
		{
			SendKeyCode(3);
			return;
		}
		S_OtherSetPageBuffer->temptime.sec = temp;
		
		if(My_Pass == RTC_SetTimeData(&(S_OtherSetPageBuffer->temptime)))
			/*修改成功*/
			SendKeyCode(1);
		else
			/*修改失败*/
			SendKeyCode(2);
	}
}

static void showPrintfIco(void)
{
	S_OtherSetPageBuffer->ico.ICO_ID = 23;
	S_OtherSetPageBuffer->ico.X = 176;
	S_OtherSetPageBuffer->ico.Y = 185;
			
	if(S_OtherSetPageBuffer->systemSetData.isAutoPrint)
		BasicUI(0x2440 ,0x1807 , 1, &(S_OtherSetPageBuffer->ico) , sizeof(Basic_ICO));
	else
		BasicUI(0x2440 ,0x1807 , 0, &(S_OtherSetPageBuffer->ico) , sizeof(Basic_ICO));
}


static void showMuteIco(void)
{
	S_OtherSetPageBuffer->ico.ICO_ID = 23;
	S_OtherSetPageBuffer->ico.X = 738;
	S_OtherSetPageBuffer->ico.Y = 185;
			
	if(S_OtherSetPageBuffer->systemSetData.isMute)
		BasicUI(0x2450 ,0x1807 , 1, &(S_OtherSetPageBuffer->ico) , sizeof(Basic_ICO));
	else
		BasicUI(0x2450 ,0x1807 , 0, &(S_OtherSetPageBuffer->ico) , sizeof(Basic_ICO));
}

static void showLcdLightNum(void)
{
	DspNum(0x240b, S_OtherSetPageBuffer->systemSetData.ledLightIntensity, 2);
}

static void showLcdSleepTime(void)
{
	S_OtherSetPageBuffer->ico.ICO_ID = 23;
	S_OtherSetPageBuffer->ico.X = 385;
	S_OtherSetPageBuffer->ico.Y = 312;

	if(S_OtherSetPageBuffer->systemSetData.ledSleepTime == 60)
		BasicUI(0x2420 ,0x1807 , 1, &(S_OtherSetPageBuffer->ico) , sizeof(Basic_ICO));
	else if(S_OtherSetPageBuffer->systemSetData.ledSleepTime == 180)
	{
		S_OtherSetPageBuffer->ico.X = 490;
		BasicUI(0x2420 ,0x1807 , 1, &(S_OtherSetPageBuffer->ico) , sizeof(Basic_ICO));
	}
	else if(S_OtherSetPageBuffer->systemSetData.ledSleepTime == 300)
	{
		S_OtherSetPageBuffer->ico.X = 595;
		BasicUI(0x2420 ,0x1807 , 1, &(S_OtherSetPageBuffer->ico) , sizeof(Basic_ICO));
	}
	else if(S_OtherSetPageBuffer->systemSetData.ledSleepTime == 60000)
	{
		S_OtherSetPageBuffer->ico.X = 700;
		BasicUI(0x2420 ,0x1807 , 1, &(S_OtherSetPageBuffer->ico) , sizeof(Basic_ICO));
	}
	else
		BasicUI(0x2420 ,0x1807 , 0, &(S_OtherSetPageBuffer->ico) , sizeof(Basic_ICO));
}

