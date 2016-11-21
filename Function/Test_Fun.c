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
#include	"SDFunction.h"
#include	"Motor_Data.h"
#include	"CardStatues_Data.h"
#include	"MyTest_Data.h"
#include	"CRC16.h"
#include 	"MLX90614_Driver.h"

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
static TestTaskData *S_TestTaskData = NULL;
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
		xTestCurveQueue = xQueueCreate( 1000, ( unsigned portBASE_TYPE ) sizeof( unsigned short ) );
	
	if(xTestCurveQueue == NULL)
		return My_Fail;
	else
		return My_Pass;
}


ResultState TestFunction(void * parm)
{
	unsigned short steps = EndTestLocation - StartTestLocation;
	unsigned short i = 0;
	
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
		
		//初始配置
		SetGB_LedValue(1000);
		vTaskDelay(10/portTICK_RATE_MS);
		
		SetGB_CLineValue(0);
		vTaskDelay(10/portTICK_RATE_MS);
		
		SelectChannel(4);
		vTaskDelay(10/portTICK_RATE_MS);
		
		SMBUS_SCK_L();
		
		S_TestTaskData = parm;
		
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
					
					S_TempCalData->tempvalue = (unsigned short)(S_TempCalData->tempvalue2);
					
					S_TestTaskData->testdata->testline.TestPoint[i/AvregeNum-1] = S_TempCalData->tempvalue;
							
					SendTestPointData(&(S_TestTaskData->testdata->testline.TestPoint[i/AvregeNum-1]));
						
					S_TempCalData->tempvalue2 = 0;
				}
				vTaskDelay(1 / portTICK_RATE_MS);
				
/*				if(GetCardState() == NoCard)				//卡被拔出
				{
					S_TempCalData->resultstatues = TestInterrupt;
					goto END;
				}*/
			}
			
			vTaskDelay(10/portTICK_RATE_MS);
			
			//分析曲线
			AnalysisTestData();
			
			if(S_TempCalData->resultstatues == NoResult)
			{
				//发送一个特定数据，清除曲线
				S_TestTaskData->testdata->testline.TestPoint[0] = 0xffff;
				SendTestPointData(&(S_TestTaskData->testdata->testline.TestPoint[0]));
				goto repeat;
			}

