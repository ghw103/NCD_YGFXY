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
static CardCodeInfo * cardQR;								//扫描二维码数据空间地址
static ReadCodeBuffer * S_ReadCodeBuffer;					//用于处理二维码的缓存
static ScanCodeResult scanresult;							//扫码二维码结果
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void ReadBasicCodeData(void);
static void AnalysisCode(void *pbuf , unsigned short len);
static MyState_TypeDef CheckCardIsTimeOut(CardCodeInfo * s_CardCodeInfo);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

ScanCodeResult ScanCodeFun(void * parm)
{
	unsigned short count = 0;
	unsigned char dir = 0;							//扫描次数	
	
	if(parm == NULL)
		return CardCodeScanFail;
	
	S_ReadCodeBuffer = MyMalloc(sizeof(ReadCodeBuffer));
	if(NULL == S_ReadCodeBuffer)
		return CardCodeScanFail;
	
	memset(S_ReadCodeBuffer, 0, sizeof(ReadCodeBuffer));
	
	cardQR = parm;
	memset(cardQR, 0, sizeof(CardCodeInfo));
		
	MotorMoveTo(0, 0);
	MotorMoveTo(500, 0);
		
	OpenCodeScanner();
	
		
	while(pdPASS == ReceiveDataFromQueue(GetUsart2RXQueue(), GetUsart2RXMutex(), &dir , 1, 1, 50 * portTICK_RATE_MS))
		;
		
	dir = 0;
	scanresult = CardCodeScanning;
	
	while(scanresult == CardCodeScanning)
	{
		if(GetCardState() == NoCard)				//卡被拔出
		{
			scanresult = CardCodeCardOut;
			break;
		}
		else
		{
			if(dir%2 == 0)
				MotorMoveTo(GetGB_MotorLocation()+2, 0);
			else
				MotorMoveTo(GetGB_MotorLocation()-2, 0);

			count++;
			
			ReadBasicCodeData();
			
			if((GetGB_MotorLocation() >= 1000)||(GetGB_MotorLocation() <= 200))
			{
				dir++;
				
			}
			
			vTaskDelay(2 / portTICK_RATE_MS);
			if(count >= (ScanOutTime*1000/3))
				scanresult = CardCodeScanTimeOut;				
			
		}
	}

	CloseCodeScanner();
	
	MyFree(S_ReadCodeBuffer);
	
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
static void ReadBasicCodeData(void)
{
	S_ReadCodeBuffer->rxcount = 0;
	
	while(pdPASS == ReceiveDataFromQueue(GetUsart2RXQueue(), GetUsart2RXMutex(), (S_ReadCodeBuffer->originalcode+S_ReadCodeBuffer->rxcount) , 1, 1, 10 / portTICK_RATE_MS))	
		S_ReadCodeBuffer->rxcount++;

	if(S_ReadCodeBuffer->rxcount > 0)
		AnalysisCode(S_ReadCodeBuffer->originalcode, S_ReadCodeBuffer->rxcount);
}

/***************************************************************************************************
*FunctionName：DecryptCode
*Description：解码并解析二维码数据
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月25日14:08:25
***************************************************************************************************/
static void AnalysisCode(void *pbuf , unsigned short len)
{
	unsigned short datalen = 0;
	unsigned char j=0, i=0;
	
	/*清空二维码空间*/
	memset(S_ReadCodeBuffer->decryptcode, 0, 320);

	/*数据解密失败*/
	if(pdFAIL == MyDencrypt(pbuf, S_ReadCodeBuffer->decryptcode, len))
		goto END;
	
	memcpy(S_ReadCodeBuffer->originalcode, S_ReadCodeBuffer->decryptcode, len);
	S_ReadCodeBuffer->pbuf2 = S_ReadCodeBuffer->originalcode;

	/*获取检测卡二维码信息存放地址*/
	if(NULL == cardQR)
		goto END;
	
	/*获取数据头*/
	S_ReadCodeBuffer->pbuf1 = strtok(S_ReadCodeBuffer->decryptcode, "#");
	if(S_ReadCodeBuffer->pbuf1)
	{
		if(0 != strcmp(S_ReadCodeBuffer->pbuf1, "AB"))
			goto END;
		else
			S_ReadCodeBuffer->pbuf2 += 3;
	}
	else
		goto END;
	
	/*获取数据长度*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL, "#");
	if(S_ReadCodeBuffer->pbuf1)
	{
		datalen = strtol(S_ReadCodeBuffer->pbuf1 , NULL , 10);
		S_ReadCodeBuffer->pbuf2 += (strlen(S_ReadCodeBuffer->pbuf1)+1);
	}
	else
		goto END;
	
	/*获取测试项目名称*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL, "#");
	if(S_ReadCodeBuffer->pbuf1)
		memcpy(cardQR->ItemName, S_ReadCodeBuffer->pbuf1 ,strlen(S_ReadCodeBuffer->pbuf1));
	else
		goto END;
		
	/*读取检测卡上的检测指标正常范围*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
		memcpy(cardQR->NormalResult, S_ReadCodeBuffer->pbuf1 ,strlen(S_ReadCodeBuffer->pbuf1));
	else
		goto END;
		
	/*读取检测卡上的最低检测值*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
		cardQR->LowstResult = strtod(S_ReadCodeBuffer->pbuf1 , NULL );
	else
		goto END;
		
	/*读取检测卡上的最高检测值*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
		cardQR->HighestResult = strtod(S_ReadCodeBuffer->pbuf1 , NULL );
	else
		goto END;	
		
	/*读取测试项目的单位*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
		memcpy(cardQR->ItemMeasure, S_ReadCodeBuffer->pbuf1 ,strlen(S_ReadCodeBuffer->pbuf1));
	else
		goto END;
	
	//读取小数点个数
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
		cardQR->ItemPoint = strtol(S_ReadCodeBuffer->pbuf1 , NULL, 10);
	else
		goto END;
	
	/*读取检测卡T线位置*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
		cardQR->ItemLocation = strtol(S_ReadCodeBuffer->pbuf1 , NULL, 10);
	else
		goto END;
		
	/*读取检测卡标准曲线临界浓度*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
		cardQR->ItemFenDuan = strtod(S_ReadCodeBuffer->pbuf1 , NULL);
	else
		goto END;

	/*标准曲线1*/
	for(i=0; i<4; i++)
	{
		S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
		if(S_ReadCodeBuffer->pbuf1)
			cardQR->ItemBiaoQu[0][i] = strtod(S_ReadCodeBuffer->pbuf1 , NULL);
		else
			goto END;
	}
	
	/*标准曲线1*/
	if(cardQR->ItemFenDuan > 0)
	{
		for(i=0; i<4; i++)
		{
			S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
			if(S_ReadCodeBuffer->pbuf1)
				cardQR->ItemBiaoQu[1][i] = strtod(S_ReadCodeBuffer->pbuf1 , NULL);
			else
				goto END;
		}
	}
		
	/*读取检测卡反应时间*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
		cardQR->CardWaitTime = strtol(S_ReadCodeBuffer->pbuf1 , NULL , 10);
	else
		goto END;
		
	/*读取检测卡C线位置*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
		cardQR->CLineLocation = strtol(S_ReadCodeBuffer->pbuf1 , NULL , 10);
	else
		goto END;


	/*读取检测卡批号*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
		memcpy(cardQR->CardPiCi, S_ReadCodeBuffer->pbuf1, strlen(S_ReadCodeBuffer->pbuf1));
	else
		goto END;

	/*读取检测卡保质期*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
	{
		static char year[10] ,month[10],day[10];
		memcpy(year, S_ReadCodeBuffer->pbuf1, 2);
		cardQR->CardBaoZhiQi.year = strtol(year , NULL , 10);

		memcpy(month, &S_ReadCodeBuffer->pbuf1[4], 2);
		cardQR->CardBaoZhiQi.month = (unsigned char)strtod(month , NULL );

		memcpy(day, &S_ReadCodeBuffer->pbuf1[6], 2);
		cardQR->CardBaoZhiQi.day = (unsigned char)strtod(day , NULL );
	}
	else
		goto END;

	/*读取二维码CRC*/
	S_ReadCodeBuffer->pbuf1 = strtok(NULL , "#");
	if(S_ReadCodeBuffer->pbuf1)
	{
		cardQR->CRC16 = strtol(S_ReadCodeBuffer->pbuf1 , NULL , 10);
			
		datalen -= strlen(S_ReadCodeBuffer->pbuf1);
		goto END;
	}
	else
		goto END;
	
	END:
		if(cardQR->CRC16 != CalModbusCRC16Fun1(S_ReadCodeBuffer->pbuf2 , datalen))
			scanresult = CardCodeCRCError;		
		else if(My_Fail == CheckCardIsTimeOut(cardQR))
			scanresult = CardCodeTimeOut;
		else
			scanresult = CardCodeScanOK;
		
}


static MyState_TypeDef CheckCardIsTimeOut(CardCodeInfo * s_CardCodeInfo)
{
	if(S_ReadCodeBuffer)
	{
		GetGB_Time(&(S_ReadCodeBuffer->temptime));
		
		if(s_CardCodeInfo->CardBaoZhiQi.year == S_ReadCodeBuffer->temptime.year)
		{
			if(s_CardCodeInfo->CardBaoZhiQi.month == S_ReadCodeBuffer->temptime.month)
			{
				if(s_CardCodeInfo->CardBaoZhiQi.day >= S_ReadCodeBuffer->temptime.day)
					return My_Pass;
			}
			else if(s_CardCodeInfo->CardBaoZhiQi.month > S_ReadCodeBuffer->temptime.month)
				return My_Pass;
		}
		else if(s_CardCodeInfo->CardBaoZhiQi.year > S_ReadCodeBuffer->temptime.year)
			return My_Pass;
	}
	
	return My_Fail;
}

