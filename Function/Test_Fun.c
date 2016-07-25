/***************************************************************************************************
*FileName:TestFun
*Description:??
*Author:xsx
*Data:2016?5?14?17:09:53
***************************************************************************************************/

/***************************************************************************************************/
/******************************************???***************************************************/
/***************************************************************************************************/
#include	"Test_Fun.h"

#include	"Ads8325_Driver.h"
#include	"TLV5617_Driver.h"
#include	"MAX4051_Driver.h"
#include	"DRV8825_Driver.h"
#include	"Motor_Fun.h"

#include	"Motor_Data.h"
#include	"CardStatues_Data.h"
#include	"MyTest_Data.h"

#include	"MyMem.h"

#include	"QueueUnits.h"
#include	"ServerFun.h"

#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"
#include	"stdlib.h"
/***************************************************************************************************/
/**************************************??????*************************************************/
/***************************************************************************************************/
static xQueueHandle xTestCurveQueue = NULL;												//发送测试曲线

static TempCalData * S_TempCalData = NULL;												//计算数据空间
static TestData * S_TestData = NULL;													//测试数据保存空间
/***************************************************************************************************/
/**************************************??????*************************************************/
/***************************************************************************************************/
static MyState_TypeDef SendTestPointData(void * data);
static void AnalysisTestData(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************??********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

MyState_TypeDef InitTestFunData(void)
{
	/*曲线队列*/
	if(xTestCurveQueue == NULL)
		xTestCurveQueue = xQueueCreate( 100, ( unsigned portBASE_TYPE ) sizeof( unsigned short ) );
	
	if(xTestCurveQueue == NULL)
		return My_Fail;
	else
		return My_Pass;
}


ResultState TestFunction(void * parm)
{
	unsigned short steps = EndTestLocation - StartTestLocation;
	unsigned short i = 0;
	ResultState result = NoResult;
	
	if(parm == NULL)
		return MemError;
	
	/*初始化测试曲线队列*/
	if(xTestCurveQueue == NULL)
		return MemError;
	while(My_Pass == TakeTestPointData(&i))
		;
	
	S_TempCalData = MyMalloc(sizeof(TempCalData));
	
	if(S_TempCalData)
	{
		memset(S_TempCalData, 0, sizeof(TempCalData));
		
		/*初始配置*/
		SetGB_LedValue(300);
		vTaskDelay(10/portTICK_RATE_MS);
		
		SetGB_CLineValue(0);
		vTaskDelay(10/portTICK_RATE_MS);
		
		SelectChannel(1);
		vTaskDelay(10/portTICK_RATE_MS);
		
		S_TestData = parm;
		
		repeat:
				
			MotorMoveTo(0, 0);
				
			MotorMoveTo(StartTestLocation, 0);
		
			memset(S_TempCalData, 0, sizeof(TempCalData));

			for(i=1; i<= steps; i++)
			{
				MotorMoveTo(GetGB_MotorLocation() + 1, 0);
						
				S_TempCalData->tempvalue2 += ADS8325();
				
				if(i%AvregeNum == 0)
				{
					S_TempCalData->tempvalue2 /= AvregeNum;
					S_TempCalData->tempvalue = (unsigned short)S_TempCalData->tempvalue2;
					
					S_TestData->testline.TestPoint[i/AvregeNum-1] = S_TempCalData->tempvalue;
							
					SendTestPointData(&(S_TestData->testline.TestPoint[i/AvregeNum-1]));
						
					S_TempCalData->tempvalue2 = 0;
				}
				vTaskDelay(1 / portTICK_RATE_MS);
				
/*				if(GetCardState() == NoCard)				//卡被拔出
				{
					S_TempCalData->resultstatues = TestInterrupt;
					goto END1;
				}*/
			}
			
			vTaskDelay(1000/portTICK_RATE_MS);
			
			/*分析曲线*/
			AnalysisTestData();
			
			if(S_TempCalData->resultstatues == NoResult)
			{
				/*发送一个特定数据，清除曲线*/
				S_TestData->testline.TestPoint[0] = 0xffff;
				SendTestPointData(&(S_TestData->testline.TestPoint[0]));
				goto repeat;
			}

			result = S_TempCalData->resultstatues;
			MotorMoveTo(MaxLocation, 0);
			
			MyFree(S_TempCalData);
			S_TempCalData = NULL;
			
			return result;
	}
	else
		return MemError;	
}

/*????*/
static MyState_TypeDef SendTestPointData(void * data)
{
	if(xTestCurveQueue == NULL)
		return My_Fail;
	
	if(pdPASS == xQueueSend( xTestCurveQueue, data, 10*portTICK_RATE_MS ))
		return My_Pass;
	else
		return My_Fail;	
}

MyState_TypeDef TakeTestPointData(void * data)
{
	if(xTestCurveQueue == NULL)
		return My_Fail;
	
	if(pdPASS == xQueueReceive( xTestCurveQueue, data, 10*portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;	
}


static void AnalysisTestData(void)
{
	unsigned short i=0;
	

	{
		/*找出最大值和最小值*/
		S_TempCalData->MaxPoint = 0;
		S_TempCalData->MaxPointNum = 0;
		S_TempCalData->MinPoint = 0xffff;
		S_TempCalData->MinPointNum = 0;
		for(i=0; i<MaxPointLen; i++)
		{
			if(S_TempCalData->MaxPoint < S_TestData->testline.TestPoint[i])
			{
				S_TempCalData->MaxPoint = S_TestData->testline.TestPoint[i];
				S_TempCalData->MaxPointNum = 0;
			}
			else if(S_TempCalData->MaxPoint == S_TestData->testline.TestPoint[i])
				S_TempCalData->MaxPointNum++;
			
			if(S_TempCalData->MinPoint > S_TestData->testline.TestPoint[i])
			{
				S_TempCalData->MinPoint = S_TestData->testline.TestPoint[i];
				S_TempCalData->MinPointNum = 0;
			}
			else if(S_TempCalData->MinPoint == S_TestData->testline.TestPoint[i])
				S_TempCalData->MinPointNum++;
		}
		
		
		/*计算峰递增值*/
		for(i=0; i<MaxPointLen; i++)
		{
			if(i >= 1)
				S_TempCalData->testline2[i-1] = S_TestData->testline.TestPoint[i] - S_TestData->testline.TestPoint[i-1];
		}

		/*寻找所有峰*/
		for(i=0; i<MaxPointLen-1; i++)
		{
			if((S_TempCalData->peakdata[S_TempCalData->peaknum].step == 0) && (S_TempCalData->testline2[i] > 0))
			{
				S_TempCalData->peakdata[S_TempCalData->peaknum].StartLocation = i;
				S_TempCalData->peakdata[S_TempCalData->peaknum].StartValue = S_TestData->testline.TestPoint[i];
				S_TempCalData->peakdata[S_TempCalData->peaknum].step = 1;
			}
			else if((S_TempCalData->peakdata[S_TempCalData->peaknum].step == 1) && (S_TempCalData->testline2[i] <= 0))
			{
				S_TempCalData->peakdata[S_TempCalData->peaknum].PeakLocation = i;
				S_TempCalData->peakdata[S_TempCalData->peaknum].PeakValue = S_TestData->testline.TestPoint[i];
				S_TempCalData->peakdata[S_TempCalData->peaknum].UpWidth = i - S_TempCalData->peakdata[S_TempCalData->peaknum].StartLocation;
				S_TempCalData->peakdata[S_TempCalData->peaknum].step = 2;
			}
			else if((S_TempCalData->peakdata[S_TempCalData->peaknum].step == 2) && (S_TempCalData->testline2[i] >= 0))
			{
				S_TempCalData->peakdata[S_TempCalData->peaknum].EndLocation = i;
				S_TempCalData->peakdata[S_TempCalData->peaknum].EndValue = S_TestData->testline.TestPoint[i];
				S_TempCalData->peakdata[S_TempCalData->peaknum].DownWidth = i - S_TempCalData->peakdata[S_TempCalData->peaknum].PeakLocation;
				S_TempCalData->peakdata[S_TempCalData->peaknum].PeakWidth = i - S_TempCalData->peakdata[S_TempCalData->peaknum].StartLocation;
				S_TempCalData->peakdata[S_TempCalData->peaknum].PeakScale = S_TempCalData->peakdata[S_TempCalData->peaknum].UpWidth;
				S_TempCalData->peakdata[S_TempCalData->peaknum].PeakScale /= S_TempCalData->peakdata[S_TempCalData->peaknum].DownWidth;
				S_TempCalData->peakdata[S_TempCalData->peaknum].step = 3;
				
				/*一个峰找完，对这个峰进行判断*/
				/*如果峰的宽度小于30或者大于100，则此峰为假*/
				if((S_TempCalData->peakdata[S_TempCalData->peaknum].PeakWidth < 30) || (S_TempCalData->peakdata[S_TempCalData->peaknum].PeakWidth > 100))
				{
					S_TempCalData->peakdata[S_TempCalData->peaknum].step = 0;
				}
				/*如果起峰宽度大于落峰，且大于2倍，则峰为假*/
				else if(S_TempCalData->peakdata[S_TempCalData->peaknum].PeakScale > 1.5)
					S_TempCalData->peakdata[S_TempCalData->peaknum].step = 0;
				/*如果起峰宽度小于落峰，且小于0.5，则峰为假*/
				else if(S_TempCalData->peakdata[S_TempCalData->peaknum].PeakScale < 0.5)
					S_TempCalData->peakdata[S_TempCalData->peaknum].step = 0;
				/*如果判断条件都通过，则峰为真*/
				else
				{				
					S_TempCalData->peaknum++;
				}
			}
		}
		
		/*如果没有峰*/
		if(S_TempCalData->peaknum == 0)
		{
			S_TempCalData->resultstatues = PeakNumZero;
			return;
		}
		/*如果只有一个峰*/
		else if(S_TempCalData->peaknum == 1)
		{
			S_TempCalData->resultstatues = PeakNumError;
			return;
		}
		/*如果都正确，根据二维码找峰*/
		else
		{
			
			S_TempCalData->tempvalue2 = S_TempCalData->MinPoint;
			S_TempCalData->tempvalue2 /= S_TempCalData->MaxPoint;
			
			/*判断测试值是否饱和*/
			if(S_TempCalData->MaxPoint >= 4000)
			{
				if(GetChannel() > 0)
				{
					SelectChannel(GetChannel() - 1);
					vTaskDelay(10/portTICK_RATE_MS);
					return;
				}
			}
			/*测试值偏小，需要放大*/
			else if(S_TempCalData->MaxPoint < 1000)
			{
				if(GetChannel() < 7)
				{
					SelectChannel(GetChannel() + 3);
					vTaskDelay(10/portTICK_RATE_MS);
					return;
				}
			}
		
			for(i=0; i<S_TempCalData->peaknum; i++)
			{
				S_TempCalData->tempvalue2 = S_TempCalData->peakdata[i].PeakLocation - S_TestData->temperweima.ItemLocation;
				if(abs(S_TempCalData->tempvalue2) < 25)
				{
					S_TestData->testline.T_Point[0] = S_TempCalData->peakdata[i].PeakValue;
					S_TestData->testline.T_Point[1] = S_TempCalData->peakdata[i].PeakLocation;
				}
				else
				{
					S_TempCalData->tempvalue2 = S_TempCalData->peakdata[i].PeakLocation - S_TestData->temperweima.CLineLocation;
					if(abs(S_TempCalData->tempvalue2) < 25)
					{
						S_TestData->testline.C_Point[0] = S_TempCalData->peakdata[i].PeakValue;
						S_TestData->testline.C_Point[1] = S_TempCalData->peakdata[i].PeakLocation;
					}
				}	
			}
			
			/*找到t和c线，找基线*/
			S_TestData->testline.B_Point[0] = 0xffff;
			if((S_TestData->testline.T_Point[1] != 0) && (S_TestData->testline.C_Point[1] != 0) && (S_TestData->testline.C_Point[1] != S_TestData->testline.T_Point[1]))
			{
				for(i=S_TestData->testline.T_Point[1]; i<S_TestData->testline.C_Point[1]; i++)
				{
					if(S_TestData->testline.B_Point[0] > S_TestData->testline.TestPoint[i])
					{
						S_TestData->testline.B_Point[0] = S_TestData->testline.TestPoint[i];
						S_TestData->testline.B_Point[1] = i;
					}
				}
				
				/*计算结果*/
				S_TempCalData->tempvalue2 = (S_TestData->testline.T_Point[0] - S_TestData->testline.B_Point[0]);
				S_TempCalData->tempvalue2 /= (S_TestData->testline.C_Point[0] - S_TestData->testline.B_Point[0]);
				
				/*原始峰高比*/
				S_TestData->testline.BasicBili = S_TempCalData->tempvalue2;
				
				/*根据分段，计算原始结果*/
				if(S_TestData->testline.BasicBili < S_TestData->temperweima.ItemFenDuan)
				{
					S_TestData->testline.BasicResult = S_TestData->testline.BasicBili * S_TestData->testline.BasicBili;
					S_TestData->testline.BasicResult *= S_TestData->temperweima.ItemBiaoQu[0][0];
					
					S_TestData->testline.BasicResult += (S_TestData->testline.BasicBili * S_TestData->temperweima.ItemBiaoQu[0][1]);
					
					S_TestData->testline.BasicResult += S_TestData->temperweima.ItemBiaoQu[0][2];
				}
				else
				{
					S_TestData->testline.BasicResult = S_TestData->testline.BasicBili * S_TestData->testline.BasicBili;
					S_TestData->testline.BasicResult *= S_TestData->temperweima.ItemBiaoQu[1][0];
					
					S_TestData->testline.BasicResult += (S_TestData->testline.BasicBili * S_TestData->temperweima.ItemBiaoQu[1][1]);
					
					S_TestData->testline.BasicResult += S_TestData->temperweima.ItemBiaoQu[1][2];
				}
				
				S_TestData->testline.AdjustResult =  S_TestData->testline.BasicResult;
				
				S_TempCalData->resultstatues = ResultIsOK;
			}
			else
			{
				S_TempCalData->resultstatues = PeakNumError;
				return;
			}
		}
	}		
}

