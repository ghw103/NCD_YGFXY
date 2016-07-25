/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"RecordPage.h"

#include	"LCD_Driver.h"

#include	"Define.h"
#include	"MyMem.h"

#include	"SystemSetPage.h"
#include	"ShowResultPage.h"
#include	"CRC16.h"

#include	"SDFunction.h"
#include	"UI_Data.h"
#include	"Time_Data.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static RecordPageBuffer * GB_RecordPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpData(void);

static MyState_TypeDef PageInit(void *parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static MyState_TypeDef ShowRecord(unsigned char pageindex);
static void ClearPage(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspRecordPage(void *  parm)
{
	SysPage * myPage = GetSysPage();
	
	myPage->LCDInput = Input;
	myPage->PageUpData = PageUpData;
	myPage->ParentPage = DspSystemSetPage;
	myPage->ChildPage = DspShowResultPage;
	myPage->PageInit = PageInit;
	myPage->PageBufferMalloc = PageBufferMalloc;
	myPage->PageBufferFree = PageBufferFree;
	
	if(DspSystemSetPage == myPage->CurrentPage)
		myPage->PageInit(parm);
	
	myPage->CurrentPage = DspRecordPage;
	
	SelectPage(86);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	unsigned short *pdata = NULL;
	SysPage * myPage = GetSysPage();
	
	pdata = MyMalloc((len/2 + 2)*sizeof(unsigned short));
	if(pdata == NULL)
		return;
	
	/*命令*/
	pdata[0] = pbuf[4];
	pdata[0] = (pdata[0]<<8) + pbuf[5];
	
	/*返回*/
	if(pdata[0] == 0x2801)
	{
		myPage->PageBufferFree();
		myPage->ParentPage(NULL);
	}
	/*打印*/
	else if(pdata[0] == 0x2800)
	{
	}
	/*上一页*/
	else if(pdata[0] == 0x2802)
	{
		if(GB_RecordPageBuffer)
		{
			GB_RecordPageBuffer->tempvalue1 = GB_RecordPageBuffer->pageindex;
			if(GB_RecordPageBuffer->tempvalue1 > 0)
			{
				if(My_Pass == ShowRecord(GB_RecordPageBuffer->tempvalue1-1))
					GB_RecordPageBuffer->pageindex -= 1;
			}
		}
	}
	/*下一页*/
	else if(pdata[0] == 0x2803)
	{
		if(GB_RecordPageBuffer)
		{
			GB_RecordPageBuffer->tempvalue1 = GB_RecordPageBuffer->pageindex;
			if(My_Pass == ShowRecord(GB_RecordPageBuffer->tempvalue1+1))
				GB_RecordPageBuffer->pageindex += 1;
		}
	}
	/*选择数据长按 -- 按下*/
	else if((pdata[0] >= 0x2805)&&(pdata[0] <= 0x280d))
	{
		if(GB_RecordPageBuffer)
		{
			GB_RecordPageBuffer->longpresscount = 0;
			if(GB_RecordPageBuffer->testdata[(pdata[0] - 0x2805)].crc == CalModbusCRC16Fun1(&(GB_RecordPageBuffer->testdata[(pdata[0] - 0x2805)]), sizeof(TestData)-2))
			{
				GB_RecordPageBuffer->selectindex = (pdata[0] - 0x2805);
				BasicPic(0x28c0, 1, 100, 39, 522, 968, 556, 39, 140+(GB_RecordPageBuffer->selectindex)*36);
			}
		}
	}
	/*选择数据长按 -- 持续按下*/
	else if(pdata[0] == 0x2804)
	{
		if(GB_RecordPageBuffer)
			GB_RecordPageBuffer->longpresscount++;
	}
	/*选择数据长按 -- 松开*/
	else if((pdata[0] >= 0x280e)&&(pdata[0] <= 0x2816))
	{
		if(GB_RecordPageBuffer)
		{
			if(GB_RecordPageBuffer->testdata[(pdata[0] - 0x280e)].crc == CalModbusCRC16Fun1(&(GB_RecordPageBuffer->testdata[(pdata[0] - 0x280e)]), sizeof(TestData)-2))
			{
				if(GB_RecordPageBuffer->longpresscount > 15)
				myPage->ChildPage(&GB_RecordPageBuffer->testdata[GB_RecordPageBuffer->selectindex]);
			}
		}
	}
	/*选择其他文件*/
	else if(pdata[0] == 0x28d0)
	{
		if(GB_RecordPageBuffer)
		{
			ClearPage();
			sprintf(GB_RecordPageBuffer->filename, "0:/%.8s.ncd",&pbuf[7]);
			//memcpy(GB_RecordPageBuffer->filename, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			GB_RecordPageBuffer->pageindex = 0;
			
			ShowRecord(GB_RecordPageBuffer->pageindex);
		}
	}

	MyFree(pdata);
}

static void PageUpData(void)
{

}

static MyState_TypeDef PageInit(void *parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	ClearPage();
	/*设置读取数据的文件时间问当前时间*/
	GetGBTimeData(&(GB_RecordPageBuffer->time));
		
	sprintf(GB_RecordPageBuffer->filename, "0:/20%02d%02d%02d.ncd", GB_RecordPageBuffer->time.date.RTC_Year, GB_RecordPageBuffer->time.date.RTC_Month, GB_RecordPageBuffer->time.date.RTC_Date);
	
	GB_RecordPageBuffer->selectindex = 0;
	GB_RecordPageBuffer->pageindex = 0;
	ShowRecord(GB_RecordPageBuffer->pageindex);

	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	if(NULL == GB_RecordPageBuffer)
	{
		GB_RecordPageBuffer = (RecordPageBuffer *)MyMalloc(sizeof(RecordPageBuffer));
		
		if(GB_RecordPageBuffer)
		{
			memset(GB_RecordPageBuffer, 0, sizeof(RecordPageBuffer));
			return My_Pass;
		}
		else
			return My_Fail;
	}
	
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(GB_RecordPageBuffer);
	GB_RecordPageBuffer = NULL;
	
	return My_Pass;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static MyState_TypeDef ShowRecord(unsigned char pageindex)
{
	if(GB_RecordPageBuffer)
	{
		//读取数据
		if(My_Pass == ReadTestData(GB_RecordPageBuffer->filename, GB_RecordPageBuffer->testdata, pageindex))
		{
			for(GB_RecordPageBuffer->tempvalue2=0; GB_RecordPageBuffer->tempvalue2<DataNumInPage; GB_RecordPageBuffer->tempvalue2++)
			{
				GB_RecordPageBuffer->tempdata = &(GB_RecordPageBuffer->testdata[GB_RecordPageBuffer->tempvalue2]);
				
				if(GB_RecordPageBuffer->tempdata->crc == CalModbusCRC16Fun1(GB_RecordPageBuffer->tempdata, sizeof(TestData)-2))
				{
					memset(GB_RecordPageBuffer->buf, 0, 300);
					sprintf(GB_RecordPageBuffer->buf, "%05d   %10s%15s  %8.2f%8.8s %02d:%02d:%02d %s ", pageindex*DataNumInPage+GB_RecordPageBuffer->tempvalue2+1, GB_RecordPageBuffer->tempdata->temperweima.ItemName,
					GB_RecordPageBuffer->tempdata->sampleid, GB_RecordPageBuffer->tempdata->testline.AdjustResult, GB_RecordPageBuffer->tempdata->temperweima.ItemMeasure,
					GB_RecordPageBuffer->tempdata->TestTime.time.RTC_Hours, GB_RecordPageBuffer->tempdata->TestTime.time.RTC_Minutes, GB_RecordPageBuffer->tempdata->TestTime.time.RTC_Seconds,
					GB_RecordPageBuffer->tempdata->user.user_name);
				
					DisText(0x28e0+(GB_RecordPageBuffer->tempvalue2)*0x30, GB_RecordPageBuffer->buf, strlen(GB_RecordPageBuffer->buf));
				}
				else
					ClearText(0x28e0+(GB_RecordPageBuffer->tempvalue2)*0x30, 100);
			}
			return My_Pass;
		}
	}
	
	return My_Fail;
}

static void ClearPage(void)
{
	if(GB_RecordPageBuffer)
	{
		for(GB_RecordPageBuffer->tempvalue2=0; GB_RecordPageBuffer->tempvalue2<DataNumInPage; GB_RecordPageBuffer->tempvalue2++)
			ClearText(0x28e0+(GB_RecordPageBuffer->tempvalue2)*0x30, 100);
		
		BasicPic(0x28c0, 0, 100, 39, 522, 968, 556, 39, 140);
	}
}

