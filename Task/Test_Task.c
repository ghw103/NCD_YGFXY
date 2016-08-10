/***************************************************************************************************
*FileName：Test_Task
*Description：测试任务
*Author：xsx
*Data：2015年8月26日17:06:08
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"Test_Task.h"


#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	"stdio.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

#define TestTask_PRIORITY			2
const char * TestTaskName = "vTestTask";

static xQueueHandle xStartTestQueue = NULL ;			//互斥量，如果接收到数据，则开始测试，接收的数据为测试数据保存地址
static xQueueHandle xTestResultQueue = NULL;											//发送测试结果
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static void vTestTask( void *pvParameters );

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：StartvSysLedTask
*Description：建立系统指示灯任务
*Input：None
*Output：None
*Author：xsx
*Data：2015年8月26日16:58:13
***************************************************************************************************/
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

/***************************************************************************************************
*FunctionName：vSysLedTask
*Description：系统指示灯闪烁表面程序正常运行
*Input：None
*Output：None
*Author：xsx
*Data：2015年8月26日16:58:46
***************************************************************************************************/
static void vTestTask( void *pvParameters )
{
	void * parm = NULL;
	ResultState testresult = NoResult;
	
	while(1)
	{
		if(pdPASS == xQueueReceive( xStartTestQueue, &parm, portMAX_DELAY))
		{
			while(pdPASS == TakeTestResult(&testresult))
				;
			
			testresult = TestFunction(parm);
			
			/*发送测试结果*/
			xQueueSend( xTestResultQueue, &testresult, 10000/portTICK_RATE_MS );
				
			parm = NULL;
		}
		
		vTaskDelay(500 * portTICK_RATE_MS);
	}
}

/***************************************************************************************************
*FunctionName：SetTestTaskRun
*Description：启动测试
*Input：parm -- 测试数据的地址，传入的应该是存放测试数据的地址，所以队列发送的时候发送的是存放测试数据的指针的地址(测试数据指的整个测试数据，包含二维码)
*Output：None
*Author：xsx
*Data：2016年5月14日17:26:56
***************************************************************************************************/
MyState_TypeDef StartTest(void * parm)
{
	if(pdPASS == xQueueSend( xStartTestQueue, &parm, 10*portTICK_RATE_MS ))
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
