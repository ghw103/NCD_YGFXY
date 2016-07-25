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
#include	"Net_Data.h"

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
static void UpLoadDeviceInfo(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void UpLoadFunction(void)
{
	static unsigned int count = 0;
	while(1)
	{
		if((count % 10) == 0)
			UpLoadDeviceInfo();
		
//		count++;
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}


static void UpLoadDeviceInfo(void)
{
	DeviceInfo * deviceinfo = NULL;
	char * buf = NULL;
	
	deviceinfo = MyMalloc(sizeof(DeviceInfo));
	buf = MyMalloc(2048);
	MyGetFreeHeapSize();
	if(deviceinfo && buf)
	{
		if((My_Pass == ReadDeviceInfo(deviceinfo)) && (deviceinfo->crc == CalModbusCRC16Fun1(deviceinfo, sizeof(DeviceInfo)-2)) &&
			(deviceinfo->isfresh == 1))
		{
			memset(buf, 0, 2048);
			sprintf(buf, "id1=%s&units=%s&manufacture=%s&manufacturetel=%s&status=1&responsibel_man=%s&use_state=0&maintain=1",
				deviceinfo->deviceid, deviceinfo->deviceunit, deviceinfo->devicemaker, deviceinfo->devicemakerphone, deviceinfo->deviceuser.user_name);

			if(My_Pass == UpLoadData("http://123.57.94.39/api/myFluorescenceInfo/", buf, strlen(buf)))
			{
				deviceinfo->isfresh = 0;
				SaveDeviceInfo(deviceinfo);
			}
		}
	}
	
	MyFree(deviceinfo);
	MyFree(buf);
}
