/***************************************************************************************************
*FileName：SelfTest_Fun
*Description：自检功能
*Author：xsx
*Data：2016年1月27日10:29:14
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"SelfTest_Fun.h"

#include	"CodeScanner_Driver.h"
#include	"SDFunction.h"
#include	"WifiFunction.h"
#include	"Ads8325_Driver.h"
#include	"TLV5617_Driver.h"
#include	"MAX4051_Driver.h"
#include	"Motor_Fun.h"
#include	"Motor_Data.h"
#include	"CardLimit_Driver.h"
#include	"DRV8825_Driver.h"
#include	"SystemSet_Data.h"

#include	"SystemSet_Dao.h"
#include	"SelfCheck_Data.h"
#include	"MyMem.h"
#include	"CRC16.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	"stdlib.h"
#include	<string.h>
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

static MyState_TypeDef ADDASelfTest(void);

static MyState_TypeDef MotorSelfTest(void);

static MyState_TypeDef GB_DataInit(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************
*FunctionName：SelfTest_Function
*Description：自检功能实现
*Input：None
*Output：None
*Author：xsx
*Data：2016年1月27日10:38:06
***************************************************************************************************/
void SelfTest_Function(void)
{	
	GB_DataInit();
	vTaskDelay(50 *portTICK_RATE_MS);
	
//	ErWeiMaSelfTest();
	
//	WIFICheck();
	vTaskDelay(50 * portTICK_RATE_MS);
	
	ADDASelfTest();
	vTaskDelay(50 *portTICK_RATE_MS);
	
	MotorSelfTest();
	vTaskDelay(50 *portTICK_RATE_MS);
	
	SetGB_SelfCheckStatus(SelfCheck_OK);
}


/***************************************************************************************************
*FunctionName：ADDASelfTest
*Description：采集模块自检
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月27日17:21:14
***************************************************************************************************/
static MyState_TypeDef ADDASelfTest(void)
{
	static double tempvalue1 = 0.0, tempvalue2 = 0.0;
	
	SetGB_LedValue(0);
	vTaskDelay(100 / portTICK_RATE_MS);	
	SetGB_CLineValue(0);
	vTaskDelay(100 / portTICK_RATE_MS);	
	SelectChannel(0);
	vTaskDelay(100 / portTICK_RATE_MS);
	
	tempvalue1 = ADS8325();
	vTaskDelay(100 / portTICK_RATE_MS);
	
	SetGB_LedValue(300);
	vTaskDelay(100 / portTICK_RATE_MS);
	tempvalue2 = ADS8325();
	vTaskDelay(100 / portTICK_RATE_MS);
	if(tempvalue2 <= tempvalue1)
		return My_Fail;
	
	SetGB_CLineValue(5);
	vTaskDelay(100 / portTICK_RATE_MS);
	tempvalue1 = ADS8325();
	vTaskDelay(100 / portTICK_RATE_MS);
	if(tempvalue2 < tempvalue1)
		return My_Fail;
	
	SelectChannel(7);
	vTaskDelay(100 / portTICK_RATE_MS);
	tempvalue2 = ADS8325();
	vTaskDelay(100 / portTICK_RATE_MS);	
	if(tempvalue2 < tempvalue1)
		return My_Fail;
	else
		return My_Pass;
}



/***************************************************************************************************
*FunctionName：MotorSelfTest
*Description：电机自检
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月27日17:39:36
***************************************************************************************************/
static MyState_TypeDef MotorSelfTest(void)
{
	static unsigned char count = 0;
	
	SetDRVPowerStatues(LowPower);

	SetGB_MotorLocation(10000);
	MotorMoveTo(0, 1);
	vTaskDelay(100 / portTICK_RATE_MS);
	
	while(!BackLimited)
	{
		vTaskDelay(500 / portTICK_RATE_MS);
		
		count++;
		if(count > 6)
			break;
	}
	
	if(count > 6)
	{
		StopMotor();
		return My_Fail;
	}
	
	
	SetDRVPowerStatues(NonamalPower);
	vTaskDelay(100 / portTICK_RATE_MS);
	MotorMoveTo(MaxLocation, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	
	//走到最大行程，如果前限位触发，则报异常
	if(PreLimited)
		return My_Fail;
	
	return My_Pass;
}


static MyState_TypeDef GB_DataInit(void)
{
	SystemSetData * systemSetData = NULL;
	MyState_TypeDef status = My_Fail;
	
	systemSetData = MyMalloc(sizeof(SystemSetData));
	
	if(systemSetData)
	{
		//读取SD卡中的配置文件
		ReadSystemSetData(systemSetData);
		
		//如果crc错误表示配置文件出错，需要使用默认配置
		if(systemSetData->crc != CalModbusCRC16Fun1(systemSetData, sizeof(SystemSetData) - 2))
		{
			//恢复默认设置
			setDefaultSetData(systemSetData);
			
			//保存默认设置
			if(My_Pass == SaveSystemSetData(systemSetData))
			{
				setSystemSetData(systemSetData);
				status = My_Pass;
			}
		}
		else
		{
			setSystemSetData(systemSetData);
			status = My_Pass;
		}
	}
	
	MyFree(systemSetData);
	
	return status;
}
