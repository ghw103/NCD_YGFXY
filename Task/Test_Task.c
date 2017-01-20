/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"Test_Task.h"
#include	"Test_Fun.h"
#include	"System_Data.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
#define TestTask_PRIORITY			2
const char * TestTaskName = "vTestTask";

static xQueueHandle xStartTestQueue = NULL ;						//扫卡数据空间队列，并用于启动扫卡任务
static TestData * testdata;											//扫卡数据指针

static xQueueHandle xTestResultQueue = NULL;						//扫卡结果队列
static ResultState resultstatues;									//扫卡结果
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void vTestTask( void *pvParameters );
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

MyState_TypeDef StartvTestTask(void)
{
	//创建任务数据队列，同时用于任务启动
	if(xStartTestQueue == NULL)
		xStartTestQueue = xQueueCreate(1, sizeof(void *));
	
	if(xStartTestQueue == NULL)
		return My_Fail;
	
	//创建任务结果队列
	if(xTestResultQueue == NULL)
		xTestResultQueue = xQueueCreate(1, sizeof(ResultState));
	
	if(xTestResultQueue == NULL)
		return My_Fail;
	
	if(pdFAIL == xTaskCreate( vTestTask, TestTaskName, configMINIMAL_STACK_SIZE, NULL, TestTask_PRIORITY, NULL ))
		return My_Fail;
	else
		return My_Pass;
}


static void vTestTask( void *pvParameters )
{
	while(1)
	{
		if(pdPASS == xQueueReceive( xStartTestQueue, &testdata, portMAX_DELAY))
		{
			while(pdPASS == TakeTestResult(&resultstatues))
				;
			
			#if (NormalCode != CodeType)
			
				SetTestStatusFlorLab(1);
			#endif
			resultstatues = TestFunction(testdata);
			
			xQueueSend( xTestResultQueue, &resultstatues, 1000/portTICK_RATE_MS );
			
			#if (NormalCode != CodeType)
			
				SetTestStatusFlorLab(0);
			#endif
		}
	}
}


MyState_TypeDef StartTest(void * parm)
{
	if(pdPASS == xQueueSend( xStartTestQueue, &parm, 10/portTICK_RATE_MS ))
		return My_Pass;
	else
		return My_Fail;	
}


MyState_TypeDef TakeTestResult(ResultState *testsult)
{
	if(pdPASS == xQueueReceive( xTestResultQueue, testsult,  10/portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;
}

/****************************************end of file************************************************/
