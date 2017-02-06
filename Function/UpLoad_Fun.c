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
static void UpLoadDeviceInfo(void);
static void UpLoadTestData(void);
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
			vTaskDelay(1000 / portTICK_RATE_MS);
			
			UpLoadDeviceInfo();

			vTaskDelay(1000 / portTICK_RATE_MS);
				
			UpLoadTestData();
				
			vTaskDelay(10000 / portTICK_RATE_MS);
		}
		else
			vTaskDelay(10000 / portTICK_RATE_MS);
	}
}

static MyState_TypeDef ReadTime(void)
{
	UpLoadDeviceDataBuffer * upLoadDeviceDataBuffer = NULL;
	MyState_TypeDef status = My_Fail;
	
	upLoadDeviceDataBuffer = MyMalloc(sizeof(UpLoadDeviceDataBuffer));
	
	if(upLoadDeviceDataBuffer)
	{
		memset(upLoadDeviceDataBuffer, 0, sizeof(UpLoadDeviceDataBuffer));
		
		getSystemSetData(&(upLoadDeviceDataBuffer->systemSetData));
		
		if(upLoadDeviceDataBuffer->systemSetData.crc == CalModbusCRC16Fun1(&(upLoadDeviceDataBuffer->systemSetData), sizeof(SystemSetData) - 2))
		{
			sprintf(upLoadDeviceDataBuffer->sendBuf, "did=%s", upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceid);
		
			if(My_Pass == UpLoadData("/NCD_Server/up_dtime", upLoadDeviceDataBuffer->sendBuf, strlen(upLoadDeviceDataBuffer->sendBuf),
				upLoadDeviceDataBuffer->recvBuf, SERVERRECVBUFLEN))
			{
				RTC_SetTimeData2(upLoadDeviceDataBuffer->recvBuf+7);
				SetGB_LineNetStatus(1);
				status = My_Pass;
			}
		}
	}
	MyFree(upLoadDeviceDataBuffer);
	
	return status;
}

static void UpLoadDeviceInfo(void)
{
	UpLoadDeviceDataBuffer * upLoadDeviceDataBuffer = NULL;
	
	upLoadDeviceDataBuffer = MyMalloc(sizeof(UpLoadDeviceDataBuffer));
	
	if(upLoadDeviceDataBuffer)
	{
		memset(upLoadDeviceDataBuffer, 0, sizeof(UpLoadDeviceDataBuffer));
		
		getSystemSetData(&(upLoadDeviceDataBuffer->systemSetData));
		
		if(upLoadDeviceDataBuffer->systemSetData.crc == CalModbusCRC16Fun1(&(upLoadDeviceDataBuffer->systemSetData), sizeof(SystemSetData) - 2))
		{
			if(upLoadDeviceDataBuffer->systemSetData.deviceInfo.isnew)
			{
				memset(upLoadDeviceDataBuffer->sendBuf, 0, UPLOADTEMPBUFLEN);

				sprintf(upLoadDeviceDataBuffer->sendBuf, "did=%s&addr=%s&name=%s&age=%s&sex=%s&phone=%s&job=%s&dsc=%s&status=ok",
					upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceid,  upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceunit, 
					upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_name, upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_age, 
					upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_sex,	upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_phone, 
					upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_job, upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_desc);
					
				if(My_Pass == UpLoadData("/NCD_Server/up_device", upLoadDeviceDataBuffer->sendBuf, strlen(upLoadDeviceDataBuffer->sendBuf), upLoadDeviceDataBuffer->recvBuf,
					SERVERRECVBUFLEN))
				{
					upLoadDeviceDataBuffer->systemSetData.deviceInfo.isnew = false ;
						
					if(My_Pass == SaveSystemSetData(&(upLoadDeviceDataBuffer->systemSetData)))
					{
						setSystemSetData(&(upLoadDeviceDataBuffer->systemSetData));
					}
				}
			}
		}
	}
	
	MyFree(upLoadDeviceDataBuffer);
}

