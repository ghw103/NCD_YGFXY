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
#include	"math.h"
/***************************************************************************************************/
/**************************************??????*************************************************/
/***************************************************************************************************/
static xQueueHandle xTestCurveQueue = NULL;												//发送测试曲线
/***************************************************************************************************/
/**************************************??????*************************************************/
/***************************************************************************************************/
static MyState_TypeDef SendTestPointData(void * data);
static void AnalysisTestData(TempCalData * S_TempCalData);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************??********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: InitTestFunData
*Description: 创建测试曲线的队列
*Input: None
*Output: None
*Author: xsx
*Date: 2016年11月29日09:14:43
***************************************************************************************************/
MyState_TypeDef InitTestFunData(void)
{
	/*曲线队列*/
	if(xTestCurveQueue == NULL)
		xTestCurveQueue = xQueueCreate( 500, ( unsigned portBASE_TYPE ) sizeof( unsigned short ) );
	
	if(xTestCurveQueue == NULL)
		return My_Fail;
	else
		return My_Pass;
}

/***************************************************************************************************
*FunctionName: SendTestPointData
*Description: 发送测试曲线点到队列中，曲线点包含y坐标信息
*Input: data -- 数据地址
*Output: None
*Return: 发送成功返回my_pass
*			发送超时，或者队列满，返回my_fail
*Author: xsx
*Date: 2016年11月29日09:18:33
***************************************************************************************************/
static MyState_TypeDef SendTestPointData(void * data)
{
	if(xTestCurveQueue == NULL)
		return My_Fail;
	
	if(pdPASS == xQueueSend( xTestCurveQueue, data, 10*portTICK_RATE_MS ))
		return My_Pass;
	else
		return My_Fail;	
}

