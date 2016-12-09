/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"ShowResultPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"Test_Fun.h"

#include	"MyTest_Data.h"
#include	"System_Data.h"
#include	"Define.h"
#include	"SDFunction.h"
#include	"Printf_Fun.h"
#include	"RecordPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static ShowPageBuffer * S_ShowPageBuffer = NULL;

static unsigned int TestLineHigh = 76800;	//此数据与曲线显示区域高度有关，如果界面不改，此数不改
/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);

static void RefreshText(void);
static void DspLine(void);
static void dspIco(void);

static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspShowResultPage(void *  parm)
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
	if(S_ShowPageBuffer)
	{
		/*命令*/
		S_ShowPageBuffer->lcdinput[0] = pbuf[4];
		S_ShowPageBuffer->lcdinput[0] = (S_ShowPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*退出*/
		if(0x2001 == S_ShowPageBuffer->lcdinput[0])
		{
			PageBufferFree();
			PageBackTo(ParentPage);
		}
		/*打印*/
		else if(0x2000 == S_ShowPageBuffer->lcdinput[0])
		{
			SendKeyCode(1);
			PrintfData(S_ShowPageBuffer->testdata);
			SendKeyCode(9);
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
	
	SelectPage(147);

	if(parm && S_ShowPageBuffer)
	{
		S_ShowPageBuffer->testdata = parm;
		
		RefreshText();
		
		DspLine();
		
		dspIco();
	}
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	if(NULL == S_ShowPageBuffer)
	{
		S_ShowPageBuffer = (ShowPageBuffer *)MyMalloc(sizeof(ShowPageBuffer));
			
		if(NULL == S_ShowPageBuffer)
			return My_Fail;
	}
	
	memset(S_ShowPageBuffer, 0, sizeof(ShowPageBuffer));
		
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_ShowPageBuffer);
	S_ShowPageBuffer = NULL;
	
	return My_Pass;
}

static void RefreshText(void)
{
	if(S_ShowPageBuffer)
	{
		memset(S_ShowPageBuffer->tempbuf, 0, 100);
		sprintf(S_ShowPageBuffer->tempbuf, "%s", S_ShowPageBuffer->testdata->temperweima.ItemName);
		DisText(0x2010, S_ShowPageBuffer->tempbuf, 20);
				
		memset(S_ShowPageBuffer->tempbuf, 0, 100);
		sprintf(S_ShowPageBuffer->tempbuf, "%s", S_ShowPageBuffer->testdata->sampleid);
		DisText(0x201e, S_ShowPageBuffer->tempbuf, 20);
				
		sprintf(S_ShowPageBuffer->tempbuf, "%2.1f", S_ShowPageBuffer->testdata->TestTemp.O_Temperature);
		DisText(0x202a, S_ShowPageBuffer->tempbuf, 8);
				
		sprintf(S_ShowPageBuffer->tempbuf, "%s", S_ShowPageBuffer->testdata->temperweima.CardPiCi);
		DisText(0x2030, S_ShowPageBuffer->tempbuf, 30);
				
		sprintf(S_ShowPageBuffer->tempbuf, "%.2f", S_ShowPageBuffer->testdata->testline.AdjustResult);
		DisText(0x2040, S_ShowPageBuffer->tempbuf, 30);

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
	
	for(i=0; i<MaxPointLen;i++)
	{
		if(i%10 == 0)
		{
			p = &(S_ShowPageBuffer->testdata->testline.TestPoint[i]);
			DisPlayLine(1 , p , 10);
		}
		
		if(S_ShowPageBuffer->lineinfo.MaxData <= S_ShowPageBuffer->testdata->testline.TestPoint[i])
			S_ShowPageBuffer->lineinfo.MaxData = S_ShowPageBuffer->testdata->testline.TestPoint[i];
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
		
		SetChartSize(0x2070 , S_ShowPageBuffer->lineinfo.MUL_Y);

		DspNum(0x200a , S_ShowPageBuffer->lineinfo.Y_Scale, 2);
		DspNum(0x2009 , S_ShowPageBuffer->lineinfo.Y_Scale*2, 2);

}

static void dspIco(void)
{
	double tempvalue = 0.0;
	if(S_ShowPageBuffer)
	{
		//在曲线上标记出T,C,基线
		S_ShowPageBuffer->myico[0].ICO_ID = 22;
		S_ShowPageBuffer->myico[0].X = 505+S_ShowPageBuffer->testdata->testline.T_Point[1]-5;
		tempvalue = S_ShowPageBuffer->testdata->testline.T_Point[0];
		tempvalue /= S_ShowPageBuffer->lineinfo.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_ShowPageBuffer->myico[0].Y = (unsigned short)tempvalue - 5;
		
		S_ShowPageBuffer->myico[1].ICO_ID = 22;
		S_ShowPageBuffer->myico[1].X = 505+S_ShowPageBuffer->testdata->testline.C_Point[1]-5;
		tempvalue = S_ShowPageBuffer->testdata->testline.C_Point[0];
		tempvalue /= S_ShowPageBuffer->lineinfo.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_ShowPageBuffer->myico[1].Y = (unsigned short)tempvalue - 5;
		
		S_ShowPageBuffer->myico[2].ICO_ID = 22;
		S_ShowPageBuffer->myico[2].X = 505+S_ShowPageBuffer->testdata->testline.B_Point[1]-5;
		tempvalue = S_ShowPageBuffer->testdata->testline.B_Point[0];
		tempvalue /= S_ShowPageBuffer->lineinfo.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_ShowPageBuffer->myico[2].Y = (unsigned short)tempvalue - 5;
		
		BasicUI(0x2060 ,0x1907 , 3, &(S_ShowPageBuffer->myico[0]) , sizeof(Basic_ICO)*3);
	}
}

