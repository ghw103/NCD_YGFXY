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
#include	"System_Data.h"
#include	"RemoteSoftDao.h"
#include	"RemoteSoft_Data.h"

#include	"MyMem.h"
#include	"CRC16.h"

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static MyState_TypeDef ReadTime(void);
static void UpLoadDeviceInfo(void);
static void UpLoadTestData(void);
static void readRemoteFirmwareVersion(void);
static void DownLoadFirmware(void);
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
			
			readRemoteFirmwareVersion();
			vTaskDelay(1000 / portTICK_RATE_MS);
			
			DownLoadFirmware();
			vTaskDelay(1000 / portTICK_RATE_MS);
				
			UpLoadTestData();	
			vTaskDelay(1000 / portTICK_RATE_MS);
		}

		vTaskDelay(60000 / portTICK_RATE_MS);
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
		
		copyGBSystemSetData(&(upLoadDeviceDataBuffer->systemSetData));
		
		if(upLoadDeviceDataBuffer->systemSetData.crc == CalModbusCRC16Fun1(&(upLoadDeviceDataBuffer->systemSetData), sizeof(SystemSetData) - 2))
		{
			sprintf(upLoadDeviceDataBuffer->sendBuf, "did=%s", upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceid);
		
			if(My_Pass == UpLoadData("/NCD_Server/up_dtime", upLoadDeviceDataBuffer->sendBuf, strlen(upLoadDeviceDataBuffer->sendBuf),
				upLoadDeviceDataBuffer->recvBuf, SERVERRECVBUFLEN, "POST"))
			{
				RTC_SetTimeData2(upLoadDeviceDataBuffer->recvBuf+7);
				
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
		
		copyGBSystemSetData(&(upLoadDeviceDataBuffer->systemSetData));
		
		if(upLoadDeviceDataBuffer->systemSetData.crc == CalModbusCRC16Fun1(&(upLoadDeviceDataBuffer->systemSetData), sizeof(SystemSetData) - 2))
		{
			if(upLoadDeviceDataBuffer->systemSetData.deviceInfo.isnew)
			{
				memset(upLoadDeviceDataBuffer->sendBuf, 0, UPLOADTEMPBUFLEN);

				sprintf(upLoadDeviceDataBuffer->sendBuf, "did=%s&dversion=%d&addr=%s&name=%s&age=%s&sex=%s&phone=%s&job=%s&dsc=%s&status=ok",
					upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceid,  GB_SoftVersion, upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceunit, 
					upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_name, upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_age, 
					upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_sex,	upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_phone, 
					upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_job, upLoadDeviceDataBuffer->systemSetData.deviceInfo.deviceuser.user_desc);
					
				if(My_Pass == UpLoadData("/NCD_Server/up_device", upLoadDeviceDataBuffer->sendBuf, strlen(upLoadDeviceDataBuffer->sendBuf), upLoadDeviceDataBuffer->recvBuf,
					SERVERRECVBUFLEN, "POST"))
				{
					copyGBSystemSetData(&(upLoadDeviceDataBuffer->systemSetData));
					
					upLoadDeviceDataBuffer->systemSetData.deviceInfo.isnew = false ;
					
					//保存并更新内存中的数据
					SaveSystemSetData(&(upLoadDeviceDataBuffer->systemSetData));
				}
			}
		}
	}
	
	MyFree(upLoadDeviceDataBuffer);
}

/***************************************************************************************************
*FunctionName: UpLoadTestData
*Description: 上传测试数据（数据和曲线）
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月20日16:43:44
***************************************************************************************************/
static void UpLoadTestData(void)
{
	UpLoadTestDataBuffer * upLoadTestDataBuffer = NULL;
	
	upLoadTestDataBuffer = MyMalloc(sizeof(UpLoadTestDataBuffer));
	
	if(upLoadTestDataBuffer)
	{
		memset(upLoadTestDataBuffer, 0, sizeof(UpLoadTestDataBuffer));
		
		//读取设备信息
		copyGBSystemSetData(&(upLoadTestDataBuffer->systemSetData));
		
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
				
				sprintf(upLoadTestDataBuffer->sendBuf, "cnum=%s&cid=%s&did=%s&t_name=%s&sid=%s&testtime=20%d-%d-%d %d:%d:%d&e_t=%.1f&o_t=%.1f&outt=%d&c_l=%d&t_l=%d&b_l=%d&t_c_v=%.3f&a_p=%.3f&b_v=%.*f&a_v=%.*f&t_re=%s",
					upLoadTestDataBuffer->testData->temperweima.piNum, upLoadTestDataBuffer->testData->temperweima.PiHao, 
					upLoadTestDataBuffer->systemSetData.deviceInfo.deviceid, upLoadTestDataBuffer->testData->user.user_name, 
					upLoadTestDataBuffer->testData->sampleid, upLoadTestDataBuffer->testData->TestTime.year, 
					upLoadTestDataBuffer->testData->TestTime.month, upLoadTestDataBuffer->testData->TestTime.day, 
					upLoadTestDataBuffer->testData->TestTime.hour, upLoadTestDataBuffer->testData->TestTime.min, 
					upLoadTestDataBuffer->testData->TestTime.sec, upLoadTestDataBuffer->testData->TestTemp.E_Temperature, 
					upLoadTestDataBuffer->testData->TestTemp.O_Temperature, upLoadTestDataBuffer->testData->time, 
					upLoadTestDataBuffer->testData->testline.C_Point[1], upLoadTestDataBuffer->testData->testline.T_Point[1],
					upLoadTestDataBuffer->testData->testline.B_Point[1], upLoadTestDataBuffer->testData->testline.BasicBili, 
					upLoadTestDataBuffer->testData->tempadjust.parm, upLoadTestDataBuffer->testData->temperweima.itemConstData.pointNum, 
					upLoadTestDataBuffer->testData->testline.BasicResult, upLoadTestDataBuffer->testData->temperweima.itemConstData.pointNum, 
					upLoadTestDataBuffer->testData->testline.AdjustResult,
					"ok");

				if(My_Pass != UpLoadData("/NCD_Server/up_testdata", upLoadTestDataBuffer->sendBuf, strlen(upLoadTestDataBuffer->sendBuf), 
					upLoadTestDataBuffer->recvBuf, UPLOADRECVBUFLEN, "POST"))
					break;
				
				//上传测试曲线
				for(upLoadTestDataBuffer->i=0; upLoadTestDataBuffer->i<3; upLoadTestDataBuffer->i++)
				{
					memset(upLoadTestDataBuffer->sendBuf, 0, UPLOADSENDBUFLEN);
					sprintf(upLoadTestDataBuffer->sendBuf, "cnum=%s&cid=%s&", upLoadTestDataBuffer->testData->temperweima.piNum, upLoadTestDataBuffer->testData->temperweima.PiHao);
					
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
						upLoadTestDataBuffer->recvBuf, UPLOADRECVBUFLEN, "POST"))
						goto END1;
				}
			}
			
			upLoadTestDataBuffer->systemSetData.upLoadIndex++;
			upLoadTestDataBuffer->testData++;
		}
		
		//上传曲线失败，则跳到此处，当前数据下次重新上传
		END1:
		
		//更新系统设置数据中的上传索引数据
		setUpLoadIndex(upLoadTestDataBuffer->systemSetData.upLoadIndex);
		//持久化系统设置数据
		SaveSystemSetData(getGBSystemSetData());
	}
	
	END:
		MyFree(upLoadTestDataBuffer);

}