/***************************************************************************************************
*FunctionName: TakeTestPointData
*Description: 从测试曲线队列中读取数据
*Input: 读取数据缓存地址
*Output: None
*Return: 	My_Pass -- 成功读取到数据
			My_Fail -- 读取失败
*Author: xsx
*Date: 
***************************************************************************************************/
MyState_TypeDef TakeTestPointData(void * data)
{
	if(xTestCurveQueue == NULL)
		return My_Fail;
	
	if(pdPASS == xQueueReceive( xTestCurveQueue, data, 0*portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;	
}

/***************************************************************************************************
*FunctionName: 
*Description: 
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
ResultState TestFunction(void * parm)
{
	unsigned short steps = EndTestLocation - StartTestLocation;
	unsigned short i = 0, j=0;
	unsigned short index;
	TempCalData * S_TempCalData = NULL;															//测试过程中使用的变量
	ResultState S_ResultState = NoResult;
	
	if(parm == NULL)
		return MemError;
	
	/*初始化测试曲线队列*/
	if(InitTestFunData() == My_Fail)
		return MemError;
	while(My_Pass == TakeTestPointData(&i))
		;
	
	S_TempCalData = MyMalloc(sizeof(TempCalData));
	
	if(S_TempCalData)
	{
		memset(S_TempCalData, 0, sizeof(TempCalData));
		
		//初始配置
		SetGB_LedValue(300);
		vTaskDelay(10/portTICK_RATE_MS);
		
		SetGB_CLineValue(0);
		vTaskDelay(10/portTICK_RATE_MS);
		
		SelectChannel(4);
		vTaskDelay(10/portTICK_RATE_MS);
		
		SMBUS_SCK_L();
		
		//测试数据指针指向传进来的真实数据空间
		S_TempCalData->testdata = parm;
		
		repeat:
				
			MotorMoveTo(0, 0);
				
			MotorMoveTo(StartTestLocation, 0);
		
			S_TempCalData->resultstatues = NoResult;
			
			for(i=1; i<= steps; i++)
			{
				MotorMoveTo(GetGB_MotorLocation() + 1, 0);
				
				vTaskDelay(1 / portTICK_RATE_MS);				
				S_TempCalData->tempvalue1 += ADS8325();
				
				//平均值滤波
				if(i%AvregeNum == 0)
				{
					index = i/AvregeNum;
					
					S_TempCalData->tempvalue1 /= AvregeNum;
					
					
					S_TempCalData->temptestline[index-1] = (unsigned short)(S_TempCalData->tempvalue1);

					//平滑滤波
					if(index >= FilterNum)
					{
						S_TempCalData->tempvalue2 = 0;
						for(j=index-FilterNum; j<index; j++)
						{
							S_TempCalData->tempvalue2 += S_TempCalData->temptestline[j];
						}
						
						S_TempCalData->tempvalue2 /= FilterNum;
						
						S_TempCalData->testdata->testline.TestPoint[index - FilterNum] = S_TempCalData->tempvalue2;
							
						SendTestPointData(&(S_TempCalData->testdata->testline.TestPoint[index - FilterNum]));
					}
						
					S_TempCalData->tempvalue1 = 0;
				}
			}
			
			vTaskDelay(10/portTICK_RATE_MS);
			
			//分析曲线
			AnalysisTestData(S_TempCalData);
			
			if(S_TempCalData->resultstatues == NoResult)
			{
				//发送一个特定数据，清除曲线
				S_TempCalData->testdata->testline.TestPoint[0] = 0xffff;
				SendTestPointData(&(S_TempCalData->testdata->testline.TestPoint[0]));
				goto repeat;
			}

			S_ResultState = S_TempCalData->resultstatues;
			
			MotorMoveTo(MaxLocation, 0);
			
			MyFree(S_TempCalData);
				
			SMBUS_SCK_H();
			
			SetGB_LedValue(0);
			
			return S_ResultState;
	}
	else
		return MemError;	
}




static void AnalysisTestData(TempCalData * S_TempCalData)
{
	unsigned short i=0;
	
	{
		
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
		
		//计算最大值,平均值
		S_TempCalData->maxdata = S_TempCalData->testdata->testline.TestPoint[0];
		S_TempCalData->tempvalue1 = 0;
		for(i=0; i<MaxPointLen; i++)
		{
			if(S_TempCalData->maxdata < S_TempCalData->testdata->testline.TestPoint[i])
				S_TempCalData->maxdata = S_TempCalData->testdata->testline.TestPoint[i];
			
			S_TempCalData->tempvalue1 += S_TempCalData->testdata->testline.TestPoint[i];
		}
		
		//平均值
		S_TempCalData->average = S_TempCalData->tempvalue1 / MaxPointLen;
		
		//计算标准差
		S_TempCalData->tempvalue1 = 0;
		for(i=0; i<MaxPointLen; i++)
		{
			S_TempCalData->tempvalue2 = S_TempCalData->testdata->testline.TestPoint[i];
			S_TempCalData->tempvalue2 -= S_TempCalData->average;
			S_TempCalData->tempvalue2 *= S_TempCalData->tempvalue2;
			S_TempCalData->tempvalue1 += S_TempCalData->tempvalue2;
		}
		S_TempCalData->stdev = S_TempCalData->tempvalue1 / MaxPointLen;
		S_TempCalData->stdev = sqrt(S_TempCalData->stdev);
		
		//计算变异系数
		S_TempCalData->CV = S_TempCalData->stdev / S_TempCalData->average;
		
		//如果变异系数小于1%，说明采集的为直线，未加样
		if(S_TempCalData->CV < 0.01)
		{
			goto END1;
		}
		
		//找c线
		S_TempCalData->testdata->testline.C_Point[0] = 0;
		for(i=S_TempCalData->testdata->temperweima.CLineLocation-40; i<S_TempCalData->testdata->temperweima.CLineLocation+40; i++)
		{
			if(S_TempCalData->testdata->testline.C_Point[0] < S_TempCalData->testdata->testline.TestPoint[i])
			{
				S_TempCalData->testdata->testline.C_Point[0] = S_TempCalData->testdata->testline.TestPoint[i];
				S_TempCalData->testdata->testline.C_Point[1] = i;
			}
		}
		
		//判断C线是不是真实存在
		
		//判断峰值前5个点是不是递增，否则，C峰错误
		for(i=S_TempCalData->testdata->testline.C_Point[1]-5; i<S_TempCalData->testdata->testline.C_Point[1]; i++)
		{
			if(S_TempCalData->testdata->testline.TestPoint[i] > S_TempCalData->testdata->testline.TestPoint[i+1])
				goto END1;
		}
		//判断峰值前5个点是不是递减，否则，C峰错误
		for(i=S_TempCalData->testdata->testline.C_Point[1]; i<S_TempCalData->testdata->testline.C_Point[1]+5; i++)
		{
			if(S_TempCalData->testdata->testline.TestPoint[i] < S_TempCalData->testdata->testline.TestPoint[i+1])
				goto END1;
		}
		
		//找T线
		S_TempCalData->testdata->testline.T_Point[0] = 0;
		for(i=S_TempCalData->testdata->temperweima.ItemLocation-40; i<S_TempCalData->testdata->temperweima.ItemLocation+40; i++)
		{
			if(S_TempCalData->testdata->testline.T_Point[0] < S_TempCalData->testdata->testline.TestPoint[i])
			{
				S_TempCalData->testdata->testline.T_Point[0] = S_TempCalData->testdata->testline.TestPoint[i];
				S_TempCalData->testdata->testline.T_Point[1] = i;
			}
		}
		
		
		
		//找基线
		S_TempCalData->testdata->testline.B_Point[0] = 0xffff;
		for(i=S_TempCalData->testdata->testline.T_Point[1]; i<S_TempCalData->testdata->testline.C_Point[1]; i++)
		{
			if(S_TempCalData->testdata->testline.B_Point[0] > S_TempCalData->testdata->testline.TestPoint[i])
			{
				S_TempCalData->testdata->testline.B_Point[0] = S_TempCalData->testdata->testline.TestPoint[i];
				S_TempCalData->testdata->testline.B_Point[1] = i;
			}
		}
				
		/*计算结果*/
		S_TempCalData->tempvalue2 = (S_TempCalData->testdata->testline.T_Point[0] - S_TempCalData->testdata->testline.B_Point[0]);
		S_TempCalData->tempvalue2 /= (S_TempCalData->testdata->testline.C_Point[0] - S_TempCalData->testdata->testline.B_Point[0]);
				
		/*原始峰高比*/
		S_TempCalData->testdata->testline.BasicBili = S_TempCalData->tempvalue2;
				
		/*根据分段，计算原始结果*/
		if((S_TempCalData->testdata->testline.BasicBili < S_TempCalData->testdata->temperweima.ItemFenDuan) || (S_TempCalData->testdata->temperweima.ItemBiaoQuNum == 1))
		{
			S_TempCalData->testdata->testline.BasicResult = S_TempCalData->testdata->testline.BasicBili * S_TempCalData->testdata->testline.BasicBili;
			S_TempCalData->testdata->testline.BasicResult *= S_TempCalData->testdata->temperweima.ItemBiaoQu[0][0];
					
			S_TempCalData->testdata->testline.BasicResult += (S_TempCalData->testdata->testline.BasicBili * S_TempCalData->testdata->temperweima.ItemBiaoQu[0][1]);
					
			S_TempCalData->testdata->testline.BasicResult += S_TempCalData->testdata->temperweima.ItemBiaoQu[0][2];
		}
		else
		{
			S_TempCalData->testdata->testline.BasicResult = S_TempCalData->testdata->testline.BasicBili * S_TempCalData->testdata->testline.BasicBili;
			S_TempCalData->testdata->testline.BasicResult *= S_TempCalData->testdata->temperweima.ItemBiaoQu[1][0];
					
			S_TempCalData->testdata->testline.BasicResult += (S_TempCalData->testdata->testline.BasicBili * S_TempCalData->testdata->temperweima.ItemBiaoQu[1][1]);
					
			S_TempCalData->testdata->testline.BasicResult += S_TempCalData->testdata->temperweima.ItemBiaoQu[1][2];
		}
				
		if(S_TempCalData->testdata->testline.BasicResult < 0)
			S_TempCalData->testdata->testline.BasicResult = 0;
			
		if(S_TempCalData->testdata->tempadjust.crc == CalModbusCRC16Fun1(&(S_TempCalData->testdata->tempadjust), sizeof(AdjustData)-2) )
			S_TempCalData->testdata->testline.AdjustResult =  S_TempCalData->testdata->testline.BasicResult * S_TempCalData->testdata->tempadjust.parm;
		else
			S_TempCalData->testdata->testline.AdjustResult =  S_TempCalData->testdata->testline.BasicResult;
		
		S_TempCalData->resultstatues = ResultIsOK;
		
		//未加样
		END1:
			S_TempCalData->testdata->testline.T_Point[0] = 0;
			S_TempCalData->testdata->testline.T_Point[1] = 0;
			S_TempCalData->testdata->testline.T_Point[0] = 0;
			S_TempCalData->testdata->testline.T_Point[1] = 0;
			S_TempCalData->testdata->testline.T_Point[0] = 0;
			S_TempCalData->testdata->testline.T_Point[1] = 0;
			S_TempCalData->testdata->testline.BasicBili = 0;
			S_TempCalData->testdata->testline.BasicResult = 0;
			S_TempCalData->testdata->testline.AdjustResult = 0;
			
			S_TempCalData->resultstatues = NoSample;
			
			return ;
		
		END2:
			S_TempCalData->testdata->testline.T_Point[0] = 0;
			S_TempCalData->testdata->testline.T_Point[1] = 0;
			S_TempCalData->testdata->testline.T_Point[0] = 0;
			S_TempCalData->testdata->testline.T_Point[1] = 0;
			S_TempCalData->testdata->testline.T_Point[0] = 0;
			S_TempCalData->testdata->testline.T_Point[1] = 0;
			S_TempCalData->testdata->testline.BasicBili = 0;
			S_TempCalData->testdata->testline.BasicResult = 0;
			S_TempCalData->testdata->testline.AdjustResult = 0;
			
			S_TempCalData->resultstatues = NoSample;
			
			return ;
	}		
}

