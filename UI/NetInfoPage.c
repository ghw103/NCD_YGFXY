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

static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);

static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void ReadNetInfo(void);
static void ShowNetInfo(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspNetInfoPage(void *  parm)
{
	PageInfo * currentpage = NULL;
	
	if(My_Pass == GetCurrentPage(&currentpage))
	{
		currentpage->PageInit = PageInit;
		currentpage->PageUpDate = PageUpDate;
		currentpage->LCDInput = Input;
		currentpage->PageBufferMalloc = PageBufferMalloc;
		currentpage->PageBufferFree = PageBufferFree;
		currentpage->tempP = &S_NetInfoPageBuffer;
		
		currentpage->PageInit(currentpage->pram);
	}
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_NetInfoPageBuffer)
	{
		/*命令*/
		S_NetInfoPageBuffer->lcdinput[0] = pbuf[4];
		S_NetInfoPageBuffer->lcdinput[0] = (S_NetInfoPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		//返回
		if(S_NetInfoPageBuffer->lcdinput[0] == 0x1ca0)
		{
			PageBufferFree();
			PageBackTo(ParentPage);
		}
	}
}

static void PageUpDate(void)
{
	if(TimeOut == timer_expired(&(S_NetInfoPageBuffer->timer)))
	{
		ReadNetInfo();
		ShowNetInfo();
		timer_reset(&(S_NetInfoPageBuffer->timer));
	}
}


static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(145);
	
	timer_set(&(S_NetInfoPageBuffer->timer), 10);
	
	ReadNetInfo();
	ShowNetInfo();
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{
	if(S_NetInfoPageBuffer == NULL)
	{
		S_NetInfoPageBuffer = (NetInfoPageBuffer *)MyMalloc(sizeof(NetInfoPageBuffer));
			
		if(S_NetInfoPageBuffer)
		{
			memset(S_NetInfoPageBuffer, 0, sizeof(NetInfoPageBuffer));
			return My_Pass;
		}
	}
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	if(S_NetInfoPageBuffer)
	{
		MyFree(S_NetInfoPageBuffer);
		S_NetInfoPageBuffer = NULL;
	}
	
	return My_Pass;
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
	GetWifiStaIP(&(S_NetInfoPageBuffer->netinfo.WifiIP));
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
	if(S_NetInfoPageBuffer->netinfo.LineStatus == 0)
		S_NetInfoPageBuffer->lineico.ICO_ID = 30;
	else
		S_NetInfoPageBuffer->lineico.ICO_ID = 31;
	S_NetInfoPageBuffer->lineico.X = 185;
	S_NetInfoPageBuffer->lineico.Y = 130;
	
	BasicUI(0x1CB0 ,0x1907 , 1, &(S_NetInfoPageBuffer->lineico) , sizeof(Basic_ICO));
	
	memset(S_NetInfoPageBuffer->tempbuffer1, 0, 100);
	sprintf(S_NetInfoPageBuffer->tempbuffer1, "%03d.%03d.%03d.%03d", S_NetInfoPageBuffer->netinfo.LineIP.ip_1, S_NetInfoPageBuffer->netinfo.LineIP.ip_2, 
		S_NetInfoPageBuffer->netinfo.LineIP.ip_3, S_NetInfoPageBuffer->netinfo.LineIP.ip_4);
	DisText(0x1CC0, S_NetInfoPageBuffer->tempbuffer1, strlen(S_NetInfoPageBuffer->tempbuffer1));
	
	memset(S_NetInfoPageBuffer->tempbuffer1, 0, 100);
	sprintf(S_NetInfoPageBuffer->tempbuffer1, "%02X-%02X-%02X-%02X-%02X-%02X", S_NetInfoPageBuffer->netinfo.LineMAC[0], S_NetInfoPageBuffer->netinfo.LineMAC[1], 
		S_NetInfoPageBuffer->netinfo.LineMAC[2], S_NetInfoPageBuffer->netinfo.LineMAC[3], S_NetInfoPageBuffer->netinfo.LineMAC[4], 
		S_NetInfoPageBuffer->netinfo.LineMAC[5]);
	DisText(0x1CD0, S_NetInfoPageBuffer->tempbuffer1, strlen(S_NetInfoPageBuffer->tempbuffer1));
	
	//显示wifi信息
	
}
