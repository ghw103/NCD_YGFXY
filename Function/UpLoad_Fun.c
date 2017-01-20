/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"UpLoad_Fun.h"
#include	"ServerFun.h"
#include	"RTC_Driver.h"
#include	"SystemSet_Dao.h"
#include	"NetInfo_Data.h"

#include	"MyMem.h"
#include	"CRC16.h"

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	"stdio.h"
#include	"string.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static MyState_TypeDef ReadTime(void);
static MyState_TypeDef UpLoadDeviceInfo(void);
static MyState_TypeDef UpLoadTestData(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void UpLoadFunction(void)
{
	while(1)
	{
		if(My_Pass == ReadTime())
		{
			vTaskDelay(100 / portTICK_RATE_MS);
			
//			UpLoadDeviceInfo();
			
			
//			vTaskDelay(1000 / portTICK_RATE_MS);
				
//			UpLoadTestData();
				
//			vTaskDelay(10000 / portTICK_RATE_MS);
		}
		else
			vTaskDelay(100 / portTICK_RATE_MS);
	}
}

static MyState_TypeDef ReadTime(void)
{
	char * buf = NULL;
	SystemSetData * systemSetData = NULL;
	MyState_TypeDef status = My_Fail;
	
	buf = MyMalloc(100);
	systemSetData = MyMalloc(sizeof(SystemSetData));
	
	if(buf && systemSetData)
	{
		memset(buf, 0, 100);
		
		getSystemSetData(systemSetData);
		
		if(systemSetData->crc == CalModbusCRC16Fun1(systemSetData, sizeof(SystemSetData) - 2))
		{
			sprintf(buf, "did=%s", systemSetData->deviceInfo.deviceid);
		
			if(My_Pass == UpLoadData("/NCD_Server/up_dtime", buf, 2))//strlen(buf)))
			{
				RTC_SetTimeData2(buf+7);
				SetGB_LineNetStatus(1);
				status = My_Pass;
			}
		}
	}
	MyFree(buf);
	MyFree(systemSetData);
	
	return status;
}

static MyState_TypeDef UpLoadDeviceInfo(void)
{
	MyState_TypeDef status = My_Fail;

	SystemSetData * systemSetData = NULL;
	char * buf = NULL;
	static unsigned char i=0;
	
	systemSetData = MyMalloc(sizeof(SystemSetData));
	
	if(systemSetData)
	{
		getSystemSetData(systemSetData);
		
		if(systemSetData->crc == CalModbusCRC16Fun1(systemSetData, sizeof(SystemSetData) - 2)) 
		{
			if(systemSetData->deviceInfo.isnew)
			{
				buf = MyMalloc(2048);
				
				if(buf)
				{
					memset(buf, 0, 2048);

					sprintf(buf, "did=%s&addr=%s&name=%s&age=%s&sex=%s&phone=%s&job=%s&dsc=%s&status=ok",
						systemSetData->deviceInfo.deviceid,  systemSetData->deviceInfo.deviceunit, systemSetData->deviceInfo.deviceuser.user_name, systemSetData->deviceInfo.deviceuser.user_age+i, 
						systemSetData->deviceInfo.deviceuser.user_sex,	systemSetData->deviceInfo.deviceuser.user_phone, systemSetData->deviceInfo.deviceuser.user_job, systemSetData->deviceInfo.deviceuser.user_desc);
					
					if(My_Pass == UpLoadData("/NCD_Server/up_device", buf, strlen(buf)))
					{
						i++;
						systemSetData->deviceInfo.isnew = false ;
						
						if(My_Pass == SaveSystemSetData(systemSetData))
						{
							setSystemSetData(systemSetData);
							status = My_Pass;
						}
					}
					MyFree(buf);
				}
			}
			else
				status = My_Pass;
		}
		
		MyFree(systemSetData);
		
		return status;
	}
	else
		return My_Fail;
}

static MyState_TypeDef UpLoadTestData(void)
{
	MyState_TypeDef statues = My_Fail;
	unsigned short i=0, j;
	
	UpLoadTestDataBuffer * myUpLoadTestDataBuffer = NULL;
	
	myUpLoadTestDataBuffer = MyMalloc(sizeof(UpLoadTestDataBuffer));
	
	if(myUpLoadTestDataBuffer)
	{
		memset(myUpLoadTestDataBuffer, 0, sizeof(UpLoadTestDataBuffer));
		
		//读取设备信息
		getSystemSetData(&(myUpLoadTestDataBuffer->systemSetData));
		
		//读取测试数据头,失败则退出
		myUpLoadTestDataBuffer->readTestDataPackage.startReadIndex = 0;
		myUpLoadTestDataBuffer->readTestDataPackage.maxReadNum = 0;
		if(My_Pass != ReadTestData(&(myUpLoadTestDataBuffer->readTestDataPackage)))
			goto END;
		
		//检测数据头是否校验正确，且有数据待发送
		if((myUpLoadTestDataBuffer->readTestDataPackage.testDataHead.crc != CalModbusCRC16Fun1(&(myUpLoadTestDataBuffer->readTestDataPackage.testDataHead), sizeof(TestDataHead)-2)) ||
				myUpLoadTestDataBuffer->readTestDataPackage.testDataHead.readindex >= myUpLoadTestDataBuffer->readTestDataPackage.testDataHead.datanum)
			goto END;
		
		//读取测试数据,读取失败则退出
		myUpLoadTestDataBuffer->readTestDataPackage.startReadIndex = myUpLoadTestDataBuffer->readTestDataPackage.testDataHead.readindex;
		myUpLoadTestDataBuffer->readTestDataPackage.maxReadNum = 1;
		if(My_Pass != ReadTestData(&(myUpLoadTestDataBuffer->readTestDataPackage)))
			goto END;
		
		//校验读取的数据的正确性,如果不正确，则上传索引+1，略过此数据
		if(myUpLoadTestDataBuffer->readTestDataPackage.testData[0].crc != CalModbusCRC16Fun1(&(myUpLoadTestDataBuffer->readTestDataPackage.testData[0]), sizeof(TestData)-2))
		{
			myUpLoadTestDataBuffer->readTestDataPackage.testDataHead.readindex += 1;
			WriteTestDataHead(&(myUpLoadTestDataBuffer->readTestDataPackage.testDataHead));
			
			statues = My_Pass;

			goto END;
		}
		
		//上传测试数据
		memset(myUpLoadTestDataBuffer->sendbuf, 0, 2048);
		myUpLoadTestDataBuffer->testData = &(myUpLoadTestDataBuffer->readTestDataPackage.testData[0]);
		
		sprintf(myUpLoadTestDataBuffer->sendbuf, "cnum=%s&card.cid=%s&device.did=%s&t_name=%s&sid=%s&testtime=20%d-%d-%d %d:%d:%d&e_t=%.1f&o_t=%.1f&outt=%d&c_l=%d&t_l=%d&b_l=%d&t_c_v=%.3f&a_p=%.3f&b_v=%.3f&a_v=%.3f&t_re=%s",
			myUpLoadTestDataBuffer->testData->temperweima.piNum, myUpLoadTestDataBuffer->testData->temperweima.PiHao, myUpLoadTestDataBuffer->systemSetData.deviceInfo.deviceid, myUpLoadTestDataBuffer->testData->user.user_name, myUpLoadTestDataBuffer->testData->sampleid,
			myUpLoadTestDataBuffer->testData->TestTime.year, myUpLoadTestDataBuffer->testData->TestTime.month, myUpLoadTestDataBuffer->testData->TestTime.day, myUpLoadTestDataBuffer->testData->TestTime.hour, myUpLoadTestDataBuffer->testData->TestTime.min, myUpLoadTestDataBuffer->testData->TestTime.sec,
			myUpLoadTestDataBuffer->testData->TestTemp.E_Temperature, myUpLoadTestDataBuffer->testData->TestTemp.O_Temperature, myUpLoadTestDataBuffer->testData->time, myUpLoadTestDataBuffer->testData->testline.C_Point[1], myUpLoadTestDataBuffer->testData->testline.T_Point[1],
			myUpLoadTestDataBuffer->testData->testline.B_Point[1], myUpLoadTestDataBuffer->testData->testline.BasicBili, myUpLoadTestDataBuffer->testData->tempadjust.parm, myUpLoadTestDataBuffer->testData->testline.BasicResult, myUpLoadTestDataBuffer->testData->testline.AdjustResult,
			"ok");

		if(My_Pass != UpLoadData("/NCD_Server/up_testdata", myUpLoadTestDataBuffer->sendbuf, strlen(myUpLoadTestDataBuffer->sendbuf)))
			goto END;
		
		//上传测试曲线
		for(i=0; i<3; i++)
		{
			memset(myUpLoadTestDataBuffer->sendbuf, 0, 1024);
			sprintf(myUpLoadTestDataBuffer->sendbuf, "cnum=%s&card.cid=%s&", myUpLoadTestDataBuffer->testData->temperweima.piNum, myUpLoadTestDataBuffer->testData->temperweima.PiHao);
			
			memset(myUpLoadTestDataBuffer->tempbuf, 0, 100);
			if(i == 0)
				sprintf(myUpLoadTestDataBuffer->tempbuf, "serie_a=[");
			else if(i == 1)
				sprintf(myUpLoadTestDataBuffer->tempbuf, "serie_b=[");
			else
				sprintf(myUpLoadTestDataBuffer->tempbuf, "serie_c=[");
			strcat(myUpLoadTestDataBuffer->sendbuf, myUpLoadTestDataBuffer->tempbuf);
			
			for(j=0; j<100; j++)
			{
				memset(myUpLoadTestDataBuffer->tempbuf, 0, 20);
				if(j == 0)
					sprintf(myUpLoadTestDataBuffer->tempbuf, "%d", myUpLoadTestDataBuffer->testData->testline.TestPoint[i*100+j]);
				else
					sprintf(myUpLoadTestDataBuffer->tempbuf, ",%d", myUpLoadTestDataBuffer->testData->testline.TestPoint[i*100+j]);
				strcat(myUpLoadTestDataBuffer->sendbuf, myUpLoadTestDataBuffer->tempbuf);
			}
			
			sprintf(myUpLoadTestDataBuffer->tempbuf, "]");
			strcat(myUpLoadTestDataBuffer->sendbuf, myUpLoadTestDataBuffer->tempbuf);
			

			if(My_Pass != UpLoadData("/NCD_Server/up_series", myUpLoadTestDataBuffer->sendbuf, strlen(myUpLoadTestDataBuffer->sendbuf)))
				goto END;
		}
		
		myUpLoadTestDataBuffer->readTestDataPackage.testDataHead.readindex += 1;
		WriteTestDataHead(&(myUpLoadTestDataBuffer->readTestDataPackage.testDataHead));
		statues = My_Pass;
	}
	
	END:
		MyFree(myUpLoadTestDataBuffer);

	return statues;
}
