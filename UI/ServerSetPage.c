/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"ServerSetPage.h"

#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"NetPreSetPage.h"
#include	"SDFunction.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static ServerPageBuffer * S_ServerPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);

static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void SetTempIP(unsigned char *buf, unsigned char len);
static void RefreshPageText(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspServerSetPage(void *  parm)
{
	SetGBSysPage(DspServerSetPage, DspNetPreSetPage, NULL, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	unsigned short *pdata = NULL;
	
	pdata = MyMalloc((len/2)*sizeof(unsigned short));
	if(pdata == NULL)
		return;
	
	/*命令*/
	pdata[0] = pbuf[4];
	pdata[0] = (pdata[0]<<8) + pbuf[5];
	
	/*获取服务器ip*/
	if(pdata[0] == 0x2e30)
	{
		if(S_ServerPageBuffer)
		{
			SetTempIP(&pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_ServerPageBuffer->ischanged = 1;
		}
	}
	/*确认修改*/
	else if(pdata[0] == 0x2e21)
	{
		if(S_ServerPageBuffer)
		{
			if(1 == S_ServerPageBuffer->ischanged)
			{
				if(My_Pass == SaveNetData(&(S_ServerPageBuffer->myNetData)))
				{
					SendKeyCode(1);
					S_ServerPageBuffer->ischanged = 0;
				}
				else
					SendKeyCode(2);
			}
		}
	}
	/*返回*/
	else if(pdata[0] == 0x2e20)
	{
		GBPageBufferFree();
		
		GotoGBParentPage(NULL);
	}

	MyFree(pdata);
}

static void PageUpDate(void)
{
	
}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(84);
	
	ReadNetData(&(S_ServerPageBuffer->myNetData));
	
	RefreshPageText();
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	S_ServerPageBuffer = (ServerPageBuffer *)MyMalloc(sizeof(ServerPageBuffer));
			
	if(S_ServerPageBuffer)
	{
		memset(S_ServerPageBuffer, 0, sizeof(ServerPageBuffer));
		
		return My_Pass;
	}
	else
		return My_Fail;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_ServerPageBuffer);
	S_ServerPageBuffer = NULL;
	
	return My_Pass;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void RefreshPageText(void)
{
	if(S_ServerPageBuffer)
	{
		/*更新ip*/
		memset(S_ServerPageBuffer->buf, 0, 100);
		sprintf(S_ServerPageBuffer->buf, "%03d.%03d.%03d.%03d", S_ServerPageBuffer->myNetData.serverip.ip_1, S_ServerPageBuffer->myNetData.serverip.ip_2, S_ServerPageBuffer->myNetData.serverip.ip_3, S_ServerPageBuffer->myNetData.serverip.ip_4);
		DisText(0x2e30,S_ServerPageBuffer->buf, strlen(S_ServerPageBuffer->buf));
	}
}
static void SetTempIP(unsigned char *buf, unsigned char len)
{
	unsigned short temp = 0;
	
	if(S_ServerPageBuffer)
	{
		if(len == 15)
		{
			memset(S_ServerPageBuffer->buf, 0, 100);
			memcpy(S_ServerPageBuffer->buf, buf, 3);
			temp = strtol(S_ServerPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			S_ServerPageBuffer->myNetData.serverip.ip_1 = temp;
			
			memset(S_ServerPageBuffer->buf, 0, 100);
			memcpy(S_ServerPageBuffer->buf, buf+4, 3);
			temp = strtol(S_ServerPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			S_ServerPageBuffer->myNetData.serverip.ip_2 = temp;
			
			memset(S_ServerPageBuffer->buf, 0, 100);
			memcpy(S_ServerPageBuffer->buf, buf+8, 3);
			temp = strtol(S_ServerPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			S_ServerPageBuffer->myNetData.serverip.ip_3 = temp;
			
			memset(S_ServerPageBuffer->buf, 0, 100);
			memcpy(S_ServerPageBuffer->buf, buf+12, 3);
			temp = strtol(S_ServerPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			S_ServerPageBuffer->myNetData.serverip.ip_4 = temp;
		}
		else
			SendKeyCode(3);
	}
}

