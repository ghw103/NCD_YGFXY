/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"CodeScanner_Driver.h"
#include	"CodeScanFunction.h"
#include	"Usart2_Driver.h"

#include	"MyEncryptTool.h"
#include	"QueueUnits.h"
#include	"Motor_Fun.h"
#include	"Motor_Data.h"
#include	"CardStatues_Data.h"
#include	"System_Data.h"
#include	"Timer_Data.h"

#include	"Define.h"
#include	"CRC16.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/

/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void ReadBasicCodeData(ReadQRCodeBuffer * readQRCodeBuffer);
static void AnalysisCode(ReadQRCodeBuffer * readQRCodeBuffer);
static MyState_TypeDef CheckCardIsTimeOut(ReadQRCodeBuffer * readQRCodeBuffer);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

ScanCodeResult ScanCodeFun(QRCode * parm)
{
	ReadQRCodeBuffer * readQRCodeBuffer;
	ScanCodeResult scanresult;							//扫码二维码结果
	
	if(parm == NULL)
		return CardCodeScanFail;
	
	readQRCodeBuffer = MyMalloc(sizeof(ReadQRCodeBuffer));
	if(NULL == readQRCodeBuffer)
		return CardCodeScanFail;
	
	memset(readQRCodeBuffer, 0, sizeof(ReadQRCodeBuffer));
	
	readQRCodeBuffer->cardQR = parm;
	memset(readQRCodeBuffer->cardQR, 0, sizeof(QRCode));
		
	MotorMoveTo(0, 0);
	MotorMoveTo(500, 0);
	readQRCodeBuffer->motorLocation = GetGB_MotorLocation();
		
	OpenCodeScanner();
	
	while(pdPASS == ReceiveDataFromQueue(GetUsart2RXQueue(), GetUsart2RXMutex(), readQRCodeBuffer->originalcode , MAX_QR_CODE_LENGHT, 1, 50 * portTICK_RATE_MS))
		;

	readQRCodeBuffer->scanresult = CardCodeScanning;
	
	//设置定时器
	timer_set(&(readQRCodeBuffer->timer), MAX_SCAN_QR_TIME);
	
	while(readQRCodeBuffer->scanresult == CardCodeScanning)
	{
		if(GetCardState() == NoCard)				//卡被拔出
		{
			readQRCodeBuffer->scanresult = CardCodeCardOut;
			break;
		}
		else
		{
			if((readQRCodeBuffer->motorLocation >= 1000)||(readQRCodeBuffer->motorLocation <= 200))
				readQRCodeBuffer->motorDir++;
			
			if(readQRCodeBuffer->motorDir%2 == 0)
				readQRCodeBuffer->motorLocation += 1;
			else
				readQRCodeBuffer->motorLocation -= 1;
			
			MotorMoveTo(readQRCodeBuffer->motorLocation, 0);
			
			if(readQRCodeBuffer->motorLocation % 50 == 0)
				ReadBasicCodeData(readQRCodeBuffer);

			vTaskDelay(2 / portTICK_RATE_MS);
			
			if(TimeOut == timer_expired(&(readQRCodeBuffer->timer)))
				readQRCodeBuffer->scanresult = CardCodeScanTimeOut;				
		}
	}

	CloseCodeScanner();
	
	scanresult = readQRCodeBuffer->scanresult;
	
	MyFree(readQRCodeBuffer);
	
	return scanresult;
}

/***************************************************************************************************
*FunctionName：ReadBasicCodeData
*Description：读取原始二维码
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月25日14:07:38
***************************************************************************************************/
static void ReadBasicCodeData(ReadQRCodeBuffer * readQRCodeBuffer)
{
	if(readQRCodeBuffer == NULL)
		return;
	
	while(pdPASS == ReceiveDataFromQueue(GetUsart2RXQueue(), GetUsart2RXMutex(), readQRCodeBuffer->originalcode , MAX_QR_CODE_LENGHT, 1, 10 / portTICK_RATE_MS))	
		;
	
	readQRCodeBuffer->originalCodeLen = strlen(readQRCodeBuffer->originalcode);
	
	if(readQRCodeBuffer->originalCodeLen > 0)
		AnalysisCode(readQRCodeBuffer);
}

