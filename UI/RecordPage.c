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
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static RecordPageBuffer * S_RecordPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);

static MyState_TypeDef PageInit(void *parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static MyState_TypeDef ShowRecord(unsigned char pageindex);
static void ClearPage(void);
static void ParseInputTimeStr(char *buf, unsigned char len);
static MyState_TypeDef ReadFileSaveDateInfo(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspRecordPage(void *  parm)
{
	SetGBParentPage(DspSystemSetPage);
	SetGBChildPage(DspShowResultPage);
	SetGBPageUpDate(PageUpDate);
	SetGBGBPageInput(Input);
	SetGBPageInit(PageInit);
	SetGBPageBufferMalloc(PageBufferMalloc);
	SetGBPageBufferFree(PageBufferFree);
	
	if(DspSystemSetPage == GetGBCurrentPage())
		GBPageInit(parm);
	
	SelectPage(86);
	
	SetGBCurrentPage(DspRecordPage);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_RecordPageBuffer)
	{
		/*命令*/
		S_RecordPageBuffer->lcdinput[0] = pbuf[4];
		S_RecordPageBuffer->lcdinput[0] = (S_RecordPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_RecordPageBuffer->lcdinput[0] == 0x2801)
		{
			GBPageBufferFree();
			GotoGBParentPage(NULL);
		}
		/*打印*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2800)
		{
		}
		/*上一页*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2802)
		{
			if(S_RecordPageBuffer->pageindex > 0)
				S_RecordPageBuffer->pageindex -= 1;
				
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
		/*下一页*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2803)
		{
			if((S_RecordPageBuffer->pageindex + 1) < S_RecordPageBuffer->maxpagenum)
				S_RecordPageBuffer->pageindex += 1;
				
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
		/*选择数据长按 -- 按下*/
		else if((S_RecordPageBuffer->lcdinput[0] >= 0x2805)&&(S_RecordPageBuffer->lcdinput[0] <= 0x280d))
		{
			S_RecordPageBuffer->longpresscount = 0;
			if(S_RecordPageBuffer->testdata[(S_RecordPageBuffer->lcdinput[0] - 0x2805)].crc == CalModbusCRC16Fun1(&(S_RecordPageBuffer->testdata[(S_RecordPageBuffer->lcdinput[0] - 0x2805)]), sizeof(TestData)-2))
			{
				S_RecordPageBuffer->selectindex = (S_RecordPageBuffer->lcdinput[0] - 0x2805);
				BasicPic(0x28c0, 1, 100, 39, 522, 968, 556, 39, 140+(S_RecordPageBuffer->selectindex)*36);
			}
		}
		/*选择数据长按 -- 持续按下*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2804)
		{
			S_RecordPageBuffer->longpresscount++;
		}
		/*选择数据长按 -- 松开*/
		else if((S_RecordPageBuffer->lcdinput[0] >= 0x280e)&&(S_RecordPageBuffer->lcdinput[0] <= 0x2816))
		{
			if(S_RecordPageBuffer->longpresscount > 5)
				GotoGBChildPage(&S_RecordPageBuffer->testdata[S_RecordPageBuffer->selectindex]);
		}
		/*选择其他文件*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x28d0)
		{
			ClearPage();
			ParseInputTimeStr((char *)(&pbuf[7]), GetBufLen(&pbuf[7] , 2*pbuf[6]));

			ReadFileSaveDateInfo();
			S_RecordPageBuffer->selectindex = 0;
			S_RecordPageBuffer->pageindex = 0;
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
	}
}

static void PageUpDate(void)
{

}

static MyState_TypeDef PageInit(void *parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	ClearPage();
	/*设置读取数据的文件时间问当前时间*/
	GetGBTimeData(&(S_RecordPageBuffer->datatime));
	
	ReadFileSaveDateInfo();
	
	S_RecordPageBuffer->selectindex = 0;
	S_RecordPageBuffer->pageindex = 0;
	ShowRecord(S_RecordPageBuffer->pageindex);

	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	if(NULL == S_RecordPageBuffer)
	{
		S_RecordPageBuffer = (RecordPageBuffer *)MyMalloc(sizeof(RecordPageBuffer));
		
		if(NULL == S_RecordPageBuffer)
			return My_Fail;
	}
	
	memset(S_RecordPageBuffer, 0, sizeof(RecordPageBuffer));
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_RecordPageBuffer);
	S_RecordPageBuffer = NULL;
	
	return My_Pass;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static MyState_TypeDef ReadFileSaveDateInfo(void)
{
	if(S_RecordPageBuffer)
	{
		S_RecordPageBuffer->maxdatanum = 0;
		memset(S_RecordPageBuffer->testdatainfo, 0, TestDataDateRepeatNum*sizeof(TestDateInfo_Def));
		
		//读取数据的日期索引
		ReadDateInfo(S_RecordPageBuffer->testdatainfo, &(S_RecordPageBuffer->datatime));
		
		for(S_RecordPageBuffer->tempvalue2=0; S_RecordPageBuffer->tempvalue2<TestDataDateRepeatNum; S_RecordPageBuffer->tempvalue2++)
			S_RecordPageBuffer->maxdatanum += S_RecordPageBuffer->testdatainfo[S_RecordPageBuffer->tempvalue2].num;
		
		S_RecordPageBuffer->maxpagenum = ((S_RecordPageBuffer->maxdatanum % DataNumInPage) == 0)?(S_RecordPageBuffer->maxdatanum / DataNumInPage):
		((S_RecordPageBuffer->maxdatanum / DataNumInPage)+1);
	}
	
	return My_Pass;
}

static MyState_TypeDef ShowRecord(unsigned char pageindex)
{
	unsigned short i=0, j=0;
	unsigned char num = 0;
	if(S_RecordPageBuffer)
	{
		memset(S_RecordPageBuffer->testdata, 0, DataNumInPage*sizeof(TestData));
		
		S_RecordPageBuffer->tempvalue1 = pageindex;
		S_RecordPageBuffer->tempvalue1 *= DataNumInPage;
		
		S_RecordPageBuffer->tempvalue2 = 0;
		
		for(i=0; i<TestDataDateRepeatNum; i++)
		{
			for(j=0; j<S_RecordPageBuffer->testdatainfo[i].num; j++)
			{
				if((S_RecordPageBuffer->tempvalue2 >= S_RecordPageBuffer->tempvalue1) && 
				(S_RecordPageBuffer->tempvalue2 < (S_RecordPageBuffer->tempvalue1 + DataNumInPage)))
				{
					ReadTestData(&(S_RecordPageBuffer->testdata[num]), S_RecordPageBuffer->testdatainfo[i].index+j-1, 1, NULL);
					num++;
					if(num > DataNumInPage)
						goto END;
				}
				S_RecordPageBuffer->tempvalue2++;
			}			
		}
		
		END:
		for(S_RecordPageBuffer->tempvalue2=0; S_RecordPageBuffer->tempvalue2<DataNumInPage; S_RecordPageBuffer->tempvalue2++)
		{
			S_RecordPageBuffer->tempdata = &(S_RecordPageBuffer->testdata[S_RecordPageBuffer->tempvalue2]);
				
			if(S_RecordPageBuffer->tempdata->crc == CalModbusCRC16Fun1(S_RecordPageBuffer->tempdata, sizeof(TestData)-2))
			{
				memset(S_RecordPageBuffer->buf, 0, 300);
				sprintf(S_RecordPageBuffer->buf, "%05d   %10s%15s  %8.2f%8.8s %02d:%02d:%02d %s ", pageindex*DataNumInPage+S_RecordPageBuffer->tempvalue2+1, S_RecordPageBuffer->tempdata->temperweima.ItemName,
				S_RecordPageBuffer->tempdata->sampleid, S_RecordPageBuffer->tempdata->testline.AdjustResult, S_RecordPageBuffer->tempdata->temperweima.ItemMeasure,
				S_RecordPageBuffer->tempdata->TestTime.hour, S_RecordPageBuffer->tempdata->TestTime.min, S_RecordPageBuffer->tempdata->TestTime.sec,
				S_RecordPageBuffer->tempdata->user.user_name);
				
				DisText(0x28e0+(S_RecordPageBuffer->tempvalue2)*0x30, S_RecordPageBuffer->buf, strlen(S_RecordPageBuffer->buf));
			}
			else
				ClearText(0x28e0+(S_RecordPageBuffer->tempvalue2)*0x30, 100);
		}
		return My_Pass;
	}
	
	return My_Fail;
}

static void ClearPage(void)
{
	if(S_RecordPageBuffer)
	{
		for(S_RecordPageBuffer->tempvalue2=0; S_RecordPageBuffer->tempvalue2<DataNumInPage; S_RecordPageBuffer->tempvalue2++)
			ClearText(0x28e0+(S_RecordPageBuffer->tempvalue2)*0x30, 100);
		
		BasicPic(0x28c0, 0, 100, 39, 522, 968, 556, 39, 140);
	}
}
static void ParseInputTimeStr(char *buf, unsigned char len)
{
	short temp = 0;
	
	if(S_RecordPageBuffer)
	{
		if(len < 8)
			goto END;
		
		memset(S_RecordPageBuffer->buf, 0, 50);
		memcpy(S_RecordPageBuffer->buf, buf, 4);
		temp = strtol(S_RecordPageBuffer->buf, NULL, 10);
		if((temp < 2000)||(temp > 2100))
			goto END;
		S_RecordPageBuffer->datatime.year = temp - 2000;
		
		memset(S_RecordPageBuffer->buf, 0, 50);
		memcpy(S_RecordPageBuffer->buf, buf+4, 2);
		temp = strtol(S_RecordPageBuffer->buf, NULL, 10);
		if((temp < 1)||(temp > 12))
			goto END;
		S_RecordPageBuffer->datatime.month = temp;
		
		memset(S_RecordPageBuffer->buf, 0, 50);
		memcpy(S_RecordPageBuffer->buf, buf+6, 2);
		temp = strtol(S_RecordPageBuffer->buf, NULL, 10);
		if((temp < 1)||(temp > 31))
			goto END;
		S_RecordPageBuffer->datatime.day = temp;
		
		return;
		
		END:
			memset(&(S_RecordPageBuffer->datatime), 0, sizeof(MyTime_Def));
	}
}
