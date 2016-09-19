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
						
				S_TempCalData->tempvalue2 += ADS8325();
				
				if(i%AvregeNum == 0)
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
				}
				vTaskDelay(1 / portTICK_RATE_MS);
				
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
	
	{
		memset(&(S_TempCalData->peakdata[0]), 0, 2*sizeof(PeakData));
		
		/*找T线*/
		if(S_TestTaskData->testdata->temperweima.ItemLocation < 40)
		{
			S_TempCalData->resultstatues = LineLocationError;
			return;
		}
		S_TestTaskData->testdata->testline.T_Point[0] = 0;
		for(i=(S_TestTaskData->testdata->temperweima.ItemLocation - 40); i<(S_TestTaskData->testdata->temperweima.ItemLocation + 40); i++)
		{
			if(S_TestTaskData->testdata->testline.T_Point[0] < S_TestTaskData->testdata->testline.TestPoint[i])
			{
				S_TestTaskData->testdata->testline.T_Point[0] = S_TestTaskData->testdata->testline.TestPoint[i];
				S_TestTaskData->testdata->testline.T_Point[1] = i;
				
				S_TempCalData->peakdata[0].PeakLocation = i;
				S_TempCalData->peakdata[0].PeakValue = S_TestTaskData->testdata->testline.T_Point[0];
			}
		}
		
		/*找C线*/
		if(S_TestTaskData->testdata->temperweima.CLineLocation < 40)
		{
			S_TempCalData->resultstatues = LineLocationError;
			return;
		}
		S_TestTaskData->testdata->testline.C_Point[0] = 0;
		for(i=(S_TestTaskData->testdata->temperweima.CLineLocation - 40); i<(S_TestTaskData->testdata->temperweima.CLineLocation + 40); i++)
		{
			if(S_TestTaskData->testdata->testline.C_Point[0] < S_TestTaskData->testdata->testline.TestPoint[i])
			{
				S_TestTaskData->testdata->testline.C_Point[0] = S_TestTaskData->testdata->testline.TestPoint[i];
				S_TestTaskData->testdata->testline.C_Point[1] = i;
				
				S_TempCalData->peakdata[1].PeakLocation = i;
				S_TempCalData->peakdata[1].PeakValue = S_TestTaskData->testdata->testline.C_Point[0];
			}
		}
		
		/*找基线*/
		if(S_TestTaskData->testdata->testline.T_Point[1] >= S_TestTaskData->testdata->testline.C_Point[1])
		{
			S_TempCalData->resultstatues = LineLocationError;
			return;
		}
		S_TestTaskData->testdata->testline.B_Point[0] = 0xffff;
		for(i=S_TestTaskData->testdata->testline.T_Point[1]; i<S_TestTaskData->testdata->testline.C_Point[1]; i++)
		{
			if(S_TestTaskData->testdata->testline.B_Point[0] > S_TestTaskData->testdata->testline.TestPoint[i])
			{
				S_TestTaskData->testdata->testline.B_Point[0] = S_TestTaskData->testdata->testline.TestPoint[i];
				S_TestTaskData->testdata->testline.B_Point[1] = i;
			}
		}
		
		/*找T峰的前半段*/
		for(j=0; j<S_TestTaskData->testdata->testline.T_Point[1]; j++)
		{
			if(S_TestTaskData->testdata->testline.TestPoint[S_TestTaskData->testdata->testline.T_Point[1]-j] == S_TestTaskData->testdata->testline.T_Point[0])
				continue;
			
			S_TempCalData->tempvalue2 = S_TestTaskData->testdata->testline.TestPoint[S_TestTaskData->testdata->testline.T_Point[1]-j] - 
				S_TestTaskData->testdata->testline.TestPoint[S_TestTaskData->testdata->testline.T_Point[1]-j-1];
			if(S_TempCalData->tempvalue2 < 0)
			{
				S_TempCalData->peakdata[0].StartLocation = S_TestTaskData->testdata->testline.T_Point[1]-j;
				S_TempCalData->peakdata[0].StartValue = S_TestTaskData->testdata->testline.TestPoint[S_TempCalData->peakdata[0].StartLocation];
				break;
			}
		}
		
		/*找T峰的后半段*/
		for(j=S_TestTaskData->testdata->testline.T_Point[1]; j<MaxPointLen-1; j++)
		{
			if(S_TestTaskData->testdata->testline.TestPoint[j] == S_TestTaskData->testdata->testline.T_Point[0])
				continue;
			
			S_TempCalData->tempvalue2 = S_TestTaskData->testdata->testline.TestPoint[j] - S_TestTaskData->testdata->testline.TestPoint[j+1];
			if(S_TempCalData->tempvalue2 < 0)
			{
				S_TempCalData->peakdata[0].EndLocation = j;
				S_TempCalData->peakdata[0].EndValue = S_TestTaskData->testdata->testline.TestPoint[S_TempCalData->peakdata[0].EndLocation];
				break;
			}
		}
		
		S_TempCalData->peakdata[0].UpWidth = S_TempCalData->peakdata[0].PeakLocation - S_TempCalData->peakdata[0].StartLocation;
		S_TempCalData->peakdata[0].DownWidth = S_TempCalData->peakdata[0].EndLocation - S_TempCalData->peakdata[0].PeakLocation;
		S_TempCalData->peakdata[0].PeakWidth = S_TempCalData->peakdata[0].EndLocation - S_TempCalData->peakdata[0].StartLocation;
		S_TempCalData->peakdata[0].PeakScale = S_TempCalData->peakdata[0].UpWidth;
		S_TempCalData->peakdata[0].PeakScale /= S_TempCalData->peakdata[0].DownWidth;
		/*一个峰找完，对这个峰进行判断*/
		/*如果峰的宽度小于30或者大于100，则此峰为假*/
		if((S_TempCalData->peakdata[0].PeakWidth < 30)&&(S_TempCalData->peakdata[0].PeakWidth > 150))
		{
			S_TempCalData->peakdata[0].status = 1;			
		}
		
		/*找C峰的前半段*/
		for(j=0; j<S_TestTaskData->testdata->testline.C_Point[1]; j++)
		{
			if(S_TestTaskData->testdata->testline.TestPoint[S_TestTaskData->testdata->testline.C_Point[1]-j] == S_TestTaskData->testdata->testline.C_Point[0])
				continue;
			
			S_TempCalData->tempvalue2 = S_TestTaskData->testdata->testline.TestPoint[S_TestTaskData->testdata->testline.C_Point[1]-j] - 
				S_TestTaskData->testdata->testline.TestPoint[S_TestTaskData->testdata->testline.C_Point[1]-j-1];
			if(S_TempCalData->tempvalue2 < 0)
			{
		
				S_TempCalData->peakdata[1].StartLocation = S_TestTaskData->testdata->testline.C_Point[1]-j;
				S_TempCalData->peakdata[1].StartValue = S_TestTaskData->testdata->testline.TestPoint[S_TempCalData->peakdata[1].StartLocation];
				break;
			}
		}
		
		/*找C峰的后半段*/
		for(j=S_TestTaskData->testdata->testline.C_Point[1]; j<MaxPointLen-1; j++)
		{
			if(S_TestTaskData->testdata->testline.TestPoint[j] == S_TestTaskData->testdata->testline.C_Point[0])
				continue;
			
			S_TempCalData->tempvalue2 = S_TestTaskData->testdata->testline.TestPoint[j] - S_TestTaskData->testdata->testline.TestPoint[j+1];
			if(S_TempCalData->tempvalue2 < 0)
			{
				S_TempCalData->peakdata[1].EndLocation = j;
				S_TempCalData->peakdata[1].EndValue = S_TestTaskData->testdata->testline.TestPoint[S_TempCalData->peakdata[1].EndLocation];
				break;
			}
		}
		
		S_TempCalData->peakdata[1].UpWidth = S_TempCalData->peakdata[1].PeakLocation - S_TempCalData->peakdata[1].StartLocation;
		S_TempCalData->peakdata[1].DownWidth = S_TempCalData->peakdata[1].EndLocation - S_TempCalData->peakdata[1].PeakLocation;
		S_TempCalData->peakdata[1].PeakWidth = S_TempCalData->peakdata[1].EndLocation - S_TempCalData->peakdata[1].StartLocation;
		S_TempCalData->peakdata[1].PeakScale = S_TempCalData->peakdata[1].UpWidth;
		S_TempCalData->peakdata[1].PeakScale /= S_TempCalData->peakdata[1].DownWidth;
		/*一个峰找完，对这个峰进行判断*/
		/*如果峰的宽度小于30或者大于100，则此峰为假*/
		if((S_TempCalData->peakdata[1].PeakWidth < 30)&&(S_TempCalData->peakdata[1].PeakWidth > 150))
		{
			S_TempCalData->peakdata[1].status = 1;			
		}
		
		/*如果没有峰*/
		if((S_TempCalData->peakdata[0].status == 1) && (S_TempCalData->peakdata[1].status == 1))
		{
			S_TempCalData->resultstatues = PeakNumZero;
			return;
		}
		/*如果C峰错误，则次卡异常*/
		else if(S_TempCalData->peakdata[1].status == 1)
		{
			S_TempCalData->resultstatues = PeakNumError;
			return;
		}
		/*如果都正确，根据二维码找峰*/
		else
		{
			//如果T峰不存在，则使其等于基线
			if(S_TempCalData->peakdata[0].status == 1)
			{
				S_TestTaskData->testdata->testline.T_Point[0] = S_TestTaskData->testdata->testline.B_Point[0];
				S_TestTaskData->testdata->testline.T_Point[1] = S_TestTaskData->testdata->testline.B_Point[1];
			}
			/*计算结果*/
			if(((S_TestTaskData->testdata->testline.T_Point[0] < S_TestTaskData->testdata->testline.B_Point[0])) || (S_TestTaskData->testdata->testline.C_Point[0] <= S_TestTaskData->testdata->testline.B_Point[0]))
			{
				S_TempCalData->resultstatues = PeakNumError;
				return;
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
				
			if(S_TestTaskData->testdata->tempadjust.crc == CalModbusCRC16Fun1(&(S_TestTaskData->testdata->tempadjust), sizeof(AdjustData)-2) )
				S_TestTaskData->testdata->testline.AdjustResult =  S_TestTaskData->testdata->testline.BasicResult * S_TestTaskData->testdata->tempadjust.parm;
			else
				S_TestTaskData->testdata->testline.AdjustResult =  S_TestTaskData->testdata->testline.BasicResult;
			
			S_TestTaskData->testdata->testline.AdjustResult = GetCurrentData();
			
			S_TempCalData->resultstatues = ResultIsOK;

		}
	}		
}