/***************************************************************************************************
*FunctionName：DecryptCode
*Description：解码并解析二维码数据
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月25日14:08:25
***************************************************************************************************/
static void AnalysisCode(ReadQRCodeBuffer * readQRCodeBuffer)
{
	unsigned short datalen = 0;
	unsigned char i=0;
	
	if(readQRCodeBuffer == NULL)
		return;
	
	/*清空二维码空间*/
	memset(readQRCodeBuffer->decryptcode, 0, MAX_QR_CODE_LENGHT);

	/*数据解密失败*/
	if(pdFAIL == MyDencrypt(readQRCodeBuffer->originalcode, readQRCodeBuffer->decryptcode, readQRCodeBuffer->originalCodeLen))
		goto END;
	
	memcpy(readQRCodeBuffer->originalcode, readQRCodeBuffer->decryptcode, readQRCodeBuffer->originalCodeLen);
	readQRCodeBuffer->pbuf2 = readQRCodeBuffer->originalcode;
	
	/*获取数据头*/
	readQRCodeBuffer->pbuf1 = strtok(readQRCodeBuffer->decryptcode, "#");
	if(readQRCodeBuffer->pbuf1)
	{
		if(0 != strcmp(readQRCodeBuffer->pbuf1, "AB"))
			goto END;
		else
			readQRCodeBuffer->pbuf2 += 3;
	}
	else
		goto END;
	
	/*获取数据长度*/
	readQRCodeBuffer->pbuf1 = strtok(NULL, "#");
	if(readQRCodeBuffer->pbuf1)
	{
		datalen = strtol(readQRCodeBuffer->pbuf1 , NULL , 10);
		readQRCodeBuffer->pbuf2 += (strlen(readQRCodeBuffer->pbuf1)+1);
	}
	else
		goto END;
	
	/*获取测试项目名称*/
	readQRCodeBuffer->pbuf1 = strtok(NULL, "#");
	if(readQRCodeBuffer->pbuf1)
		memcpy(readQRCodeBuffer->cardQR->ItemName, readQRCodeBuffer->pbuf1 ,strlen(readQRCodeBuffer->pbuf1));
	else
		goto END;
		
	/*读取检测卡上的检测指标正常范围*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		memcpy(readQRCodeBuffer->cardQR->NormalResult, readQRCodeBuffer->pbuf1 ,strlen(readQRCodeBuffer->pbuf1));
	else
		goto END;
		
	/*读取检测卡上的最低检测值*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->LowstResult = strtod(readQRCodeBuffer->pbuf1 , NULL );
	else
		goto END;
		
	/*读取检测卡上的最高检测值*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->HighestResult = strtod(readQRCodeBuffer->pbuf1 , NULL );
	else
		goto END;	
		
	/*读取测试项目的单位*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		memcpy(readQRCodeBuffer->cardQR->ItemMeasure, readQRCodeBuffer->pbuf1 ,strlen(readQRCodeBuffer->pbuf1));
	else
		goto END;
	
	//读取小数点个数
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ItemPoint = strtol(readQRCodeBuffer->pbuf1 , NULL, 10);
	else
		goto END;
	
	/*读取检测卡T线位置*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ItemLocation = strtol(readQRCodeBuffer->pbuf1 , NULL, 10);
	else
		goto END;
		
	/*读取检测卡标准曲线临界浓度1*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ItemFenDuan[0] = strtod(readQRCodeBuffer->pbuf1 , NULL);
	else
		goto END;
	
	/*读取检测卡标准曲线临界浓度2*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->ItemFenDuan[1] = strtod(readQRCodeBuffer->pbuf1 , NULL);
	else
		goto END;

	/*标准曲线1*/
	for(i=0; i<3; i++)
	{
		readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
		if(readQRCodeBuffer->pbuf1)
			readQRCodeBuffer->cardQR->ItemBiaoQu[0][i] = strtod(readQRCodeBuffer->pbuf1 , NULL);
		else
			goto END;
	}
	
	/*标准曲线2*/
	if(readQRCodeBuffer->cardQR->ItemFenDuan[0] > 0)
	{
		for(i=0; i<3; i++)
		{
			readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
			if(readQRCodeBuffer->pbuf1)
				readQRCodeBuffer->cardQR->ItemBiaoQu[1][i] = strtod(readQRCodeBuffer->pbuf1 , NULL);
			else
				goto END;
		}
	}
	
	/*标准曲线3*/
	if(readQRCodeBuffer->cardQR->ItemFenDuan[1] > 0)
	{
		for(i=0; i<3; i++)
		{
			readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
			if(readQRCodeBuffer->pbuf1)
				readQRCodeBuffer->cardQR->ItemBiaoQu[2][i] = strtod(readQRCodeBuffer->pbuf1 , NULL);
			else
				goto END;
		}
	}
		
	/*读取检测卡反应时间*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->CardWaitTime = strtol(readQRCodeBuffer->pbuf1 , NULL , 10);
	else
		goto END;
		
	/*读取检测卡C线位置*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		readQRCodeBuffer->cardQR->CLineLocation = strtol(readQRCodeBuffer->pbuf1 , NULL , 10);
	else
		goto END;


	/*读取检测卡批号*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		memcpy(readQRCodeBuffer->cardQR->PiHao, readQRCodeBuffer->pbuf1, strlen(readQRCodeBuffer->pbuf1));
	else
		goto END;
	
	
	/*读取检测卡批号*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
		memcpy(readQRCodeBuffer->cardQR->piNum, readQRCodeBuffer->pbuf1, strlen(readQRCodeBuffer->pbuf1));
	else
		goto END;

	/*读取检测卡保质期*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
	{
		static char year[10] ,month[10],day[10];
		memcpy(year, readQRCodeBuffer->pbuf1, 2);
		readQRCodeBuffer->cardQR->CardBaoZhiQi.year = strtol(year , NULL , 10);

		memcpy(month, &readQRCodeBuffer->pbuf1[2], 2);
		readQRCodeBuffer->cardQR->CardBaoZhiQi.month = (unsigned char)strtod(month , NULL );

		memcpy(day, &readQRCodeBuffer->pbuf1[4], 2);
		readQRCodeBuffer->cardQR->CardBaoZhiQi.day = (unsigned char)strtod(day , NULL );
	}
	else
		goto END;

	/*读取二维码CRC*/
	readQRCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(readQRCodeBuffer->pbuf1)
	{
		readQRCodeBuffer->cardQR->CRC16 = strtol(readQRCodeBuffer->pbuf1 , NULL , 10);
			
		datalen -= strlen(readQRCodeBuffer->pbuf1);
		goto END;
	}
	else
		goto END;
	
	END:
		if(readQRCodeBuffer->cardQR->CRC16 != CalModbusCRC16Fun1(readQRCodeBuffer->pbuf2 , datalen))
			readQRCodeBuffer->scanresult = CardCodeCRCError;		
		else if(My_Fail == CheckCardIsTimeOut(readQRCodeBuffer))
			readQRCodeBuffer->scanresult = CardCodeTimeOut;
		else
			readQRCodeBuffer->scanresult = CardCodeScanOK;
}


static MyState_TypeDef CheckCardIsTimeOut(ReadQRCodeBuffer * readQRCodeBuffer)
{
	if(readQRCodeBuffer)
	{
		GetGB_Time(&(readQRCodeBuffer->temptime));
		
		if(readQRCodeBuffer->cardQR->CardBaoZhiQi.year == readQRCodeBuffer->temptime.year)
		{
			if(readQRCodeBuffer->cardQR->CardBaoZhiQi.month == readQRCodeBuffer->temptime.month)
			{
				if(readQRCodeBuffer->cardQR->CardBaoZhiQi.day >= readQRCodeBuffer->temptime.day)
					return My_Pass;
			}
			else if(readQRCodeBuffer->cardQR->CardBaoZhiQi.month > readQRCodeBuffer->temptime.month)
				return My_Pass;
		}
		else if(readQRCodeBuffer->cardQR->CardBaoZhiQi.year > readQRCodeBuffer->temptime.year)
			return My_Pass;
	}
	
	return My_Fail;
}

