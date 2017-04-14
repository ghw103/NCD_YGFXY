/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"Printf_Fun.h"
#include 	"Usart3_Driver.h"
#include	"SystemSet_Data.h"
#include	"System_Data.h"
#include	"QueueUnits.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void PrintfData(TestData * testd2)
{
	char * printfbuf = NULL;
	TestData * tempTestData = NULL;
	MyTime_Def mytime;
	float tempvalue = 0.0;
	
	printfbuf = MyMalloc(100);
	tempTestData = MyMalloc(sizeof(TestData));

	if(printfbuf && testd2 && tempTestData)
	{
		//首先复制数据到自己的缓冲器，以防多任务下，其他任务释放原数据区
		memcpy(tempTestData, testd2, sizeof(TestData));
		
		sprintf(printfbuf, "------------------------------\r\0");
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		vTaskDelay(1000 / portTICK_RATE_MS);
		
		sprintf(printfbuf, "武汉纽康度生物科技股份有限公司\r\r\0");
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		vTaskDelay(1200 / portTICK_RATE_MS);
		
		sprintf(printfbuf, "测试人: %s\r\0", tempTestData->user.user_name);
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		vTaskDelay(1200 / portTICK_RATE_MS);
		
		sprintf(printfbuf, "样品编号: %s\r\0", tempTestData->sampleid);
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		vTaskDelay(1200 / portTICK_RATE_MS);
		
		sprintf(printfbuf, "测试项目: %s\r\0", tempTestData->temperweima.ItemName);
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		vTaskDelay(1200 / portTICK_RATE_MS);
		
		tempvalue = tempTestData->testline.AdjustResult;
		if(tempTestData->testResultDesc != ResultIsOK)
			sprintf(printfbuf, "测试结果: ERROR\r\0");
		else if(IsShowRealValue() == true)
			sprintf(printfbuf, "测试结果: %.*f %-8.8s\r\0", tempTestData->temperweima.itemConstData.pointNum, tempTestData->testline.AdjustResult, tempTestData->temperweima.itemConstData.itemMeasure);
		else if(tempvalue <= tempTestData->temperweima.itemConstData.lowstResult)
			sprintf(printfbuf, "测试结果: <%.*f %-8.8s\r\0", tempTestData->temperweima.itemConstData.pointNum, tempTestData->temperweima.itemConstData.lowstResult, tempTestData->temperweima.itemConstData.itemMeasure);
		else if(tempvalue >= tempTestData->temperweima.itemConstData.highestResult)
			sprintf(printfbuf, "测试结果: >%.*f %-8.8s\r\0", tempTestData->temperweima.itemConstData.pointNum, tempTestData->temperweima.itemConstData.highestResult, tempTestData->temperweima.itemConstData.itemMeasure);
		else
			sprintf(printfbuf, "测试结果: %.*f %-8.8s\r\0", tempTestData->temperweima.itemConstData.pointNum, tempTestData->testline.AdjustResult, tempTestData->temperweima.itemConstData.itemMeasure);
		
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		vTaskDelay(1200 / portTICK_RATE_MS);
		
		sprintf(printfbuf, "测试时间: 20%02d-%02d-%02d %02d:%02d:%02d\r\0", tempTestData->TestTime.year, tempTestData->TestTime.month, tempTestData->TestTime.day
			, tempTestData->TestTime.hour, tempTestData->TestTime.min, tempTestData->TestTime.sec);
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		vTaskDelay(1200 / portTICK_RATE_MS);
		
		GetGB_Time(&mytime);
		sprintf(printfbuf, "打印时间: 20%02d-%02d-%02d %02d:%02d:%02d\r\0", mytime.year, mytime.month, mytime.day
			, mytime.hour, mytime.min, mytime.sec);
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		vTaskDelay(1200 / portTICK_RATE_MS);
		
		sprintf(printfbuf, "声明：本结果仅对本标本负责！\r\0");
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		vTaskDelay(1500 / portTICK_RATE_MS);
		
		sprintf(printfbuf, "\r------------------------------\r\r\r\r\r\n\n\r\n\0");
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		vTaskDelay(2000 / portTICK_RATE_MS);
	}
	
	MyFree(tempTestData);
	MyFree(printfbuf);
}

/****************************************end of file************************************************/
