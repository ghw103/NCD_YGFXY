/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"NetInfoPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"SDFunction.h"
#include	"NetPreSetPage.h"
#include	"MyMem.h"
#include	"WifiFunction.h"
#include	"SleepPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static NetInfoPageBuffer *S_NetInfoPageBuffer = NULL;
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

static void ReadNetInfo(void);
static void ShowNetInfo(void);
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
MyState_TypeDef createNetInfoActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "NetInfoActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_NetInfoPageBuffer)
	{		
		timer_set(&(S_NetInfoPageBuffer->timer), 10);
	
		SelectPage(145);
		
		ReadNetInfo();
		
		ShowNetInfo();	
	}

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
	if(S_NetInfoPageBuffer)
	{
		/*命令*/
		S_NetInfoPageBuffer->lcdinput[0] = pbuf[4];
		S_NetInfoPageBuffer->lcdinput[0] = (S_NetInfoPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		//返回
		if(S_NetInfoPageBuffer->lcdinput[0] == 0x1ca0)
		{
			RestartWifi();
			
			backToFatherActivity();
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
	if(S_NetInfoPageBuffer)
	{
		if(TimeOut == timer_expired(&(S_NetInfoPageBuffer->timer)))
		{
			ReadNetInfo();
			ShowNetInfo();
			timer_restart(&(S_NetInfoPageBuffer->timer));
		}

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
	if(S_NetInfoPageBuffer)
	{
		timer_restart(&(S_NetInfoPageBuffer->timer));
	}
	
	SelectPage(145);
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
	
	giveWifixMutex();
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
	if(NULL == S_NetInfoPageBuffer)
	{
		S_NetInfoPageBuffer = MyMalloc(sizeof(NetInfoPageBuffer));
		
		if(S_NetInfoPageBuffer)
		{
			memset(S_NetInfoPageBuffer, 0, sizeof(NetInfoPageBuffer));
	
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
	MyFree(S_NetInfoPageBuffer);
	S_NetInfoPageBuffer = NULL;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: ReadNetInfo
*Description: 读取网络信息
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月5日15:38:30
***************************************************************************************************/
static void ReadNetInfo(void)
{
	S_NetInfoPageBuffer->wifiico.ICO_ID = 32;
	S_NetInfoPageBuffer->wifiico.X = 189;
	S_NetInfoPageBuffer->wifiico.Y = 315;
	
	memset(&(S_NetInfoPageBuffer->WifiIP), 0, sizeof(IP_Def));
	memset(S_NetInfoPageBuffer->WifiSSID, 0, 30);
		
	memset(S_NetInfoPageBuffer->WifiMAC, 0, 13);
	
	if(S_NetInfoPageBuffer->isGetWifiControl == false)
	{
		if(My_Fail == takeWifiMutex(100 / portTICK_RATE_MS))
		{
			goto END;
		}
		else
			S_NetInfoPageBuffer->isGetWifiControl = true;
	}
	/*如果不是at模式，则进入at模式*/
	SetWifiWorkInAT(AT_Mode);
	
	if(My_Pass == WifiIsConnectted(S_NetInfoPageBuffer->WifiSSID))
	{
		//读取IP
		GetWifiStaIP(&(S_NetInfoPageBuffer->WifiIP));
		
		//读取mac
		GetWifiStaMac(S_NetInfoPageBuffer->WifiMAC);
		
		//读取信号强度
		S_NetInfoPageBuffer->WifiIndicator = GetWifiIndicator();
		
		if(S_NetInfoPageBuffer->WifiIndicator <= 10)
			S_NetInfoPageBuffer->wifiico.ICO_ID = 36;
		else if(S_NetInfoPageBuffer->WifiIndicator < 40)
			S_NetInfoPageBuffer->wifiico.ICO_ID = 35;
		else if(S_NetInfoPageBuffer->WifiIndicator < 70)
			S_NetInfoPageBuffer->wifiico.ICO_ID = 34;
		else
			S_NetInfoPageBuffer->wifiico.ICO_ID = 33;
		
		//显示wifi状态图标
		BasicUI(0x1CB8 ,0x1807 , 1, &(S_NetInfoPageBuffer->wifiico) , sizeof(Basic_ICO));
		//显示ssid
		memset(S_NetInfoPageBuffer->tempbuffer1, 0, 100);
		sprintf(S_NetInfoPageBuffer->tempbuffer1, "%s", S_NetInfoPageBuffer->WifiSSID);
		DisText(0x1Cf0, S_NetInfoPageBuffer->tempbuffer1, 30);
		
		//显示ip
		sprintf(S_NetInfoPageBuffer->tempbuffer1, "%03d.%03d.%03d.%03d\0", S_NetInfoPageBuffer->WifiIP.ip_1, S_NetInfoPageBuffer->WifiIP.ip_2, 
			S_NetInfoPageBuffer->WifiIP.ip_3, S_NetInfoPageBuffer->WifiIP.ip_4);
		DisText(0x1CC8, S_NetInfoPageBuffer->tempbuffer1, 15);
		//显示mac
		sprintf(S_NetInfoPageBuffer->tempbuffer1, "%.2s-%.2s-%.2s-%.2s-%.2s-%.2s\0", S_NetInfoPageBuffer->WifiMAC, &(S_NetInfoPageBuffer->WifiMAC[2]), &(S_NetInfoPageBuffer->WifiMAC[4]),
			&(S_NetInfoPageBuffer->WifiMAC[6]), &(S_NetInfoPageBuffer->WifiMAC[8]), &(S_NetInfoPageBuffer->WifiMAC[10]));
		DisText(0x1Ce0, S_NetInfoPageBuffer->tempbuffer1, 20);
		
		return;
	}
	
	END:
		//显示wifi状态图标
		BasicUI(0x1CB8 ,0x1807 , 0, &(S_NetInfoPageBuffer->wifiico) , sizeof(Basic_ICO));
		//显示ssid
		sprintf(S_NetInfoPageBuffer->tempbuffer1, "Disconnected\0");
		DisText(0x1Cf0, S_NetInfoPageBuffer->tempbuffer1, strlen(S_NetInfoPageBuffer->tempbuffer1)+1);
			
		//显示ip
		sprintf(S_NetInfoPageBuffer->tempbuffer1, "000.000.000.000\0");
		DisText(0x1CC8, S_NetInfoPageBuffer->tempbuffer1, 15);
		//显示mac
		sprintf(S_NetInfoPageBuffer->tempbuffer1, "00-00-00-00-00-00\0");
		DisText(0x1Ce0, S_NetInfoPageBuffer->tempbuffer1, 20);
}

/***************************************************************************************************
*FunctionName: ShowNetInfo
*Description: 显示网络信息
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月5日15:38:44
***************************************************************************************************/
static void ShowNetInfo(void)
{
	GetGB_NetInfo(&(S_NetInfoPageBuffer->netinfo));
	
	//显示有线网信息
	
	S_NetInfoPageBuffer->lineico.ICO_ID = 31;
	S_NetInfoPageBuffer->lineico.X = 189;
	S_NetInfoPageBuffer->lineico.Y = 130;
	
	if(S_NetInfoPageBuffer->netinfo.LineStatus == 0)
		BasicUI(0x1CB0 ,0x1807 , 0, &(S_NetInfoPageBuffer->lineico) , sizeof(Basic_ICO));
	else
		BasicUI(0x1CB0 ,0x1807 , 1, &(S_NetInfoPageBuffer->lineico) , sizeof(Basic_ICO));
	
	sprintf(S_NetInfoPageBuffer->tempbuffer1, "%03d.%03d.%03d.%03d", S_NetInfoPageBuffer->netinfo.LineIP.ip_1, S_NetInfoPageBuffer->netinfo.LineIP.ip_2, 
		S_NetInfoPageBuffer->netinfo.LineIP.ip_3, S_NetInfoPageBuffer->netinfo.LineIP.ip_4);
	DisText(0x1CC0, S_NetInfoPageBuffer->tempbuffer1, 15);
	
	sprintf(S_NetInfoPageBuffer->tempbuffer1, "%02X-%02X-%02X-%02X-%02X-%02X\0", S_NetInfoPageBuffer->netinfo.LineMAC[0], S_NetInfoPageBuffer->netinfo.LineMAC[1], 
		S_NetInfoPageBuffer->netinfo.LineMAC[2], S_NetInfoPageBuffer->netinfo.LineMAC[3], S_NetInfoPageBuffer->netinfo.LineMAC[4], 
		S_NetInfoPageBuffer->netinfo.LineMAC[5]);
	DisText(0x1CD0, S_NetInfoPageBuffer->tempbuffer1, strlen(S_NetInfoPageBuffer->tempbuffer1)+1);
	
	//显示wifi信息
	
}
