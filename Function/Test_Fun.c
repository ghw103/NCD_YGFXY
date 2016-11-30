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
#include	"BackDoorData.h"

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
		
		SelectChannel(7);
		vTaskDelay(10/portTICK_RATE_MS);
		
		S_TestTaskData = parm;
		
		repeat:
			
			MotorMoveTo(0, 0);
				
			MotorMoveTo(StartTestLocation, 0);
		
			memset(S_TempCalData, 0, sizeof(TempCalData));

			S_TempCalData->maxdata = 0;
			
			for(i=1; i<= steps; i++)
			{
				MotorMoveTo(GetGB_MotorLocation() + 1, 0);
				
				vTaskDelay(1 / portTICK_RATE_MS);
				
				S_TempCalData->tempvalue2 += ADS8325();
				
/*				if(i%AvregeNum == 0)
				{
					S_TempCalData->tempvalue2 /= AvregeNum;
					
					S_TempCalData->tempvalue = (unsigned short)(S_TempCalData->tempvalue2);
					
					if(S_TempCalData->maxdata < S_TempCalData->tempvalue)
						S_TempCalData->maxdata = S_TempCalData->tempvalue;
					
					if(S_TempCalData->maxdata >= 4000)
					{
						if(GetChannel() > 0)
						{
							SelectChannel(GetChannel() - 1);
							goto END1;
						}
					}
					S_TestTaskData->testdata->testline.TestPoint[i/AvregeNum-1] = S_TempCalData->tempvalue;
							
					//SendTestPointData(&(S_TestTaskData->testdata->testline.TestPoint[i/AvregeNum-1]));
						
					S_TempCalData->tempvalue2 = 0;
				}*/
				
				
/*				if(GetCardState() == NoCard)				//卡被拔出
				{
					S_TempCalData->resultstatues = TestInterrupt;
					goto END;
				}*/
			}
			
			vTaskDelay(100/portTICK_RATE_MS);
			
			//分析曲线
			AnalysisTestData();
			
			goto END;
			
			END1:
				vTaskDelay(100/portTICK_RATE_MS);
				//发送一个特定数据，清除曲线
				S_TestTaskData->testdata->testline.TestPoint[0] = 0xffff;
				//SendTestPointData(&(S_TestTaskData->testdata->testline.TestPoint[0]));
				goto repeat;
			
			END:
				S_TestTaskData->testresult = S_TempCalData->resultstatues;
				MotorMoveTo(GetGB_MotorMaxLocation(), 0);
			
				MyFree(S_TempCalData);

				return S_TestTaskData->testresult;
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
	
	if(pdPASS == xQueueReceive( xTestCurveQueue, data, 0*portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;	
}


static void AnalysisTestData(void)
{
	unsigned short i=0, j=0;
	
	
			S_TestTaskData->testdata->testline.AdjustResult = GetCurrentData();
			
			S_TempCalData->resultstatues = ResultIsOK;
	
}

