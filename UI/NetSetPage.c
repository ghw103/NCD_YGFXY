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
static NetSetPageBuffer *GB_NetSetPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpData(void);

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
	SysPage * myPage = GetSysPage();

	myPage->CurrentPage = DspNetSetPage;
	myPage->LCDInput = Input;
	myPage->PageUpData = PageUpData;
	myPage->ParentPage = DspNetPreSetPage;
	myPage->ChildPage = NULL;
	myPage->PageInit = PageInit;
	myPage->PageBufferMalloc = PageBufferMalloc;
	myPage->PageBufferFree = PageBufferFree;
	
	myPage->PageInit(parm);
	SelectPage(80);
	
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
	
	/*有线网的ip获取模式*/
	if(pdata[0] == 0x2d04)
	{
		/*数据*/
		pdata[1] = pbuf[7];
		pdata[1] = (pdata[1]<<8) + pbuf[8];
		
		if(GB_NetSetPageBuffer)
		{
			/*自动获取ip*/
			if(pdata[1] == 0x8000)
				GB_NetSetPageBuffer->myNetData.ipmode = DHCP_Mode;
			/*使用设置的ip*/
			else if(pdata[1] == 0x0000)
				GB_NetSetPageBuffer->myNetData.ipmode = User_Mode;
			
			GB_NetSetPageBuffer->ischanged = 1;
		}
	}
	/*设置IP*/
	else if(pdata[0] == 0x2d20)
	{
		if(GB_NetSetPageBuffer)
		{
			SetTempIP(&pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			GB_NetSetPageBuffer->ischanged = 1;
		}
	}
	/*确认修改*/
	else if(pdata[0] == 0x2d06)
	{
		if(GB_NetSetPageBuffer)
		{
			if(1 == GB_NetSetPageBuffer->ischanged)
			{
				if(My_Pass == SaveNetData(&(GB_NetSetPageBuffer->myNetData)))
				{
					SendKeyCode(1);
					GB_NetSetPageBuffer->ischanged = 0;
				}
				else
					SendKeyCode(2);
			}
		}
	}
	/*返回*/
	else if(pdata[0] == 0x2d05)
	{
		PageBufferFree();
		GetSysPage()->ParentPage(NULL);
	}
	
	MyFree(pdata);
}

static void PageUpData(void)
{

}


static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	ReadNetData(&(GB_NetSetPageBuffer->myNetData));

	UpPageValue();
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	if(GB_NetSetPageBuffer == NULL)
	{
		GB_NetSetPageBuffer = (NetSetPageBuffer *)MyMalloc(sizeof(NetSetPageBuffer));
			
		if(GB_NetSetPageBuffer)
		{
			memset(GB_NetSetPageBuffer, 0, sizeof(NetSetPageBuffer));
			return My_Pass;
		}
	}
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	if(GB_NetSetPageBuffer)
	{
		MyFree(GB_NetSetPageBuffer);
		GB_NetSetPageBuffer = NULL;
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
	if(GB_NetSetPageBuffer)
	{
		memset(GB_NetSetPageBuffer->buf, 0, 100);
		/*更新ip获取方式*/
		if(GB_NetSetPageBuffer->myNetData.ipmode != User_Mode)
			GB_NetSetPageBuffer->buf[0] = 0x80;	
		else
			GB_NetSetPageBuffer->buf[0] = 0x00;

		WriteData(0x2d04, GB_NetSetPageBuffer->buf, 2);
		
		/*更新ip*/
		if(GB_NetSetPageBuffer->myNetData.ipmode == User_Mode)
		{
			sprintf((GB_NetSetPageBuffer->buf), "%03d.%03d.%03d.%03d", GB_NetSetPageBuffer->myNetData.myip.ip_1, GB_NetSetPageBuffer->myNetData.myip.ip_2, GB_NetSetPageBuffer->myNetData.myip.ip_3, GB_NetSetPageBuffer->myNetData.myip.ip_4);
			DisText(0x2d20, GB_NetSetPageBuffer->buf, strlen((GB_NetSetPageBuffer->buf)));
		}
		else
			ClearText(0x2d20, 15);
	}
}

static void SetTempIP(unsigned char *buf, unsigned char len)
{
	unsigned short temp = 0;
	
	if(GB_NetSetPageBuffer)
	{
		if(len == 15)
		{
			memset(GB_NetSetPageBuffer->buf, 0, 100);
			memcpy(GB_NetSetPageBuffer->buf, buf, 3);
			temp = strtol(GB_NetSetPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			GB_NetSetPageBuffer->myNetData.myip.ip_1 = temp;
			
			memset(GB_NetSetPageBuffer->buf, 0, 100);
			memcpy(GB_NetSetPageBuffer->buf, buf+4, 3);
			temp = strtol(GB_NetSetPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			GB_NetSetPageBuffer->myNetData.myip.ip_2 = temp;
			
			memset(GB_NetSetPageBuffer->buf, 0, 100);
			memcpy(GB_NetSetPageBuffer->buf, buf+8, 3);
			temp = strtol(GB_NetSetPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			GB_NetSetPageBuffer->myNetData.myip.ip_3 = temp;
			
			memset(GB_NetSetPageBuffer->buf, 0, 100);
			memcpy(GB_NetSetPageBuffer->buf, buf+12, 3);
			temp = strtol(GB_NetSetPageBuffer->buf, NULL, 10);
			if(temp > 255)
			{
				SendKeyCode(3);
				return;
			}
			GB_NetSetPageBuffer->myNetData.myip.ip_4 = temp;
		}
		else
			SendKeyCode(3);
	}
}

