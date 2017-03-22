/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"ShowResultPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"MyMem.h"
#include	"Test_Fun.h"
#include	"Intent.h"

#include	"MyTest_Data.h"
#include	"System_Data.h"
#include	"Define.h"
#include	"SDFunction.h"
#include	"Printf_Fun.h"
#include	"RecordPage.h"
#include	"SleepPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static ShowPageBuffer * S_ShowPageBuffer = NULL;

static unsigned int TestLineHigh = 76500;	//此数据与曲线显示区域高度有关，如果界面不改，此数不改
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void RefreshText(void);
static void DspLine(void);
static void dspIco(void);

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
MyState_TypeDef createShowResultActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "ShowResultActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		//如果传入参数，
		if(pram)
		{
			readIntent(pram, &(S_ShowPageBuffer->testdata), sizeof(TestData));
		}
		
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
	if(S_ShowPageBuffer)
	{

		RefreshText();
			
		DspLine();
			
		dspIco();
	}
	
	SelectPage(147);
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
	if(S_ShowPageBuffer)
	{
		/*命令*/
		S_ShowPageBuffer->lcdinput[0] = pbuf[4];
		S_ShowPageBuffer->lcdinput[0] = (S_ShowPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*退出*/
		if(0x2301 == S_ShowPageBuffer->lcdinput[0])
		{
			backToFatherActivity();
		}
		/*打印*/
		else if(0x2300 == S_ShowPageBuffer->lcdinput[0])
		{
			SendKeyCode(1);
			PrintfData(&(S_ShowPageBuffer->testdata));
			SendKeyCode(16);
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
	if(S_ShowPageBuffer)
	{

	}
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
	if(S_ShowPageBuffer)
	{

	}
	
	SelectPage(147);
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
	if(NULL == S_ShowPageBuffer)
	{
		S_ShowPageBuffer = MyMalloc(sizeof(ShowPageBuffer));
		
		if(S_ShowPageBuffer)
		{
			memset(S_ShowPageBuffer, 0, sizeof(ShowPageBuffer));
	
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
	MyFree(S_ShowPageBuffer);
	S_ShowPageBuffer = NULL;
}

static void RefreshText(void)
{
	if(S_ShowPageBuffer)
	{
		sprintf(S_ShowPageBuffer->tempbuf, "%s\0", S_ShowPageBuffer->testdata.temperweima.ItemName);
		DisText(0x2310, S_ShowPageBuffer->tempbuf, strlen(S_ShowPageBuffer->tempbuf)+1);
				
		sprintf(S_ShowPageBuffer->tempbuf, "%s\0", S_ShowPageBuffer->testdata.sampleid);
		DisText(0x2320, S_ShowPageBuffer->tempbuf, strlen(S_ShowPageBuffer->tempbuf)+1);
				
		sprintf(S_ShowPageBuffer->tempbuf, "%2.1f ℃\0", S_ShowPageBuffer->testdata.TestTemp.O_Temperature);
		DisText(0x2330, S_ShowPageBuffer->tempbuf, strlen(S_ShowPageBuffer->tempbuf)+1);
		
		sprintf(S_ShowPageBuffer->tempbuf, "%s-%s\0", S_ShowPageBuffer->testdata.temperweima.PiHao,
			S_ShowPageBuffer->testdata.temperweima.piNum);
		DisText(0x2340, S_ShowPageBuffer->tempbuf, strlen(S_ShowPageBuffer->tempbuf)+1);
		
		
		if(IsShowRealValue() == true)
			sprintf(S_ShowPageBuffer->tempbuf, "%.*f %s\0", S_ShowPageBuffer->testdata.temperweima.itemConstData.pointNum,
				S_ShowPageBuffer->testdata.testline.AdjustResult, S_ShowPageBuffer->testdata.temperweima.itemConstData.itemMeasure);
		else if(S_ShowPageBuffer->testdata.testline.AdjustResult <= S_ShowPageBuffer->testdata.temperweima.itemConstData.lowstResult)
			sprintf(S_ShowPageBuffer->tempbuf, "<%.*f %s\0", S_ShowPageBuffer->testdata.temperweima.itemConstData.pointNum, 
				S_ShowPageBuffer->testdata.temperweima.itemConstData.lowstResult, S_ShowPageBuffer->testdata.temperweima.itemConstData.itemMeasure);
		else if(S_ShowPageBuffer->testdata.testline.AdjustResult >= S_ShowPageBuffer->testdata.temperweima.itemConstData.highestResult)
			sprintf(S_ShowPageBuffer->tempbuf, ">%.*f %s\0", S_ShowPageBuffer->testdata.temperweima.itemConstData.pointNum, 
				S_ShowPageBuffer->testdata.temperweima.itemConstData.highestResult, S_ShowPageBuffer->testdata.temperweima.itemConstData.itemMeasure);
		else
			sprintf(S_ShowPageBuffer->tempbuf, "%.*f %s\0", S_ShowPageBuffer->testdata.temperweima.itemConstData.pointNum, 
				S_ShowPageBuffer->testdata.testline.AdjustResult,S_ShowPageBuffer->testdata.temperweima.itemConstData.itemMeasure);

		DisText(0x2338, S_ShowPageBuffer->tempbuf, strlen(S_ShowPageBuffer->tempbuf)+1);
		
		sprintf(S_ShowPageBuffer->tempbuf, "%s\0", S_ShowPageBuffer->testdata.temperweima.itemConstData.normalResult);

		DisText(0x2350, S_ShowPageBuffer->tempbuf, strlen(S_ShowPageBuffer->tempbuf)+1);

	}
}

static void DspLine(void)
{
	unsigned short i=0;
	double TempMul_Y2;
	double TempY_Scale;
	unsigned short tempvalue;
	
	unsigned short *p = NULL;

	ClearLine(0x57);
	vTaskDelay(200 / portTICK_RATE_MS);
	for(i=0; i<MaxPointLen;i++)
	{
		if(i%50 == 0)
		{
			p = &(S_ShowPageBuffer->testdata.testline.TestPoint[i]);
			DisPlayLine(1 , p , 50);
			vTaskDelay(2 / portTICK_RATE_MS);
		}
		
		if(S_ShowPageBuffer->lineinfo.MaxData <= S_ShowPageBuffer->testdata.testline.TestPoint[i])
			S_ShowPageBuffer->lineinfo.MaxData = S_ShowPageBuffer->testdata.testline.TestPoint[i];
	}
	
	////////////////////////针对当前曲线最大值计算y轴放大倍数//////////////////////////////////////
		TempMul_Y2 = TestLineHigh;
		TempMul_Y2 /= S_ShowPageBuffer->lineinfo.MaxData;
		TempMul_Y2 *= 0.8;			//*0.8是将最大值缩放到满刻度的0.8高度处

		
		tempvalue = (unsigned short)(TempMul_Y2*10);
		S_ShowPageBuffer->lineinfo.MUL_Y = ((tempvalue%10) > 5)?(tempvalue/10 + 1):(tempvalue/10);
		
		if(S_ShowPageBuffer->lineinfo.MUL_Y < 1)			//最小值为1
			S_ShowPageBuffer->lineinfo.MUL_Y = 1;

		/////////////////////////针对当前放大倍数，计算y轴刻度递增基数/////////////////////////////////////
		TempY_Scale = TestLineHigh ;
		TempY_Scale /= S_ShowPageBuffer->lineinfo.MUL_Y;
		TempY_Scale /= 2.0;																//目前显示2个y轴刻度
		S_ShowPageBuffer->lineinfo.Y_Scale = (unsigned short)TempY_Scale;
		
		SetChartSize(0x2370 , S_ShowPageBuffer->lineinfo.MUL_Y);

		DspNum(0x230b , S_ShowPageBuffer->lineinfo.Y_Scale, 2);
		DspNum(0x230a , S_ShowPageBuffer->lineinfo.Y_Scale*2, 2);

}

static void dspIco(void)
{
	double tempvalue = 0.0;
	if(S_ShowPageBuffer)
	{
		//在曲线上标记出T,C,基线
		S_ShowPageBuffer->myico[0].ICO_ID = 22;
		S_ShowPageBuffer->myico[0].X = 574+S_ShowPageBuffer->testdata.testline.T_Point[1]-12;
		tempvalue = S_ShowPageBuffer->testdata.testline.T_Point[0];
		tempvalue /= S_ShowPageBuffer->lineinfo.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_ShowPageBuffer->myico[0].Y = (unsigned short)tempvalue - 11;
		
		S_ShowPageBuffer->myico[1].ICO_ID = 22;
		S_ShowPageBuffer->myico[1].X = 574+S_ShowPageBuffer->testdata.testline.C_Point[1]-12;
		tempvalue = S_ShowPageBuffer->testdata.testline.C_Point[0];
		tempvalue /= S_ShowPageBuffer->lineinfo.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_ShowPageBuffer->myico[1].Y = (unsigned short)tempvalue - 11;
		
		S_ShowPageBuffer->myico[2].ICO_ID = 22;
		S_ShowPageBuffer->myico[2].X = 574+S_ShowPageBuffer->testdata.testline.B_Point[1]-12;
		tempvalue = S_ShowPageBuffer->testdata.testline.B_Point[0];
		tempvalue /= S_ShowPageBuffer->lineinfo.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_ShowPageBuffer->myico[2].Y = (unsigned short)tempvalue - 11;
		
		BasicUI(0x2380 ,0x1807 , 3, &(S_ShowPageBuffer->myico[0]) , sizeof(Basic_ICO)*3);
	}
}

