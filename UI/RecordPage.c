/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"RecordPage.h"

#include	"LCD_Driver.h"

#include	"Define.h"
#include	"MyMem.h"

#include	"SystemSetPage.h"
#include	"ShowResultPage.h"
#include	"CRC16.h"
#include	"Printf_Fun.h"
#include	"UI_Data.h"
#include	"System_Data.h"

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
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspRecordPage(void *  parm)
{
	PageInfo * currentpage = NULL;
	
	if(My_Pass == GetCurrentPage(&currentpage))
	{
		currentpage->PageInit = PageInit;
		currentpage->PageUpDate = PageUpDate;
		currentpage->LCDInput = Input;
		currentpage->PageBufferMalloc = PageBufferMalloc;
		currentpage->PageBufferFree = PageBufferFree;
		currentpage->tempP = &S_RecordPageBuffer;
		
		currentpage->PageInit(currentpage->pram);
	}
	
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
		if(S_RecordPageBuffer->lcdinput[0] == 0x1E00)
		{
			PageBufferFree();
			PageBackTo(ParentPage);
		}
		//查看
		else if(S_RecordPageBuffer->lcdinput[0] == 0x1E01)
		{
			if((S_RecordPageBuffer->selectindex > 0) && (S_RecordPageBuffer->selectindex <= S_RecordPageBuffer->readTestDataPackage.readDataNum))
				PageAdvanceTo(DspShowResultPage, &S_RecordPageBuffer->readTestDataPackage.testData[S_RecordPageBuffer->selectindex-1]);
		}
		/*上一页*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x1E03)
		{
			if(S_RecordPageBuffer->pageindex > 1)
				S_RecordPageBuffer->pageindex -= 1;
			else
				S_RecordPageBuffer->pageindex = S_RecordPageBuffer->maxpagenum;
				
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
		/*下一页*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x1E04)
		{
			if(S_RecordPageBuffer->pageindex < S_RecordPageBuffer->maxpagenum)
				S_RecordPageBuffer->pageindex += 1;
			else
				S_RecordPageBuffer->pageindex = 1;
				
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
		//选择数据
		else if((S_RecordPageBuffer->lcdinput[0] >= 0x1E05)&&(S_RecordPageBuffer->lcdinput[0] <= 0x1E0C))
		{
			
			S_RecordPageBuffer->tempvalue1 = S_RecordPageBuffer->lcdinput[0] - 0x1E05 + 1;
			if(S_RecordPageBuffer->tempvalue1 <= S_RecordPageBuffer->readTestDataPackage.readDataNum)
			{
				S_RecordPageBuffer->selectindex = (S_RecordPageBuffer->lcdinput[0] - 0x1E05 + 1);
				BasicPic(0x1E10, 1, 128, 37, 520, 970, 559, 36, 148+(S_RecordPageBuffer->selectindex - 1)*39);
			}
		}
		//跳页
		else if(S_RecordPageBuffer->lcdinput[0] == 0x1fb0)
		{
			S_RecordPageBuffer->tempvalue1 = strtol((char *)(&pbuf[7]), NULL, 10);
			if( (S_RecordPageBuffer->tempvalue1 > 0) && (S_RecordPageBuffer->tempvalue1 <= S_RecordPageBuffer->maxpagenum))
			{
				S_RecordPageBuffer->pageindex = S_RecordPageBuffer->tempvalue1;
		
				S_RecordPageBuffer->selectindex = 0;

				ShowRecord(S_RecordPageBuffer->pageindex);
			}
		}
	}
}

static void PageUpDate(void)
{

}

static MyState_TypeDef PageInit(void *parm)
{
	if(My_Pass == PageBufferMalloc())
	{
		S_RecordPageBuffer->selectindex = 0;
		S_RecordPageBuffer->pageindex = 1;
		ShowRecord(S_RecordPageBuffer->pageindex);
	}
	
	SelectPage(114);
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	if(NULL == S_RecordPageBuffer)
	{
		S_RecordPageBuffer = (RecordPageBuffer *)MyMalloc(sizeof(RecordPageBuffer));
		
		
		if(NULL == S_RecordPageBuffer)
			return My_Fail;
		
		memset(S_RecordPageBuffer, 0, sizeof(RecordPageBuffer));
		
		return My_Pass;
	}
	
	return My_Fail;
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

static MyState_TypeDef ShowRecord(unsigned char pageindex)
{
	unsigned short i=0;
	
	if(S_RecordPageBuffer)
	{
		memset(&(S_RecordPageBuffer->readTestDataPackage), 0, sizeof(ReadTestDataPackage));
		
		S_RecordPageBuffer->tempvalue1 = pageindex-1;
		S_RecordPageBuffer->tempvalue1 *= DataShowNumInPage;
		S_RecordPageBuffer->readTestDataPackage.startReadIndex = S_RecordPageBuffer->tempvalue1;
		S_RecordPageBuffer->readTestDataPackage.maxReadNum = DataShowNumInPage;
		
		//读取数据
		ReadTestData(&(S_RecordPageBuffer->readTestDataPackage));
		
		S_RecordPageBuffer->maxpagenum = ((S_RecordPageBuffer->readTestDataPackage.testDataHead.datanum % DataShowNumInPage) == 0)?(S_RecordPageBuffer->readTestDataPackage.testDataHead.datanum / DataShowNumInPage):
		((S_RecordPageBuffer->readTestDataPackage.testDataHead.datanum / DataShowNumInPage)+1);
		
		BasicPic(0x1E10, 0, 100, 39, 522, 968, 556, 39, 140+(S_RecordPageBuffer->selectindex)*36);
		
		S_RecordPageBuffer->tempdata = &(S_RecordPageBuffer->readTestDataPackage.testData[0]);
		for(i=0; i<DataShowNumInPage; i++)
		{
			if(S_RecordPageBuffer->tempdata->crc == CalModbusCRC16Fun1(S_RecordPageBuffer->tempdata, sizeof(TestData)-2))
			{
				memset(S_RecordPageBuffer->buf, 0, 300);
				sprintf(S_RecordPageBuffer->buf, "%5d   %10s%15s  %8.2f %s %d-%d-%d %d:%d:%d %s ", (pageindex-1)*DataShowNumInPage+i+1, S_RecordPageBuffer->tempdata->temperweima.ItemName,
				S_RecordPageBuffer->tempdata->sampleid, S_RecordPageBuffer->tempdata->testline.AdjustResult, S_RecordPageBuffer->tempdata->temperweima.ItemMeasure,
				S_RecordPageBuffer->tempdata->TestTime.year, S_RecordPageBuffer->tempdata->TestTime.month, S_RecordPageBuffer->tempdata->TestTime.day,
				S_RecordPageBuffer->tempdata->TestTime.hour, S_RecordPageBuffer->tempdata->TestTime.min, S_RecordPageBuffer->tempdata->TestTime.sec,
				S_RecordPageBuffer->tempdata->user.user_name);
				
				DisText(0x1e20+(i)*0x30, S_RecordPageBuffer->buf, 95);
			}
			else
				ClearText(0x1e20+(i)*0x30, 95);
			
			S_RecordPageBuffer->tempdata++;
		}
		
		return My_Pass;
	}
	
	return My_Fail;
}

