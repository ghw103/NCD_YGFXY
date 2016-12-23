/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"PreReadCardPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"MyTest_Data.h"
#include 	"MLX90614_Driver.h"
#include	"LunchPage.h"
#include	"CodeScan_Task.h"
#include	"WaittingCardPage.h"
#include	"TimeDownNorPage.h"
#include	"CodeScanFunction.h"
#include	"PaiDuiPage.h"
#include	"CardStatues_Data.h"
#include	"Motor_Fun.h"
#include	"PlaySong_Task.h"
#include	"Test_Task.h"
#include	"SDFunction.h"
#include	"MyTools.h"
#include	"CRC16.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static PreReadPageBuffer * S_PreReadPageBuffer = NULL;
const unsigned int TestLineHigh1 = 77010;	//此数据与曲线显示区域高度有关，如果界面不改，此数不改
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

static void CheckQRCode(void);
static void ShowCardInfo(void);
static void CheckPreTestCard(void);
static void RefreshCurve(void);
static void AddDataToLine(unsigned short data);
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
MyState_TypeDef createPreReadCardActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "PreReadCardActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_PreReadPageBuffer)
	{
		/*清除界面*/
		ClearText(0x1420, 30);
		ClearText(0x1430, 30);
		ClearText(0x1440, 10);
		ClearText(0x1450, 30);
		
		S_PreReadPageBuffer->currenttestdata = GetCurrentTestItem();
		
		AddNumOfSongToList(44, 0);
		
		StartScanQRCode(&(S_PreReadPageBuffer->temperweima));
	}
	
	SelectPage(92);
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
	if(S_PreReadPageBuffer)
	{
		/*命令*/
		S_PreReadPageBuffer->lcdinput[0] = pbuf[4];
		S_PreReadPageBuffer->lcdinput[0] = (S_PreReadPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*二维码读取失败，过期，已使用*/
		if((S_PreReadPageBuffer->lcdinput[0] >= 0x1400) && (S_PreReadPageBuffer->lcdinput[0] <= 0x1404))
		{
			/*数据*/
			S_PreReadPageBuffer->lcdinput[1] = pbuf[7];
			S_PreReadPageBuffer->lcdinput[1] = (S_PreReadPageBuffer->lcdinput[1]<<8) + pbuf[8];
			
			/*更换检测卡*/
			if(S_PreReadPageBuffer->lcdinput[1] == 0x0001)
			{
				backToFatherActivity();
			}
			/*退出*/
			else if(S_PreReadPageBuffer->lcdinput[1] == 0x0000)
			{
				DeleteCurrentTest();
				
				backToActivity(lunchActivityName);
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
	if(S_PreReadPageBuffer)
	{
		CheckQRCode();
	
		

		RefreshCurve();
		
		CheckPreTestCard();
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
	if(NULL == S_PreReadPageBuffer)
	{
		S_PreReadPageBuffer = MyMalloc(sizeof(PreReadPageBuffer));
		
		if(S_PreReadPageBuffer)
		{
			memset(S_PreReadPageBuffer, 0, sizeof(PreReadPageBuffer));
	
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
	MyFree(S_PreReadPageBuffer);
	S_PreReadPageBuffer = NULL;
}



static void CheckQRCode(void)
{
	if((S_PreReadPageBuffer) && (My_Pass == TakeScanQRCodeResult(&(S_PreReadPageBuffer->scancode))))
	{	
		ClearLine(0x56);
		if((S_PreReadPageBuffer->scancode == CardCodeScanFail) || (S_PreReadPageBuffer->scancode == CardCodeCardOut) ||
			(S_PreReadPageBuffer->scancode == CardCodeScanTimeOut) || (S_PreReadPageBuffer->scancode == CardCodeCRCError))
		{
			SendKeyCode(1);
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(23, 0);
		}
		else if(S_PreReadPageBuffer->scancode == CardCodeTimeOut)
		{
			SendKeyCode(4);
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(21, 0);
		}
		else if(S_PreReadPageBuffer->scancode == CardCodeScanOK)
		{
			//获取检测卡温度
			S_PreReadPageBuffer->currenttestdata->testdata.TestTemp.O_Temperature = GetCardTemperature();
			
			ShowCardInfo();
			
			//如果测试数据包中的二维码crc校验错误，则表明是第一次读取二维码
			if(S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CRC16 == 0)
			{
				//将读取的二维码数据拷贝到测试数据包中
				memcpy(&(S_PreReadPageBuffer->currenttestdata->testdata.temperweima), &(S_PreReadPageBuffer->temperweima), sizeof(CardCodeInfo));
				
				//设置倒计时时间
				timer_set(&(S_PreReadPageBuffer->currenttestdata->timer), S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardWaitTime*60);
			
				//读取校准参数
				memcpy(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust.ItemName, S_PreReadPageBuffer->currenttestdata->testdata.temperweima.ItemName, ItemNameLen);
				if(My_Fail == ReadAdjustData(&(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust)))
					memset(&(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust), 0, sizeof(AdjustData));
				
				StartTest(&(S_PreReadPageBuffer->currenttestdata->testdata));
			}
			else
			{
				//校验试机卡编号
				if(pdFAIL == CheckStrIsSame(S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardPiCi, S_PreReadPageBuffer->temperweima.CardPiCi, CardPiCiHaoLen))
				{
					SendKeyCode(2);
					MotorMoveTo(MaxLocation, 1);
				}
				else
				{
					startActivity(createTimeDownActivity, NULL);
				}
			}
		}
	}
}

static void CheckPreTestCard(void)
{
	if((S_PreReadPageBuffer) && (My_Pass == TakeTestResult(&(S_PreReadPageBuffer->cardpretestresult))))
	{
		ShowCardInfo();
		
		if(S_PreReadPageBuffer->cardpretestresult == TestInterrupt)
		{
			SendKeyCode(1);
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(22, 0);
			memset(&(S_PreReadPageBuffer->currenttestdata->testdata.temperweima), 0, sizeof(CardCodeInfo));
		}
		else if(S_PreReadPageBuffer->cardpretestresult == NoSample)
		{
			SendKeyCode(5);
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(22, 0);
			memset(&(S_PreReadPageBuffer->currenttestdata->testdata.temperweima), 0, sizeof(CardCodeInfo));
		}
		else if(S_PreReadPageBuffer->cardpretestresult == PeakError)
		{
			//如果是排队模式，则进入排队界面
			if(S_PreReadPageBuffer->currenttestdata->testlocation > 0)
			{
				MotorMoveTo(MaxLocation, 1);
				
				S_PreReadPageBuffer->currenttestdata->statues = startpaidui;
				timer_restart(&(S_PreReadPageBuffer->currenttestdata->timer));

				startActivity(createPaiDuiActivity, NULL);
			}
			else
			{		
				timer_restart(&(S_PreReadPageBuffer->currenttestdata->timer));
				
				startActivity(createTimeDownActivity, NULL);
			}
		}
		else if(S_PreReadPageBuffer->cardpretestresult == ResultIsOK)
		{
			SendKeyCode(3);
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(22, 0);
			memset(&(S_PreReadPageBuffer->currenttestdata->testdata.temperweima), 0, sizeof(CardCodeInfo));
		}
	}
}

static void ShowCardInfo(void)
{
	double tempvalue = 0.0;
	
	sprintf(S_PreReadPageBuffer->buf, "%s", S_PreReadPageBuffer->temperweima.ItemName);
	DisText(0x1420, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
			
	sprintf(S_PreReadPageBuffer->buf, "%s", S_PreReadPageBuffer->temperweima.CardPiCi);
	DisText(0x1430, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
			
	sprintf(S_PreReadPageBuffer->buf, "%d", S_PreReadPageBuffer->temperweima.CardWaitTime*60);
	DisText(0x1440, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
			
	sprintf(S_PreReadPageBuffer->buf, "20%02d年%02d月%02d日", S_PreReadPageBuffer->temperweima.CardBaoZhiQi.year, S_PreReadPageBuffer->temperweima.CardBaoZhiQi.month,
		S_PreReadPageBuffer->temperweima.CardBaoZhiQi.day);
	DisText(0x1450, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf));
	
	memset(S_PreReadPageBuffer->buf, 0, 40);
	sprintf(S_PreReadPageBuffer->buf, "cv1=%.3f", S_PreReadPageBuffer->currenttestdata->testdata.testline.BasicBili);
//	sprintf(S_PreReadPageBuffer->buf, "%2.1f", S_PreReadPageBuffer->currenttestdata->testdata.TestTemp.O_Temperature);
	DisText(0x1460, S_PreReadPageBuffer->buf, 20);
	
	memset(S_PreReadPageBuffer->buf, 0, 40);
	sprintf(S_PreReadPageBuffer->buf, "cv2=%.3f",S_PreReadPageBuffer->currenttestdata->testdata.testline.BasicResult);
	DisText(0x1470, S_PreReadPageBuffer->buf, 20);
	
	if(S_PreReadPageBuffer->cardpretestresult == ResultIsOK)
	{
		//在曲线上标记出T,C,基线
		S_PreReadPageBuffer->myico[0].ICO_ID = 22;
		S_PreReadPageBuffer->myico[0].X = 505+S_PreReadPageBuffer->currenttestdata->testdata.testline.T_Point[1]-5;
		tempvalue = S_PreReadPageBuffer->currenttestdata->testdata.testline.T_Point[0];
		tempvalue /= S_PreReadPageBuffer->line.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_PreReadPageBuffer->myico[0].Y = (unsigned short)tempvalue - 5;
		
		S_PreReadPageBuffer->myico[1].ICO_ID = 22;
		S_PreReadPageBuffer->myico[1].X = 505+S_PreReadPageBuffer->currenttestdata->testdata.testline.C_Point[1]-5;
		tempvalue = S_PreReadPageBuffer->currenttestdata->testdata.testline.C_Point[0];
		tempvalue /= S_PreReadPageBuffer->line.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_PreReadPageBuffer->myico[1].Y = (unsigned short)tempvalue - 5;
		
		S_PreReadPageBuffer->myico[2].ICO_ID = 22;
		S_PreReadPageBuffer->myico[2].X = 505+S_PreReadPageBuffer->currenttestdata->testdata.testline.B_Point[1]-5;
		tempvalue = S_PreReadPageBuffer->currenttestdata->testdata.testline.B_Point[0];
		tempvalue /= S_PreReadPageBuffer->line.Y_Scale*2;
		tempvalue = 1-tempvalue;
		tempvalue *= 302;										//曲线窗口宽度
		tempvalue += 139;										//曲线窗口起始y
		S_PreReadPageBuffer->myico[2].Y = (unsigned short)tempvalue - 5;
		
		BasicUI(0x1880 ,0x1907 , 3, &(S_PreReadPageBuffer->myico[0]) , sizeof(Basic_ICO)*3);
	}
	else
	{
		memset(S_PreReadPageBuffer->myico, 0, sizeof(Basic_ICO)*3);
		BasicUI(0x1880 ,0x1907 , 3, &S_PreReadPageBuffer->myico[0] , sizeof(Basic_ICO)*3);
	}
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
			S_PreReadPageBuffer->line.MaxData = 0;
			S_PreReadPageBuffer->line.MUL_Y = 1;
			S_PreReadPageBuffer->line.Y_Scale = 100;
				
			SetChartSize(0x1870 , S_PreReadPageBuffer->line.MUL_Y);
		}
		else
			AddDataToLine(temp);
	}

}


static void AddDataToLine(unsigned short data)
{
	unsigned short tempdata = data;
	double TempMul_Y2;
	double TempY_Scale;
	unsigned short tempvalue;
	
	if(S_PreReadPageBuffer->line.MaxData <= tempdata)
	{
		S_PreReadPageBuffer->line.MaxData = tempdata;

		////////////////////////针对当前曲线最大值计算y轴放大倍数//////////////////////////////////////
		TempMul_Y2 = TestLineHigh1;
		TempMul_Y2 /= S_PreReadPageBuffer->line.MaxData;
		TempMul_Y2 *= 0.8;			//*0.8是将最大值缩放到满刻度的0.8高度处

		
		tempvalue = (unsigned short)(TempMul_Y2*10);
		S_PreReadPageBuffer->line.MUL_Y = ((tempvalue%10) > 5)?(tempvalue/10 + 1):(tempvalue/10);
		
		if(S_PreReadPageBuffer->line.MUL_Y < 1)			//最小值为1
			S_PreReadPageBuffer->line.MUL_Y = 1;

		/////////////////////////针对当前放大倍数，计算y轴刻度递增基数/////////////////////////////////////
		TempY_Scale = TestLineHigh1 ;
		TempY_Scale /= S_PreReadPageBuffer->line.MUL_Y;
		TempY_Scale /= 2.0;																//目前显示2个y轴刻度
		S_PreReadPageBuffer->line.Y_Scale = (unsigned short)TempY_Scale;
		
		SetChartSize(0x1870 , S_PreReadPageBuffer->line.MUL_Y);


	}
	DisPlayLine(0 , &tempdata , 1);

}
