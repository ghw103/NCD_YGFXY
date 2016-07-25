/***************************************************************************************************
*FileName:SDFunction
*Description:一些SD卡的操作函数
*Author:xsx
*Data:2016年4月30日16:06:36
***************************************************************************************************/


/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"Motor_Fun.h"
#include	"Motor_Data.h"
#include	"Timer4_Driver.h"
#include	"CardLimit_Driver.h"
#include	"DRV8825_Driver.h"


#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"





/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

static xSemaphoreHandle xMotorMutex = NULL;	
unsigned char motorstautes = 0;
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static BaseType_t TakeMotorMutex(portTickType xBlockTime);

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/


MyState_TypeDef ClearMotorMutex(void)
{
	if(xMotorMutex)
	{
		while(pdPASS == TakeMotorMutex(0))
			;
	}
	
	return My_Pass;
}

void InitMotorData(void)
{
	if(xMotorMutex == NULL)
	{
		xMotorMutex = xSemaphoreCreateBinary();
	}
	
}
/***************************************************************************************************
*FunctionName:GiveMotorMutex, TakeMotorMutex
*Description:??????
*Input:None
*Output:None
*Author:xsx
*Data:2016?5?11?19:24:37
***************************************************************************************************/
BaseType_t GiveMotorMutex(void)
{
	portBASE_TYPE     xHigherPriorityTaskWoken = pdFALSE;
	
	if(xMotorMutex)
		return xSemaphoreGiveFromISR(xMotorMutex, &xHigherPriorityTaskWoken);		
	else
		return My_Pass;
}

static BaseType_t TakeMotorMutex(portTickType xBlockTime)
{
	return xSemaphoreTake(xMotorMutex , xBlockTime);		
}


void MotorMoveTo(unsigned int location, unsigned char mode)
{
	if(location > MaxLocation)
			location = MaxLocation;

//	ClearMotorMutex();
	
	SetGB_MotorTargetLocation(location);
	
	if(GetGB_MotorTargetLocation() > GetGB_MotorLocation())
		SetDRVDir(Forward);
	else
		SetDRVDir(Reverse);
	
	motorstautes = 1;
	StartTimer4();
	if(mode == 0)
	{
		while(1 == motorstautes)
			vTaskDelay(1 / portTICK_RATE_MS);;
	}
}

void StopMotor(void)
{
	StopTimer4();
}
