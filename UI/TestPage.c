/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"TestPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"

#include	"MyMem.h"
#include	"WaittingCardPage.h"
#include	"MyTest_Data.h"
#include	"PaiDuiPage.h"
#include	"CardLimit_Driver.h"

#include	"System_Data.h"
#include	"TestDataDao.h"
#include	"Test_Task.h"
#include	"LunchPage.h"
#include	"Printf_Fun.h"
#include	"Motor_Fun.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static TestPageBuffer * S_TestPageBuffer = NULL;
const unsigned int TestLineHigh = 77010;	//此数据与曲线显示区域高度有关，如果界面不改，此数不改
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void RefreshCurve(void);
static void AddDataToLine(unsigned short data);
static void RefreshPageText(void);

static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);
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
MyState_TypeDef createTestActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "TestActivity", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_TestPageBuffer)
	{
		/*清空曲线*/
		ClearLine(0x56);
		
		//初始化测试曲线
		S_TestPageBuffer->line.MaxData = 0;
		S_TestPageBuffer->line.MUL_Y = 1;
		S_TestPageBuffer->line.Y_Scale = 100;
		
		SetChartSize(0x1870 , S_TestPageBuffer->line.MUL_Y);
		
		DspNum(0x180B , S_TestPageBuffer->line.Y_Scale, 2);
		DspNum(0x180A , S_TestPageBuffer->line.Y_Scale*2, 2);
		
		/*获取当前测试数据的地址*/
		S_TestPageBuffer->currenttestdata = GetCurrentTestItem();
		
		RefreshPageText();

		StartTest(&(S_TestPageBuffer->currenttestdata->testdata));
	}
	
	SelectPage(96);

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
	if(S_TestPageBuffer)
	{
		/*命令*/
		S_TestPageBuffer->lcdinput[0] = pbuf[4];
		S_TestPageBuffer->lcdinput[0] = (S_TestPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*退出*/
		if(0x1801 == S_TestPageBuffer->lcdinput[0])
		{
			if(S_TestPageBuffer->cardpretestresult != NoResult)
			{
				if(CardPinIn)
				{
					SendKeyCode(5);
					return;
				}
				
				//删除当前测试
				DeleteCurrentTest();
				
				backToActivity(lunchActivityName);
				
				//如果还有卡在排队，则直接跳到排队界面
				if(IsPaiDuiTestting())
					startActivity(createPaiDuiActivity, NULL);
			}
			//正在测试不允许退出
			else
				SendKeyCode(4);
		}
		/*打印数据*/
		else if(0x1800 == S_TestPageBuffer->lcdinput[0])
		{
			if(S_TestPageBuffer->cardpretestresult == ResultIsOK)
			{
				if(My_Pass == ConnectPrintter())
				{
					SendKeyCode(6);
					PrintfData(&(S_TestPageBuffer->currenttestdata->testdata));
					SendKeyCode(16);
				}
				else
					SendKeyCode(4);
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
	if(S_TestPageBuffer)
		RefreshCurve();
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
	if(NULL == S_TestPageBuffer)
	{
		S_TestPageBuffer = MyMalloc(sizeof(TestPageBuffer));
		
		if(S_TestPageBuffer)
		{
			memset(S_TestPageBuffer, 0, sizeof(TestPageBuffer));
	
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
	MyFree(S_TestPageBuffer);
	S_TestPageBuffer = NULL;
}



/*更新数据*/
static void RefreshCurve(void)
{
	unsigned short temp;
	
	if(pdTRUE == TakeTestPointData(&temp))
	{
		if(0xffff == temp)
		{
			ClearLine(0x56);
				
			//初始化测试曲线
			S_TestPageBuffer->line.MaxData = 0;
			S_TestPageBuffer->line.MUL_Y = 1;
			S_TestPageBuffer->line.Y_Scale = 100;
				
			SetChartSize(0x1870 , S_TestPageBuffer->line.MUL_Y);
				
			DspNum(0x180B , S_TestPageBuffer->line.Y_Scale, 2);
			DspNum(0x180A , S_TestPageBuffer->line.Y_Scale*2, 2);
		}
		else
			AddDataToLine(temp);
	}
	 
	if(My_Pass == TakeTestResult(&(S_TestPageBuffer->cardpretestresult)))
	{
		
		MotorMoveTo(MaxLocation, 1);
		
		if(S_TestPageBuffer->cardpretestresult == ResultIsOK)
		{
			RefreshPageText();
				
			GetGB_Time(&(GetCurrentTestItem()->testdata.TestTime));
			WriteTestData(&(GetCurrentTestItem()->testdata));
		}
		else if(S_TestPageBuffer->cardpretestresult == PeakError)
		{
			/*测试失败*/
			SendKeyCode(2);
		}
		else
		{
			SendKeyCode(1);			
		}
	}
}

static void RefreshPageText(void)
{
	char *buf = NULL;
	double tempvalue = 0.0;
	
	buf = MyMalloc(100);
	if(buf)
	{
		memset(buf, 0, 100);
		
		if((S_TestPageBuffer)&&(S_TestPageBuffer->currenttestdata))
		{
			sprintf(buf, "%s", S_TestPageBuffer->currenttestdata->testdata.temperweima.ItemName);
			DisText(0x1810, buf, 20);
			
			memset(buf, 0, 100);
			memcpy(buf, S_TestPageBuffer->currenttestdata->testdata.sampleid, MaxSampleIDLen);
			DisText(0x1820, buf, 20);
			
			sprintf(buf, "%2.1f", S_TestPageBuffer->currenttestdata->testdata.TestTemp.O_Temperature);
			DisText(0x1830, buf, 8);
			
			sprintf(buf, "%s", S_TestPageBuffer->currenttestdata->testdata.temperweima.CardPiCi);
			DisText(0x1840, buf, 30);
			
			if(S_TestPageBuffer->currenttestdata->testdata.testline.AdjustResult <= S_TestPageBuffer->currenttestdata->testdata.temperweima.LowstResult)
				sprintf(buf, "<%.3f", S_TestPageBuffer->currenttestdata->testdata.temperweima.LowstResult);
			else if(S_TestPageBuffer->currenttestdata->testdata.testline.AdjustResult >= S_TestPageBuffer->currenttestdata->testdata.temperweima.HighestResult)
				sprintf(buf, ">%.3f", S_TestPageBuffer->currenttestdata->testdata.temperweima.HighestResult);
			else
				sprintf(buf, "%.3f", S_TestPageBuffer->currenttestdata->testdata.testline.AdjustResult);
			DisText(0x1850, buf, 30);
		}
	}
	MyFree(buf);
	
	if(S_TestPageBuffer->cardpretestresult == ResultIsOK)
	{
		//在曲线上标记出T,C,基线
		S_TestPageBuffer->myico[0].ICO_ID = 22;
		S_TestPageBuffer->myico[0].X = 505+S_TestPageBuffer->currenttestdata->testdata.testline.T_Point[1]-5;
		tempvalue = S_TestPageBuffer->currenttestdata->testdata.testline.T_Point[0];
		tempvalue /= S_TestPageBuffer->line.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_TestPageBuffer->myico[0].Y = (unsigned short)tempvalue - 5;
		
		S_TestPageBuffer->myico[1].ICO_ID = 22;
		S_TestPageBuffer->myico[1].X = 505+S_TestPageBuffer->currenttestdata->testdata.testline.C_Point[1]-5;
		tempvalue = S_TestPageBuffer->currenttestdata->testdata.testline.C_Point[0];
		tempvalue /= S_TestPageBuffer->line.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_TestPageBuffer->myico[1].Y = (unsigned short)tempvalue - 5;
		
		S_TestPageBuffer->myico[2].ICO_ID = 22;
		S_TestPageBuffer->myico[2].X = 505+S_TestPageBuffer->currenttestdata->testdata.testline.B_Point[1]-5;
		tempvalue = S_TestPageBuffer->currenttestdata->testdata.testline.B_Point[0];
		tempvalue /= S_TestPageBuffer->line.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_TestPageBuffer->myico[2].Y = (unsigned short)tempvalue - 5;
		
		BasicUI(0x1880 ,0x1907 , 3, &(S_TestPageBuffer->myico[0]) , sizeof(Basic_ICO)*3);
	}
	else
	{
		memset(S_TestPageBuffer->myico, 0, sizeof(Basic_ICO)*3);
		BasicUI(0x1880 ,0x1907 , 3, &S_TestPageBuffer->myico[0] , sizeof(Basic_ICO)*3);
	}
}


static void AddDataToLine(unsigned short data)
{
	unsigned short tempdata = data;
	double TempMul_Y2;
	double TempY_Scale;
	unsigned short tempvalue;
	
	if(S_TestPageBuffer->line.MaxData <= tempdata)
	{
		S_TestPageBuffer->line.MaxData = tempdata;

		////////////////////////针对当前曲线最大值计算y轴放大倍数//////////////////////////////////////
		TempMul_Y2 = TestLineHigh;
		TempMul_Y2 /= S_TestPageBuffer->line.MaxData;
		TempMul_Y2 *= 0.8;			//*0.8是将最大值缩放到满刻度的0.8高度处

		
		tempvalue = (unsigned short)(TempMul_Y2*10);
		S_TestPageBuffer->line.MUL_Y = ((tempvalue%10) > 5)?(tempvalue/10 + 1):(tempvalue/10);
		
		if(S_TestPageBuffer->line.MUL_Y < 1)			//最小值为1
			S_TestPageBuffer->line.MUL_Y = 1;

		/////////////////////////针对当前放大倍数，计算y轴刻度递增基数/////////////////////////////////////
		TempY_Scale = TestLineHigh ;
		TempY_Scale /= S_TestPageBuffer->line.MUL_Y;
		TempY_Scale /= 2.0;																//目前显示2个y轴刻度
		S_TestPageBuffer->line.Y_Scale = (unsigned short)TempY_Scale;
		
		SetChartSize(0x1870 , S_TestPageBuffer->line.MUL_Y);

		DspNum(0x180B , S_TestPageBuffer->line.Y_Scale, 2);
		DspNum(0x180A , S_TestPageBuffer->line.Y_Scale*2, 2);

	}
	DisPlayLine(0 , &tempdata , 1);

}

