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
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);

static MyState_TypeDef ShowRecord(unsigned char pageindex);
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
MyState_TypeDef createRecordActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "RecordActivity", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_RecordPageBuffer)
	{
		S_RecordPageBuffer->selectindex = 0;
		S_RecordPageBuffer->pageindex = 1;
		ShowRecord(S_RecordPageBuffer->pageindex);
	}
	
	SelectPage(114);
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
	if(S_RecordPageBuffer)
	{
		/*命令*/
		S_RecordPageBuffer->lcdinput[0] = pbuf[4];
		S_RecordPageBuffer->lcdinput[0] = (S_RecordPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_RecordPageBuffer->lcdinput[0] == 0x2000)
		{
			backToFatherActivity();
		}
		//查看
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2001)
		{
			if((S_RecordPageBuffer->selectindex > 0) && (S_RecordPageBuffer->selectindex <= S_RecordPageBuffer->readTestDataPackage.readDataNum))
				startActivity(createShowResultActivity, createIntent(&S_RecordPageBuffer->readTestDataPackage.testData[S_RecordPageBuffer->selectindex-1], sizeof(TestData)));
		}
		/*上一页*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2002)
		{
			if(S_RecordPageBuffer->pageindex > 1)
				S_RecordPageBuffer->pageindex -= 1;
			else
				S_RecordPageBuffer->pageindex = S_RecordPageBuffer->maxpagenum;
				
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
		/*下一页*/
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2003)
		{
			if(S_RecordPageBuffer->pageindex < S_RecordPageBuffer->maxpagenum)
				S_RecordPageBuffer->pageindex += 1;
			else
				S_RecordPageBuffer->pageindex = 1;
				
			ShowRecord(S_RecordPageBuffer->pageindex);
		}
		//选择数据
		else if((S_RecordPageBuffer->lcdinput[0] >= 0x2004)&&(S_RecordPageBuffer->lcdinput[0] <= 0x200b))
		{
			
			S_RecordPageBuffer->tempvalue1 = S_RecordPageBuffer->lcdinput[0] - 0x2004 + 1;
			if(S_RecordPageBuffer->tempvalue1 <= S_RecordPageBuffer->readTestDataPackage.readDataNum)
			{
				S_RecordPageBuffer->selectindex = (S_RecordPageBuffer->lcdinput[0] - 0x2004 + 1);
				BasicPic(0x2020, 1, 137, 83, 417, 1012, 454, 36, 148+(S_RecordPageBuffer->selectindex - 1)*39);
			}
		}
		//跳页
		else if(S_RecordPageBuffer->lcdinput[0] == 0x2010)
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
	SelectPage(114);
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
	if(NULL == S_RecordPageBuffer)
	{
		S_RecordPageBuffer = MyMalloc(sizeof(RecordPageBuffer));
		
		if(S_RecordPageBuffer)
		{
			memset(S_RecordPageBuffer, 0, sizeof(RecordPageBuffer));
	
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
	MyFree(S_RecordPageBuffer);
	S_RecordPageBuffer = NULL;
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
		
		BasicPic(0x2020, 0, 100, 39, 522, 968, 556, 39, 140+(S_RecordPageBuffer->selectindex)*36);
		
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
				
				DisText(0x2030+(i)*0x40, S_RecordPageBuffer->buf, 120);
			}
			else
				ClearText(0x2030+(i)*0x40, 120);
			
			S_RecordPageBuffer->tempdata++;
		}
		
		return My_Pass;
	}
	
	return My_Fail;
}