/***************************************************************************************************
*FunctionName: 
*Description: 
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static void readRemoteFirmwareVersion(void)
{
	UpLoadDeviceDataBuffer * upLoadDeviceDataBuffer = NULL;
	MyState_TypeDef status = My_Fail;
	
	upLoadDeviceDataBuffer = MyMalloc(sizeof(UpLoadDeviceDataBuffer));
	
	if(upLoadDeviceDataBuffer)
	{
		memset(upLoadDeviceDataBuffer, 0, sizeof(UpLoadDeviceDataBuffer));
		
		sprintf(upLoadDeviceDataBuffer->sendBuf, "a");
		
		if(My_Pass == UpLoadData("/NCD_Server/deviceSoftInfo", upLoadDeviceDataBuffer->sendBuf, strlen(upLoadDeviceDataBuffer->sendBuf),
			upLoadDeviceDataBuffer->recvBuf, SERVERRECVBUFLEN, "POST"))
		{
			//解析最新固件版本
			upLoadDeviceDataBuffer->remoteSoftInfo.RemoteFirmwareVersion = strtol(upLoadDeviceDataBuffer->recvBuf+16, NULL, 10);
			
			//如果读取到的版本，大于当前版本，且大于当前保存的最新远程版本，则此次读取的是最新的
			if((upLoadDeviceDataBuffer->remoteSoftInfo.RemoteFirmwareVersion > GB_SoftVersion) &&
				(upLoadDeviceDataBuffer->remoteSoftInfo.RemoteFirmwareVersion > getGbRemoteFirmwareVersion()))
			{
				//解析最新固件MD5
				upLoadDeviceDataBuffer->tempP = strtok(upLoadDeviceDataBuffer->recvBuf, "#");
				if(upLoadDeviceDataBuffer->tempP)
				{
					upLoadDeviceDataBuffer->tempP = strtok(NULL, "#");
					
					memcpy(upLoadDeviceDataBuffer->remoteSoftInfo.md5, upLoadDeviceDataBuffer->tempP+4, 32);
					
					if(My_Pass == WriteRemoteSoftInfo(&(upLoadDeviceDataBuffer->remoteSoftInfo)))
					{
						//md5保存成功后，才更新最新版本号，保存最新固件版本
						setGbRemoteFirmwareVersion(upLoadDeviceDataBuffer->remoteSoftInfo.RemoteFirmwareVersion);
						setGbRemoteFirmwareMd5(upLoadDeviceDataBuffer->remoteSoftInfo.md5);
						
						setIsSuccessDownloadFirmware(false);
					}
				}
			}	
		}
	}
	MyFree(upLoadDeviceDataBuffer);
}

static void DownLoadFirmware(void)
{
	UpLoadDeviceDataBuffer * upLoadDeviceDataBuffer = NULL;
	MyState_TypeDef status = My_Fail;
	
	//检查是否有更新，且未成功下载，则需要下载
	if((getGbRemoteFirmwareVersion() > GB_SoftVersion) && (false == getIsSuccessDownloadFirmware()))
	{
		upLoadDeviceDataBuffer = MyMalloc(sizeof(UpLoadDeviceDataBuffer));
	
		if(upLoadDeviceDataBuffer)
		{
			memset(upLoadDeviceDataBuffer, 0, sizeof(UpLoadDeviceDataBuffer));
			
			UpLoadData("/NCD_Server/deviceCodeDownload", NULL, 0, NULL, 0, "GET");
		}
		MyFree(upLoadDeviceDataBuffer);
	}
}
