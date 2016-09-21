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
#include	"SDFunction.h"
#include	"System_Data.h"
#include	"RTC_Driver.h"

#include	"cJSON.h"
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
			vTaskDelay(1000 / portTICK_RATE_MS);
			
			if(My_Pass == UpLoadDeviceInfo())
			{
				vTaskDelay(1000 / portTICK_RATE_MS);
			}
			else
				vTaskDelay(1000 / portTICK_RATE_MS);
		}
		else
			vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

static MyState_TypeDef ReadTime(void)
{
	char * buf = NULL;
	DeviceInfo * deviceinfo = NULL;
	MyState_TypeDef status = My_Fail;
	
	buf = MyMalloc(100);
	deviceinfo = MyMalloc(sizeof(DeviceInfo));
	
	if(buf && deviceinfo)
	{
		memset(buf, 0, 100);
		
		//读取设备信息
		GetGB_DeviceInfo(deviceinfo);
		
		sprintf(buf, "deviceid=%s", deviceinfo->deviceid);
		
		if(My_Pass == UpLoadData("/NCD_YGFXY/rtime.action", buf, strlen(buf)))
		{
			RTC_SetTimeData2(buf+10);
			status = My_Pass;
		}
	}
	MyFree(buf);
	MyFree(deviceinfo);
	
	return status;
}

static MyState_TypeDef UpLoadDeviceInfo(void)
{
	DeviceInfo * deviceinfo = NULL;
	char * buf = NULL;
	MyState_TypeDef status = My_Fail;
	
	if(true == GetDeviceInIsFresh())
	{
		deviceinfo = MyMalloc(sizeof(DeviceInfo));
		buf = MyMalloc(2048);
		
		if(deviceinfo && buf)
		{
			//读取设备信息
			GetGB_DeviceInfo(deviceinfo);
			
			memset(buf, 0, 2048);

			sprintf(buf, "dfo.id=%s&dfo.daddr=%s&dfo.dname=%s&dfo.dage=%s&dfo.dsex=%s&dfo.dphone=%s&dfo.djob=%s&dfo.ddesc=%s&dfo.disok=true",
				deviceinfo->deviceid,  deviceinfo->deviceunit, deviceinfo->deviceuser.user_name, deviceinfo->deviceuser.user_age, deviceinfo->deviceuser.user_sex,
				deviceinfo->deviceuser.user_phone, deviceinfo->deviceuser.user_job, deviceinfo->deviceuser.user_desc);

			if(My_Pass == UpLoadData("/NCD_YGFXY/up_info.action", buf, strlen(buf)))
			{
				SetDeviceInIsFresh(false);
				status = My_Pass;
			}
		}
		
		MyFree(deviceinfo);
		MyFree(buf);
		
		return status;
	}
	else
		return My_Pass;
}

static MyState_TypeDef UpLoadTestData(void)
{
	TestData * testdata = NULL;
	char * sendbuf = NULL;
	DeviceInfo * deviceinfo = NULL;
	
	char *tempbuf = NULL;
	UpLoadIndex uploadindex;
	char *linebuf = NULL;
	unsigned short i = 0;
	
	MyState_TypeDef statues = My_Fail;
	
	deviceinfo = MyMalloc(sizeof(DeviceInfo));
	testdata = MyMalloc(sizeof(TestData));
	sendbuf = MyMalloc(4096);
	tempbuf = MyMalloc(2500);
	linebuf = MyMalloc(10);
	
	if(testdata && sendbuf && deviceinfo && linebuf)
	{
		if(My_Pass == ReadUpLoadIndex(&uploadindex))
		{
			if((My_Pass == ReadTestData(testdata, uploadindex.index, 1)) && (testdata->crc == CalModbusCRC16Fun1(testdata, sizeof(TestData)-2)) &&
				(My_Pass == ReadDeviceInfo(deviceinfo)) && (deviceinfo->crc == CalModbusCRC16Fun1(deviceinfo, sizeof(DeviceInfo)-2)) )
			{
				memset(sendbuf, 0, 4096);
				memset(tempbuf, 0, 2500);
				
				for(i=0; i<MaxPointLen; i++)
				{
					memset(linebuf, 0, 10);
					sprintf(linebuf, "%d#", testdata->testline.TestPoint[i]);
					strcat(tempbuf, linebuf);
				}
				
				if(tempbuf)
				{
					sprintf(sendbuf, "test_reaction_time=%d&temperature=%2.1f&temperature2=%2.1f&fluorescence_data=%s&Cposition=%d&Baseposition=%d&Tposition=%d&resultratio=%.3f&resultprimitive=%.3f&resultcalibration=%.3f&testSampleID=%s&DeviceID=%.5s&testCardID=%.5s",
						testdata->time, testdata->TestTemp.E_Temperature, testdata->TestTemp.O_Temperature, tempbuf, testdata->testline.C_Point[1], testdata->testline.B_Point[1],
						testdata->testline.T_Point[1],testdata->testline.BasicBili, testdata->testline.BasicResult, testdata->testline.AdjustResult, testdata->sampleid, deviceinfo->deviceid, testdata->temperweima.CardPiCi);

					if(My_Pass == UpLoadData("http://123.57.94.39/api/myFluorescenceData/", sendbuf, strlen(sendbuf)))
					{
						uploadindex.index++;
						WriteUpLoadIndex(uploadindex.index);
						statues = My_Pass;
					}
				}
			}
		}
	}
	
	MyFree(deviceinfo);
	MyFree(testdata);
	MyFree(tempbuf);
	MyFree(sendbuf);
	MyFree(linebuf);
	
	return statues;
}
