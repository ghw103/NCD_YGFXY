/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"NetSetPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"SDFunction.h"
#include	"NetPreSetPage.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static NetSetPageBuffer *S_NetSetPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);

static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void UpPageValue(void);
static void SetTempIP(unsigned char *buf, unsigned char len);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspNetSetPage(void *  parm)
{
	SetGBSysPage(DspNetSetPage, DspNetPreSetPage, NULL, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_NetSetPageBuffer)
	{
		/*命令*/
		S_NetSetPageBuffer->lcdinput[0] = pbuf[4];
		S_NetSetPageBuffer->lcdinput[0] = (S_NetSetPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*有线网的ip获取模式*/
		if(S_NetSetPageBuffer->lcdinput[0] == 0x2d04)
		{
			/*数据*/
			S_NetSetPageBuffer->lcdinput[1] = pbuf[7];
			S_NetSetPageBuffer->lcdinput[1] = (S_NetSetPageBuffer->lcdinput[1]<<8) + pbuf[8];
			
			if(S_NetSetPageBuffer)
			{
				/*自动获取ip*/
				if(S_NetSetPageBuffer->lcdinput[1] == 0x8000)
					S_NetSetPageBuffer->myNetData.ipmode = DHCP_Mode;
				/*使用设置的ip*/
				else if(S_NetSetPageBuffer->lcdinput[1] == 0x0000)
					S_NetSetPageBuffer->myNetData.ipmode = User_Mode;
				
				S_NetSetPageBuffer->ischanged = 1;
			}
		}
		/*设置IP*/
		else if(S_NetSetPageBuffer->lcdinput[0] == 0x2d20)
		{
			if(S_NetSetPageBuffer)
			{
				SetTempIP(&pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
				S_NetSetPageBuffer->ischanged = 1;
			}
		}
		/*确认修改*/
		else if(S_NetSetPageBuffer->lcdinput[0] == 0x2d06)
		{
			if(S_NetSetPageBuffer)
			{
				if(1 == S_NetSetPageBuffer->ischanged)
				{
					if(My_Pass == SaveNetData(&(S_NetSetPageBuffer->myNetData)))
					{
						SendKeyCode(1);
						S_NetSetPageBuffer->ischanged = 0;
					}
					else
						SendKeyCode(2);
				}
			}
		}
		/*返回*/
		else if(S_NetSetPageBuffer->lcdinput[0] == 0x2d05)
		{
			GBPageBufferFree();
			GotoGBParentPage(NULL);
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
	
	SelectPage(80);
	
	ReadNetData(&(S_NetSetPageBuffer->myNetData));

	UpPageValue();
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	if(S_NetSetPageBuffer == NULL)
	{
		S_NetSetPageBuffer = (NetSetPageBuffer *)MyMalloc(sizeof(NetSetPageBuffer));
			
		if(S_NetSetPageBuffer)
		{
			memset(S_NetSetPageBuffer, 0, sizeof(NetSetPageBuffer));
			return My_Pass;
		}
	}
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	if(S_NetSetPageBuffer)
	{
		MyFree(S_NetSetPageBuffer);
		S_NetSetPageBuffer = NULL;
	}
	
	return My_Pass;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void UpPageValue(void)
{
	if(S_NetSetPageBuffer)
	{
		memset(S_NetSetPageBuffer->buf, 0, 100);
		/*更新ip获取方式*/
		if(S_NetSetPageBuffer->myNetData.ipmode != User_Mode)
			S_NetSetPageBuffer->buf[0] = 0x80;	
		else
			S_NetSetPageBuffer->buf[0] = 0x00;

		WriteRadioData(0x2d04, S_NetSetPageBuffer->buf, 2);
		
		/*更新ip*/
		if(S_NetSetPageBuffer->myNetData.ipmode == User_Mode)
		{
			sprintf((S_NetSetPageBuffer->buf), "%03d.%03d.%03d.%03d", S_NetSetPageBuffer->myNetData.myip.ip_1, S_NetSetPageBuffer->myNetData.myip.ip_2, S_NetSetPageBuffer->myNetData.myip.ip_3, S_NetSetPageBuffer->myNetData.myip.ip_4);
			DisText(0x2d20, S_NetSetPageBuffer->buf, strlen((S_NetSetPageBuffer->buf)));
		}
		else
			ClearText(0x2d20, 15);
	}
}

static void SetTempIP(unsigned char *buf, unsigned char len)
{
	unsigned short temp = 0;
	
	if(S_NetSetPageBuffer)
	{
		if(len == 15)
		{
			memset(S_NetSetPageBuffer->buf, 0, 100);
			memcpy(S_NetSetPageBuffer->buf, buf, 3);
			temp = strtol(S_NetSetPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			S_NetSetPageBuffer->myNetData.myip.ip_1 = temp;
			
			memset(S_NetSetPageBuffer->buf, 0, 100);
			memcpy(S_NetSetPageBuffer->buf, buf+4, 3);
			temp = strtol(S_NetSetPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			S_NetSetPageBuffer->myNetData.myip.ip_2 = temp;
			
			memset(S_NetSetPageBuffer->buf, 0, 100);
			memcpy(S_NetSetPageBuffer->buf, buf+8, 3);
			temp = strtol(S_NetSetPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			S_NetSetPageBuffer->myNetData.myip.ip_3 = temp;
			
			memset(S_NetSetPageBuffer->buf, 0, 100);
			memcpy(S_NetSetPageBuffer->buf, buf+12, 3);
			temp = strtol(S_NetSetPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			S_NetSetPageBuffer->myNetData.myip.ip_4 = temp;
		}
		else
			SendKeyCode(3);
	}
}

