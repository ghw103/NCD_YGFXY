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

void PrintfData(TestData * testd)
{
	char * printfbuf = NULL;
	MyTime_Def mytime;
	float tempvalue = 0.0;
	
	printfbuf = MyMalloc(100);

	if(printfbuf && testd)
	{
		sprintf(printfbuf, "------------------------------\r\0");
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);

		sprintf(printfbuf, "武汉纽康度生物科技股份有限公司\r\r\0");
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
		
		sprintf(printfbuf, "测试人: %s\r\0", testd->user.user_name);
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
	
		sprintf(printfbuf, "样品编号: %s\r\0", testd->sampleid);
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
	
		sprintf(printfbuf, "测试项目: %s\r\0", testd->temperweima.ItemName);
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);

		tempvalue = testd->testline.AdjustResult;
		if(testd->testResultDesc != ResultIsOK)
			sprintf(printfbuf, "测试结果: ERROR\r\0");
		else if(IsShowRealValue() == true)
			sprintf(printfbuf, "测试结果: %.*f %-8.8s\r\0", testd->temperweima.itemConstData.pointNum, testd->testline.AdjustResult, testd->temperweima.itemConstData.itemMeasure);
		else if(tempvalue <= testd->temperweima.itemConstData.lowstResult)
			sprintf(printfbuf, "测试结果: <%.*f %-8.8s\r\0", testd->temperweima.itemConstData.pointNum, testd->temperweima.itemConstData.lowstResult, testd->temperweima.itemConstData.itemMeasure);
		else if(tempvalue >= testd->temperweima.itemConstData.highestResult)
			sprintf(printfbuf, "测试结果: >%.*f %-8.8s\r\0", testd->temperweima.itemConstData.pointNum, testd->temperweima.itemConstData.highestResult, testd->temperweima.itemConstData.itemMeasure);
		else
			sprintf(printfbuf, "测试结果: %.*f %-8.8s\r\0", testd->temperweima.itemConstData.pointNum, testd->testline.AdjustResult, testd->temperweima.itemConstData.itemMeasure);
		
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);

		sprintf(printfbuf, "测试时间: 20%02d-%02d-%02d %02d:%02d:%02d\r\0", testd->TestTime.year, testd->TestTime.month, testd->TestTime.day
			, testd->TestTime.hour, testd->TestTime.min, testd->TestTime.sec);
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);

		GetGB_Time(&mytime);
		sprintf(printfbuf, "打印时间: 20%02d-%02d-%02d %02d:%02d:%02d\r\0", mytime.year, mytime.month, mytime.day
			, mytime.hour, mytime.min, mytime.sec);
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);

		sprintf(printfbuf, "声明：本结果仅对本标本负责！\r\0");
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);

		sprintf(printfbuf, "\r------------------------------\r\r\r\r\r\r\0");
		SendDataToQueue(GetUsart3TXQueue(), GetUsart3Mutex(), printfbuf, strlen(printfbuf), sizeof(unsigned char), 50 / portTICK_RATE_MS, 100 / portTICK_RATE_MS, EnableUsart3TXInterrupt);
	}
	
	MyFree(printfbuf);

}

/****************************************end of file************************************************/
