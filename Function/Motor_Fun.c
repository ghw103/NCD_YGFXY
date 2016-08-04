/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
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
/***************************************************************************************************/
/***************************************************************************************************/
static xSemaphoreHandle xMotorMutex = NULL;	
unsigned char motorstautes = 0;
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static BaseType_t TakeMotorMutex(portTickType xBlockTime);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
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
/****************************************end of file************************************************/