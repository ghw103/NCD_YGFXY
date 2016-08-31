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

#if (NormalCode != CodeType)
	#include	"System_Data.h"
#endif

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

static xQueueHandle xStartTestQueue = NULL ;
static xQueueHandle xTestResultQueue = NULL;
static TestTaskData S_TestTaskData;
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
	if(xStartTestQueue == NULL)
		xStartTestQueue = xQueueCreate(1, sizeof(void *));
	
	if(xStartTestQueue == NULL)
		return My_Fail;
	
	if(xTestResultQueue == NULL)
		xTestResultQueue = xQueueCreate(1, sizeof(ResultState));
	
	if(xTestResultQueue == NULL)
		return My_Fail;
	
	if(My_Fail == InitTestFunData())
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
		if(pdPASS == xQueueReceive( xStartTestQueue, ((void *)&(S_TestTaskData.testdata)), portMAX_DELAY))
		{
			while(pdPASS == TakeTestResult(&(S_TestTaskData.testresult)))
				;
			
			#if (NormalCode != CodeType)
			
				SetTestStatusFlorLab(1);
			#endif
			TestFunction(&S_TestTaskData);
			
			xQueueSend( xTestResultQueue, &(S_TestTaskData.testresult), 1000/portTICK_RATE_MS );
				
			memset(&S_TestTaskData, 0, sizeof(TestTaskData));
			
			#if (NormalCode != CodeType)
			
				SetTestStatusFlorLab(0);
			#endif
		}
		
		vTaskDelay(500 * portTICK_RATE_MS);
	}
}


MyState_TypeDef StartTest(void * parm)
{
	if(pdPASS == xQueueSend( xStartTestQueue, &parm, 10*portTICK_RATE_MS ))
		return My_Pass;
	else
		return My_Fail;	
}

MyState_TypeDef StopTest(void)
{
	S_TestTaskData.testresult = TestInterrupt;
	return My_Pass;
}

MyState_TypeDef TakeTestResult(ResultState *testsult)
{
	if(pdPASS == xQueueReceive( xTestResultQueue, testsult,  10/portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;
}

/****************************************end of file************************************************/
