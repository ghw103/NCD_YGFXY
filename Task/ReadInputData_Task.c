/***************************************************************************************************
*FileName：LcdInput_Task
*Description：液晶屏输入
*Author：xsx
*Data：2016年4月28日17:40:24
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"ReadInputData_Task.h"
#include	"GetLCDInputFun.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

#include	"stdio.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

#define ReadInDataTask_PRIORITY			2
const char * ReadInDataTaskName = "vReadInDataTask";
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

static void vReadInDataTask( void *pvParameters );

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
void StartvReadInDataTask(void)
{
	xTaskCreate( vReadInDataTask, ReadInDataTaskName, configMINIMAL_STACK_SIZE*2, NULL, ReadInDataTask_PRIORITY, NULL );
}

/***************************************************************************************************
*FunctionName：vSysLedTask
*Description：系统指示灯闪烁表面程序正常运行
*Input：None
*Output：None
*Author：xsx
*Data：2015年8月26日16:58:46
***************************************************************************************************/
static void vReadInDataTask( void *pvParameters )
{
	while(1)
	{
		GetLCDInputData();
	}
}
