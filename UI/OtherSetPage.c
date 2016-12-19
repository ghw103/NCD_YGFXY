/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"OtherSetPage.h"
#include	"SystemSetPage.h"

#include	"SystemSet_Dao.h"
#include	"LCD_Driver.h"
#include	"RTC_Driver.h"
#include	"Define.h"
#include	"MyMem.h"
#include	"UI_Data.h"

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
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);
static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void SetGB_Time(char *buf, unsigned char len);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspOtherSetPage(void *  parm)
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
			PageBackTo(ParentPage);
		}
		/*设置时间*/
		else if(S_OtherSetPageBuffer->lcdinput[0] == 0x2410)
		{
			SetGB_Time((char *)(&pbuf[7]), GetBufLen(&pbuf[7] , 2*pbuf[6]));
		}
		//静音和自动打印设置
		else if(S_OtherSetPageBuffer->lcdinput[0] == 0x2402)
		{
			/*数据*/
			S_OtherSetPageBuffer->lcdinput[1] = pbuf[7];
			S_OtherSetPageBuffer->lcdinput[1] = (S_OtherSetPageBuffer->lcdinput[1]<<8) + pbuf[8];
			
			//自动打印
			if(S_OtherSetPageBuffer->lcdinput[1] & 0x8000)
				S_OtherSetPageBuffer->mySystemSetData.isAutoPrint = true;
			else
				S_OtherSetPageBuffer->mySystemSetData.isAutoPrint = false;
			
			//静音
			if(S_OtherSetPageBuffer->lcdinput[1] & 0x4000)
				S_OtherSetPageBuffer->mySystemSetData.isMute = true;
			else
				S_OtherSetPageBuffer->mySystemSetData.isMute = false;
			
			if(My_Pass == SaveSystemSetData(&(S_OtherSetPageBuffer->mySystemSetData)))
			{
				SendKeyCode(1);
				//保存成功，更新内存中的数据
				setSystemSetData(&(S_OtherSetPageBuffer->mySystemSetData));
			}
			else
				SendKeyCode(2);
		}
		//进入休眠时间
		else if(S_OtherSetPageBuffer->lcdinput[0] == 0x2420)
		{
			memset(S_OtherSetPageBuffer->buf, 0, 50);
			memcpy(S_OtherSetPageBuffer->buf, (char *)(&pbuf[7]), GetBufLen(&pbuf[7] , 2*pbuf[6]));
			
			S_OtherSetPageBuffer->tempvalue = strtol(S_OtherSetPageBuffer->buf, NULL, 10);
			
			if(S_OtherSetPageBuffer->tempvalue > 600)
				S_OtherSetPageBuffer->tempvalue = 600;
			
			S_OtherSetPageBuffer->mySystemSetData.ledSleepTime = S_OtherSetPageBuffer->tempvalue;
			
			memset(S_OtherSetPageBuffer->buf, 0, 50);
			sprintf(S_OtherSetPageBuffer->buf, "%d", S_OtherSetPageBuffer->tempvalue);
			DisText(0x2420, S_OtherSetPageBuffer->buf, strlen(S_OtherSetPageBuffer->buf));
			
			if(My_Pass == SaveSystemSetData(&(S_OtherSetPageBuffer->mySystemSetData)))
			{
				SendKeyCode(1);
				//保存成功，更新内存中的数据
				setSystemSetData(&(S_OtherSetPageBuffer->mySystemSetData));
			}
			else
				SendKeyCode(2);
		}	
		//进入屏幕亮度
		else if(S_OtherSetPageBuffer->lcdinput[0] == 0x2430)
		{
			memset(S_OtherSetPageBuffer->buf, 0, 50);
			memcpy(S_OtherSetPageBuffer->buf, (char *)(&pbuf[7]), GetBufLen(&pbuf[7] , 2*pbuf[6]));
			
			S_OtherSetPageBuffer->tempvalue = strtol(S_OtherSetPageBuffer->buf, NULL, 10);
			
			if(S_OtherSetPageBuffer->tempvalue > 100)
				S_OtherSetPageBuffer->tempvalue = 100;
			if(S_OtherSetPageBuffer->tempvalue < 10)
				S_OtherSetPageBuffer->tempvalue = 10;
			
			S_OtherSetPageBuffer->mySystemSetData.ledSleepTime = S_OtherSetPageBuffer->tempvalue;
			
			memset(S_OtherSetPageBuffer->buf, 0, 50);
			sprintf(S_OtherSetPageBuffer->buf, "%d", S_OtherSetPageBuffer->tempvalue);
			DisText(0x2430, S_OtherSetPageBuffer->buf, strlen(S_OtherSetPageBuffer->buf));
			
			if(My_Pass == SaveSystemSetData(&(S_OtherSetPageBuffer->mySystemSetData)))
			{
				SetLEDLight(S_OtherSetPageBuffer->tempvalue);
				SendKeyCode(1);
				//保存成功，更新内存中的数据
				setSystemSetData(&(S_OtherSetPageBuffer->mySystemSetData));
			}
			else
				SendKeyCode(2);
		}
	}
}

static void PageUpDate(void)
{

}

static MyState_TypeDef PageInit(void *  parm)
{
	if(My_Pass == PageBufferMalloc())
		getSystemSetData(&(S_OtherSetPageBuffer->mySystemSetData));
	
	SelectPage(122);
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
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
	
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_OtherSetPageBuffer);
	S_OtherSetPageBuffer = NULL;
	return My_Pass;
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

