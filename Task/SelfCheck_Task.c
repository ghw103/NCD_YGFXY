/***************************************************************************************************
*FileName:SelfCheck_Task
*Description:自检任务
*Author:xsx
*Data:2016年4月27日10:42:43
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"SelfCheck_Task.h"
#include	"SelfTest_Fun.h"


#include 	"queue.h"
#include	"semphr.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
#define vSelfCheckTask_PRIORITY			( ( unsigned portBASE_TYPE ) 2U )
const char * SelfCheckTaskName = "vSelfCheckTask";

static xSemaphoreHandle xTaskMutex = NULL ;									//自检任务运行
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static portBASE_TYPE vSelfTestTaskInit(void);
static void vSelfCheckTask( void *pvParameters );
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/************************************************************************
** 函数名:
** 功  能:
** 输  入:无
** 输  出:无
** 返  回：无
** 备  注：无
** 时  间:  
** 作  者：xsx                                                 
************************************************************************/
void StartSelfCheckTask(void)
{
	/*创建互斥量，并清空*/
	if(xTaskMutex == NULL)
		xTaskMutex = xSemaphoreCreateMutex();
	
	if(xTaskMutex == NULL)
		return;
	
	while(pdPASS == xSemaphoreTake(xTaskMutex , 10*portTICK_RATE_MS));			//清空信号量

	if(pdFAIL == vSelfTestTaskInit())
		return;
	
	xTaskCreate( vSelfCheckTask, SelfCheckTaskName, configMINIMAL_STACK_SIZE, NULL, vSelfCheckTask_PRIORITY, NULL );
}


static portBASE_TYPE vSelfTestTaskInit(void)
{
	return SelfTestFun_Init();
}

/************************************************************************
** 函数名:
** 功  能:
** 输  入:无
** 输  出:无
** 返  回：无
** 备  注：无
** 时  间:  
** 作  者：xsx                                                 
************************************************************************/
static void vSelfCheckTask( void *pvParameters )
{
	while(1)
	{
		if(pdPASS == xSemaphoreTake(xTaskMutex , portMAX_DELAY))
		{
			SelfTest_Function();
			vTaskDelay(1000 * portTICK_RATE_MS);
		}
		
		vTaskDelete(NULL);
	}
}

/***************************************************************************************************
*FunctionName：SelfCheckTaskRun
*Description：启动自检任务
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月30日15:48:04
***************************************************************************************************/
void SelfCheckTaskRun(void)
{
	/*创建互斥量，并清空*/
	if(xTaskMutex == NULL)
	{
		xTaskMutex = xSemaphoreCreateMutex();
		
		if(xTaskMutex == NULL)
			return;
		
		while(pdPASS == xSemaphoreTake(xTaskMutex , 10*portTICK_RATE_MS));			//清空信号量
	}
	xSemaphoreGive(xTaskMutex);
}

/***************************************************************************************************
*FunctionName：GetSelfTestTaskState
*Description：Get SelfTest Task State
*Input：receivedchar -- 返回数据地址
*		xBlockTime -- 等待时间
*Output：读取状态
*Author：xsx
*Data：2016年1月27日10:21:33
***************************************************************************************************/
portBASE_TYPE GetSelfTestTaskState(unsigned char * receivedchar , portTickType xBlockTime)
{
	return GetSelfTestFunState(receivedchar, xBlockTime);

}