static void UpLoadTestData(void)
{
	UpLoadTestDataBuffer * upLoadTestDataBuffer = NULL;
	
	upLoadTestDataBuffer = MyMalloc(sizeof(UpLoadTestDataBuffer));
	
	if(upLoadTestDataBuffer)
	{
		memset(upLoadTestDataBuffer, 0, sizeof(UpLoadTestDataBuffer));
		
		//读取设备信息
		getSystemSetData(&(upLoadTestDataBuffer->systemSetData));
		
		//检测数据头是否校验正确，且有数据待发送
		if(upLoadTestDataBuffer->systemSetData.upLoadIndex >= upLoadTestDataBuffer->systemSetData.testDataNum)
			goto END;
		
		//读取测试数据,读取失败则退出
		upLoadTestDataBuffer->pageRequest.startElementIndex = upLoadTestDataBuffer->systemSetData.upLoadIndex;
		upLoadTestDataBuffer->pageRequest.orderType = DESC;
		upLoadTestDataBuffer->pageRequest.pageSize = 1;

		if(My_Pass != ReadTestData(&(upLoadTestDataBuffer->pageRequest), &(upLoadTestDataBuffer->page), &(upLoadTestDataBuffer->systemSetData)))
			goto END;
		
		upLoadTestDataBuffer->testData = upLoadTestDataBuffer->page.testData;
		for(upLoadTestDataBuffer->k=0; upLoadTestDataBuffer->k< upLoadTestDataBuffer->page.ElementsSize; upLoadTestDataBuffer->k++)
		{
			//如果crc校验正确，则开始上传
			if(upLoadTestDataBuffer->testData->crc == CalModbusCRC16Fun1(upLoadTestDataBuffer->testData, sizeof(TestData)-2))
			{
				//上传测试数据
				memset(upLoadTestDataBuffer->sendBuf, 0, UPLOADSENDBUFLEN);
				
				sprintf(upLoadTestDataBuffer->sendBuf, "cnum=%s&card.cid=%s&device.did=%s&t_name=%s&sid=%s&testtime=20%d-%d-%d %d:%d:%d&e_t=%.1f&o_t=%.1f&outt=%d&c_l=%d&t_l=%d&b_l=%d&t_c_v=%.3f&a_p=%.3f&b_v=%.3f&a_v=%.3f&t_re=%s",
					upLoadTestDataBuffer->testData->temperweima.piNum, upLoadTestDataBuffer->testData->temperweima.PiHao, upLoadTestDataBuffer->systemSetData.deviceInfo.deviceid, upLoadTestDataBuffer->testData->user.user_name, upLoadTestDataBuffer->testData->sampleid,
					upLoadTestDataBuffer->testData->TestTime.year, upLoadTestDataBuffer->testData->TestTime.month, upLoadTestDataBuffer->testData->TestTime.day, upLoadTestDataBuffer->testData->TestTime.hour, upLoadTestDataBuffer->testData->TestTime.min, upLoadTestDataBuffer->testData->TestTime.sec,
					upLoadTestDataBuffer->testData->TestTemp.E_Temperature, upLoadTestDataBuffer->testData->TestTemp.O_Temperature, upLoadTestDataBuffer->testData->time, upLoadTestDataBuffer->testData->testline.C_Point[1], upLoadTestDataBuffer->testData->testline.T_Point[1],
					upLoadTestDataBuffer->testData->testline.B_Point[1], upLoadTestDataBuffer->testData->testline.BasicBili, upLoadTestDataBuffer->testData->tempadjust.parm, upLoadTestDataBuffer->testData->testline.BasicResult, upLoadTestDataBuffer->testData->testline.AdjustResult,
					"ok");

				if(My_Pass != UpLoadData("/NCD_Server/up_testdata", upLoadTestDataBuffer->sendBuf, strlen(upLoadTestDataBuffer->sendBuf), 
					upLoadTestDataBuffer->recvBuf, UPLOADRECVBUFLEN))
					break;
				
				//上传测试曲线
				for(upLoadTestDataBuffer->i=0; upLoadTestDataBuffer->i<3; upLoadTestDataBuffer->i++)
				{
					memset(upLoadTestDataBuffer->sendBuf, 0, UPLOADSENDBUFLEN);
					sprintf(upLoadTestDataBuffer->sendBuf, "cnum=%s&card.cid=%s&", upLoadTestDataBuffer->testData->temperweima.piNum, upLoadTestDataBuffer->testData->temperweima.PiHao);
					
					memset(upLoadTestDataBuffer->tempBuf, 0, UPLOADTEMPBUFLEN);
					if(upLoadTestDataBuffer->i == 0)
						sprintf(upLoadTestDataBuffer->tempBuf, "serie_a=[");
					else if(upLoadTestDataBuffer->i == 1)
						sprintf(upLoadTestDataBuffer->tempBuf, "serie_b=[");
					else
						sprintf(upLoadTestDataBuffer->tempBuf, "serie_c=[");
					strcat(upLoadTestDataBuffer->sendBuf, upLoadTestDataBuffer->tempBuf);
					
					for(upLoadTestDataBuffer->j=0; upLoadTestDataBuffer->j<100; upLoadTestDataBuffer->j++)
					{
						memset(upLoadTestDataBuffer->tempBuf, 0, UPLOADTEMPBUFLEN);
						if(upLoadTestDataBuffer->j == 0)
							sprintf(upLoadTestDataBuffer->tempBuf, "%d", upLoadTestDataBuffer->testData->testline.TestPoint[upLoadTestDataBuffer->i*100 + upLoadTestDataBuffer->j]);
						else
							sprintf(upLoadTestDataBuffer->tempBuf, ",%d", upLoadTestDataBuffer->testData->testline.TestPoint[upLoadTestDataBuffer->i*100 + upLoadTestDataBuffer->j]);
						strcat(upLoadTestDataBuffer->sendBuf, upLoadTestDataBuffer->tempBuf);
					}
					
					sprintf(upLoadTestDataBuffer->tempBuf, "]");
					strcat(upLoadTestDataBuffer->sendBuf, upLoadTestDataBuffer->tempBuf);
					

					if(My_Pass != UpLoadData("/NCD_Server/up_series", upLoadTestDataBuffer->sendBuf, strlen(upLoadTestDataBuffer->sendBuf),
						upLoadTestDataBuffer->recvBuf, UPLOADRECVBUFLEN))
						break;
				}
			}
			
			upLoadTestDataBuffer->systemSetData.upLoadIndex++;
			upLoadTestDataBuffer->testData++;
		}
		
		//更新系统设置数据中的上传索引数据
		setUpLoadIndex(upLoadTestDataBuffer->systemSetData.upLoadIndex);
		//持久化系统设置数据
		persistSystemSetData();
	}
	
	END:
		MyFree(upLoadTestDataBuffer);

}