//			END:
				S_TestTaskData->testresult = S_TempCalData->resultstatues;
				MotorMoveTo(GetGB_MotorMaxLocation(), 0);
			
				MyFree(S_TempCalData);
				
				SMBUS_SCK_H();
				
				return S_TestTaskData->testresult;
	}
	else
		return MemError;	
}

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
	
	if(pdPASS == xQueueReceive( xTestCurveQueue, data, 0*portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;	
}


static void AnalysisTestData(void)
{
	unsigned short i=0;
	
	{
		S_TempCalData->maxdata = S_TestTaskData->testdata->testline.TestPoint[0];
		
		/*计算峰递增值*/
		for(i=1; i<MaxPointLen; i++)
		{
			if(S_TempCalData->maxdata < S_TestTaskData->testdata->testline.TestPoint[i])
				S_TempCalData->maxdata = S_TestTaskData->testdata->testline.TestPoint[i];
		}
		
		/*判断测试值是否饱和*/
		if(S_TempCalData->maxdata >= 4000)
		{
			if(GetChannel() > 0)
			{
				SelectChannel(GetChannel() - 1);

				vTaskDelay(10/portTICK_RATE_MS);
				return;
			}
		}
		else if(S_TempCalData->maxdata < 500)
		{
			if(GetChannel() < 7)
			{
				SelectChannel(GetChannel() + 3);

				vTaskDelay(10/portTICK_RATE_MS);
				return;
			}
		}

		//找T线
		S_TestTaskData->testdata->testline.T_Point[0] = 0;
		for(i=S_TestTaskData->testdata->temperweima.ItemLocation-25; i<S_TestTaskData->testdata->temperweima.ItemLocation+25; i++)
		{
			if(S_TestTaskData->testdata->testline.T_Point[0] < S_TestTaskData->testdata->testline.TestPoint[i])
			{
				S_TestTaskData->testdata->testline.T_Point[0] = S_TestTaskData->testdata->testline.TestPoint[i];
				S_TestTaskData->testdata->testline.T_Point[1] = i;
			}
		}
		
		//找c线
		S_TestTaskData->testdata->testline.C_Point[0] = 0;
		for(i=S_TestTaskData->testdata->temperweima.CLineLocation-25; i<S_TestTaskData->testdata->temperweima.CLineLocation+25; i++)
		{
			if(S_TestTaskData->testdata->testline.C_Point[0] < S_TestTaskData->testdata->testline.TestPoint[i])
			{
				S_TestTaskData->testdata->testline.C_Point[0] = S_TestTaskData->testdata->testline.TestPoint[i];
				S_TestTaskData->testdata->testline.C_Point[1] = i;
			}
		}
		
/*		//判断c线的峰
		for(i=S_TestTaskData->testdata->testline.C_Point[1]-5; i<S_TestTaskData->testdata->testline.C_Point[1]; i++)
		{
			if(S_TestTaskData->testdata->testline.TestPoint[i] > S_TestTaskData->testdata->testline.TestPoint[i+1])
			{
				S_TempCalData->resultstatues = PeakNumError;
				return;
			}
		}
		for(i=S_TestTaskData->testdata->testline.C_Point[1]; i<S_TestTaskData->testdata->testline.C_Point[1]+5; i++)
		{
			if(S_TestTaskData->testdata->testline.TestPoint[i] < S_TestTaskData->testdata->testline.TestPoint[i+1])
			{
				S_TempCalData->resultstatues = PeakNumError;
				return;
			}
		}*/
		
		
		//找基线
		S_TestTaskData->testdata->testline.B_Point[0] = 0xffff;
		for(i=S_TestTaskData->testdata->testline.T_Point[1]; i<S_TestTaskData->testdata->testline.C_Point[1]; i++)
		{
			if(S_TestTaskData->testdata->testline.B_Point[0] > S_TestTaskData->testdata->testline.TestPoint[i])
			{
				S_TestTaskData->testdata->testline.B_Point[0] = S_TestTaskData->testdata->testline.TestPoint[i];
				S_TestTaskData->testdata->testline.B_Point[1] = i;
			}
		}
				
		/*计算结果*/
		S_TempCalData->tempvalue2 = (S_TestTaskData->testdata->testline.T_Point[0] - S_TestTaskData->testdata->testline.B_Point[0]);
		S_TempCalData->tempvalue2 /= (S_TestTaskData->testdata->testline.C_Point[0] - S_TestTaskData->testdata->testline.B_Point[0]);
				
		/*原始峰高比*/
		S_TestTaskData->testdata->testline.BasicBili = S_TempCalData->tempvalue2;
				
		/*根据分段，计算原始结果*/
		if((S_TestTaskData->testdata->testline.BasicBili < S_TestTaskData->testdata->temperweima.ItemFenDuan) || (S_TestTaskData->testdata->temperweima.ItemBiaoQuNum == 1))
		{
			S_TestTaskData->testdata->testline.BasicResult = S_TestTaskData->testdata->testline.BasicBili * S_TestTaskData->testdata->testline.BasicBili;
			S_TestTaskData->testdata->testline.BasicResult *= S_TestTaskData->testdata->temperweima.ItemBiaoQu[0][0];
					
			S_TestTaskData->testdata->testline.BasicResult += (S_TestTaskData->testdata->testline.BasicBili * S_TestTaskData->testdata->temperweima.ItemBiaoQu[0][1]);
					
			S_TestTaskData->testdata->testline.BasicResult += S_TestTaskData->testdata->temperweima.ItemBiaoQu[0][2];
		}
		else
		{
			S_TestTaskData->testdata->testline.BasicResult = S_TestTaskData->testdata->testline.BasicBili * S_TestTaskData->testdata->testline.BasicBili;
			S_TestTaskData->testdata->testline.BasicResult *= S_TestTaskData->testdata->temperweima.ItemBiaoQu[1][0];
					
			S_TestTaskData->testdata->testline.BasicResult += (S_TestTaskData->testdata->testline.BasicBili * S_TestTaskData->testdata->temperweima.ItemBiaoQu[1][1]);
					
			S_TestTaskData->testdata->testline.BasicResult += S_TestTaskData->testdata->temperweima.ItemBiaoQu[1][2];
		}
				
		if(S_TestTaskData->testdata->testline.BasicResult < 0)
			S_TestTaskData->testdata->testline.BasicResult = 0;
			
		if(S_TestTaskData->testdata->tempadjust.crc == CalModbusCRC16Fun1(&(S_TestTaskData->testdata->tempadjust), sizeof(AdjustData)-2) )
			S_TestTaskData->testdata->testline.AdjustResult =  S_TestTaskData->testdata->testline.BasicResult * S_TestTaskData->testdata->tempadjust.parm;
		else
			S_TestTaskData->testdata->testline.AdjustResult =  S_TestTaskData->testdata->testline.BasicResult;
		
		S_TempCalData->resultstatues = ResultIsOK;
	}		
}

