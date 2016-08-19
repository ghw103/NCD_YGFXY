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
#include	"Time_Data.h"

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
static ScanQRTaskData * S_ScanQRTaskData;					//二维码扫描任务数据
static char buf[100];
static unsigned short rxcount = 0;
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
	
	S_ScanQRTaskData = parm;
	
	memset(S_ScanQRTaskData->cardQR, 0, sizeof(CardCodeInfo));
		
	MotorMoveTo(0, 0);
	MotorMoveTo(500, 0);
		
	OpenCodeScanner();
		
	while(pdPASS == ReceiveDataFromQueue(GetUsart2RXQueue(), GetUsart2RXMutex(), &dir , 1, 1, 50 * portTICK_RATE_MS))
		;
		
	dir = 0;
	S_ScanQRTaskData->scanresult = CardCodeScanning;
	
	while(S_ScanQRTaskData->scanresult == CardCodeScanning)
	{
		if(GetCardState() == NoCard)				//卡被拔出
		{
			S_ScanQRTaskData->scanresult = CardCodeCardOut;
			break;
		}
		else
		{
			if(dir%2 == 0)
				MotorMoveTo(GetGB_MotorLocation()+2, 0);
			else
				MotorMoveTo(GetGB_MotorLocation()-2, 0);

			count++;
					
			if((GetGB_MotorLocation() >= 1500)||(GetGB_MotorLocation() <= 500))
			{
				dir++;
				ReadBasicCodeData();
			}
			
			vTaskDelay(2 / portTICK_RATE_MS);
			if(count >= (ScanOutTime*1000/2))
				S_ScanQRTaskData->scanresult = CardCodeScanTimeOut;				
			
		}
	}

	CloseCodeScanner();
	
	return S_ScanQRTaskData->scanresult;
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
	rxcount = 0;
	
	while(pdPASS == ReceiveDataFromQueue(GetUsart2RXQueue(), GetUsart2RXMutex(), (buf+rxcount) , 1, 1, 10 / portTICK_RATE_MS))	
		rxcount++;

	if(rxcount > 0)
		AnalysisCode(buf, rxcount);
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
	char *basicrealbuf = NULL;									//解密后的原始数据
	char *p = NULL;												//p指向q，保留申请的内存地址，最后释放
	char *q = NULL;												//q作为strtok的输入
	char *tempstr = NULL;
	unsigned short datalen = 0;
	unsigned char j=0;
	
	basicrealbuf = MyMalloc(len);
	q = MyMalloc(len+20);
	if((basicrealbuf == NULL)||(q == NULL))
		goto END;
	
	/*清空二维码空间*/
	memset(basicrealbuf, 0, len);
	memset(q, 0, len+20);

	/*数据解密失败*/
	if(pdFAIL == MyDencrypt(pbuf, basicrealbuf, len))
		goto END;

	/*复制解密后的二维码数据*/
	memcpy(q, basicrealbuf, len);
	p = q;
	
	/*获取检测卡二维码信息存放地址*/
	if(NULL == S_ScanQRTaskData->cardQR)
		goto END;
	
	/*获取数据头*/
	tempstr = strtok(q, "#");
	if(tempstr)
	{
		if(0 != strcmp(tempstr, "AB"))
			goto END;
		else
			basicrealbuf += 3;
	}
	else
		goto END;
	
	/*获取数据长度*/
	tempstr = strtok(NULL, "#");
	if(tempstr)
	{
		datalen = strtol(tempstr , NULL , 10);
		basicrealbuf += (strlen(tempstr)+1);
	}
	else
		goto END;
	
	/*获取测试项目名称*/
	tempstr = strtok(NULL, "#");
	if(tempstr)
		memcpy(S_ScanQRTaskData->cardQR->ItemName, tempstr ,strlen(tempstr));
	else
		goto END;
		
	/*读取检测卡上的检测指标计算方式*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
		S_ScanQRTaskData->cardQR->TestType = strtol(tempstr , NULL , 10);
	else
		goto END;
		
	/*读取检测卡上的检测指标正常范围*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
		S_ScanQRTaskData->cardQR->NormalResult = strtod(tempstr , NULL );
	else
		goto END;
		
	/*读取检测卡上的最低检测值*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
		S_ScanQRTaskData->cardQR->LowstResult = strtod(tempstr , NULL );
	else
		goto END;
		
	/*读取检测卡上的最高检测值*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
		S_ScanQRTaskData->cardQR->HighestResult = strtod(tempstr , NULL );
	else
		goto END;	
		
	/*读取测试项目的单位*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
		memcpy(S_ScanQRTaskData->cardQR->ItemMeasure, tempstr ,strlen(tempstr));
	else
		goto END;
		
	/*读取检测卡T线位置*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
		//S_CardQRCodeInfo->ItemLocation = strtol(tempstr , NULL, 10);
	S_ScanQRTaskData->cardQR->ItemLocation = 172;
	else
		goto END;
		
	/*读取检测卡标准曲线数目*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
		S_ScanQRTaskData->cardQR->ItemBiaoQuNum = strtol(tempstr , NULL, 10);
	else
		goto END;
	
	if(S_ScanQRTaskData->cardQR->ItemBiaoQuNum > 1)
	{
		/*读取检测卡标准曲线临界浓度*/
		tempstr = strtok(NULL , "#");
		if(tempstr)
			S_ScanQRTaskData->cardQR->ItemFenDuan = strtod(tempstr , NULL);
		else
			goto END;
	}
	
		
	/*标准曲线*/
	for(j=0; j<S_ScanQRTaskData->cardQR->ItemBiaoQuNum; j++)
	{
		char *index = MyMalloc(50);
		
		tempstr = strtok(NULL , "#");
		
		if(tempstr && index)
		{
			memset(index, 0, 50);
			S_ScanQRTaskData->cardQR->ItemBiaoQu[j][0] = strtod(tempstr , &index );
				
			S_ScanQRTaskData->cardQR->ItemBiaoQu[j][1] = strtod(index , &index );
				
			S_ScanQRTaskData->cardQR->ItemBiaoQu[j][2] = strtod(index , &index );

			MyFree(index);
		}
		else
		{
			MyFree(index);
				
			goto END;
		}
	}
		
	/*读取检测卡反应时间*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
		S_ScanQRTaskData->cardQR->CardWaitTime = strtol(tempstr , NULL , 10);
	else
		goto END;
		
	/*读取检测卡C线位置*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
		S_ScanQRTaskData->cardQR->CLineLocation = 264;//strtol(tempstr , NULL , 10);
	else
		goto END;


	/*读取检测卡批号*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
		memcpy(S_ScanQRTaskData->cardQR->CardPiCi, tempstr, strlen(tempstr));
	else
		goto END;

	/*读取检测卡保质期*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
	{
		static char year[10] ,month[10],day[10];
		memcpy(year, tempstr, 4);
		S_ScanQRTaskData->cardQR->CardBaoZhiQi.RTC_Year = strtol(year , NULL , 10) - 2000;

		memcpy(month, &tempstr[4], 2);
		S_ScanQRTaskData->cardQR->CardBaoZhiQi.RTC_Month = (unsigned char)strtod(month , NULL );

		memcpy(day, &tempstr[6], 2);
		S_ScanQRTaskData->cardQR->CardBaoZhiQi.RTC_Date = (unsigned char)strtod(day , NULL );
	}
	else
		goto END;

	/*读取二维码CRC*/
	tempstr = strtok(NULL , "#");
	if(tempstr)
	{
		S_ScanQRTaskData->cardQR->CRC16 = strtol(tempstr , NULL , 10);
			
		datalen -= strlen(tempstr);
		goto END;
	}
	else
		goto END;
	
	END:
		if(S_ScanQRTaskData->cardQR->CRC16 != CalModbusCRC16Fun1(basicrealbuf , datalen))
			S_ScanQRTaskData->scanresult = CardCodeCRCError;		
		else if(My_Fail == CheckCardIsTimeOut(S_ScanQRTaskData->cardQR))
			S_ScanQRTaskData->scanresult = CardCodeTimeOut;
		else
			S_ScanQRTaskData->scanresult = CardCodeScanOK;
		
		MyFree(basicrealbuf);
		basicrealbuf = NULL;
		
		MyFree(p);
		p = NULL;
}


static MyState_TypeDef CheckCardIsTimeOut(CardCodeInfo * s_CardCodeInfo)
{
	MyTime_Def *temp = NULL;
	temp = MyMalloc(sizeof(MyTime_Def));
	
	if(temp)
	{
		GetGBTimeData(temp);
		
		if(s_CardCodeInfo->CardBaoZhiQi.RTC_Year == temp->year)
		{
			if(s_CardCodeInfo->CardBaoZhiQi.RTC_Month == temp->month)
			{
				if(s_CardCodeInfo->CardBaoZhiQi.RTC_Date >= temp->day)
					return My_Pass;
				else
					return My_Fail;
			}
			else if(s_CardCodeInfo->CardBaoZhiQi.RTC_Month > temp->month)
				return My_Pass;
			else
				return My_Fail;
		}
		else if(s_CardCodeInfo->CardBaoZhiQi.RTC_Year > temp->year)
			return My_Pass;
		else
			return My_Fail;
	}
	else
		return My_Fail;
	
	
}

