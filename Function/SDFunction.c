/***************************************************************************************************
*FileName:SDFunction
*Description:一些SD卡的操作函数
*Author:xsx
*Data:2016年4月30日16:06:36
***************************************************************************************************/


/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"SDFunction.h"
#include	"Net_Data.h"

#include	"CRC16.h"
#include	"MyMem.h"
#include	"MyTools.h"

#include 	"FreeRTOS.h"
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





/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：CheckSDFunction
*Description：检测SD卡是否正常
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月30日16:38:48
***************************************************************************************************/
MyState_TypeDef CheckSDFunction(void)
{
	FatfsFileInfo_Def * myfile = NULL;
	unsigned char *buf = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	buf = MyMalloc(100);
	
	if(myfile && buf)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		memset(buf, 0, 100);

		myfile->res = f_open(&(myfile->file), "0:/SDCheck.ncd", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->res = f_write(&(myfile->file), "0123456789", 10, &(myfile->bw));
			if(FR_OK == myfile->res)
			{
				f_lseek(&(myfile->file), 0);
				myfile->res = f_read(&(myfile->file), buf, 10, &(myfile->br));
				if((FR_OK == myfile->res) && (0 == memcmp(buf, "0123456789", 10)))
					statues = My_Pass;
			}

			f_close(&(myfile->file));
		}
	}
	
	MyFree(buf);
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************读写设备信息************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************
*FunctionName：SaveDeviceInfo
*Description：save device info
*Input：deviceinfo -- 设备信息地址
*Output：My_Fail -- 保存失败
*		 My_Pass -- 保存成功
*Author：xsx
*Data：2016年7月8日17:03:15
***************************************************************************************************/
MyState_TypeDef SaveDeviceInfo(DeviceInfo * deviceinfo)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile && deviceinfo)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/DeviceInfo.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);
			
			deviceinfo->crc = CalModbusCRC16Fun1(deviceinfo, sizeof(DeviceInfo)-2);
			
			myfile->res = f_write(&(myfile->file), deviceinfo, sizeof(DeviceInfo), &(myfile->bw));
				
			if((FR_OK == myfile->res)&&(myfile->bw == sizeof(DeviceInfo)))
				statues = My_Pass;
			
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************
*FunctionName：ReadDeviceInfo
*Description：读取设备信息
*Input：None
*Output：None
*Author：xsx
*Data：2016年7月8日17:03:33
***************************************************************************************************/
MyState_TypeDef ReadDeviceInfo(DeviceInfo * deviceinfo)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile && deviceinfo)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/DeviceInfo.ncd", FA_READ);
		
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);
					
			myfile->res = f_read(&(myfile->file), deviceinfo, sizeof(DeviceInfo), &(myfile->br));
			if((FR_OK == myfile->res)&&(myfile->br == sizeof(DeviceInfo)))
				statues = My_Pass;

			f_close(&(myfile->file));
		}
	}
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************读写操作人**************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
MyState_TypeDef SaveUserData(User_Type * user)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/Testers.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);
			
			myfile->res = f_write(&(myfile->file), user, sizeof(User_Type)*MaxUserNum, &(myfile->bw));
				
			if((FR_OK == myfile->res)&&(myfile->bw == sizeof(User_Type)*MaxUserNum))
				statues = My_Pass;
				
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

void ReadUserData(User_Type * user)
{
	FatfsFileInfo_Def * myfile = NULL;
	unsigned char i = 0;

	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));

	if(myfile)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/Testers.ncd", FA_READ);
		
		if(FR_OK == myfile->res)
		{
			for(i=0; i<MaxUserNum; i++)
			{
				f_lseek(&(myfile->file), i*sizeof(User_Type));
					
				myfile->res = f_read(&(myfile->file), user, sizeof(User_Type), &(myfile->br));
				if((FR_OK == myfile->res)&&(myfile->br == sizeof(User_Type)))
				{
					if(user->crc == CalModbusCRC16Fun1(user, sizeof(User_Type)-2))
					{
						user++;
					}
				}
			}
			f_close(&(myfile->file));
		}
	}	
	MyFree(myfile);
}

/***************************************************************************************************/
/***************************************************************************************************/
/********************************************测试数据***********************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************
*FunctionName：SaveTestData
*Description：保存测试数据
*Input：None
*Output：None
*Author：xsx
*Data：2016年7月16日19:43:40
***************************************************************************************************/
MyState_TypeDef SaveTestData(TestData *tempdata)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/TestData.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			if(myfile->size == 0)
				myfile->res = f_write(&(myfile->file), &(myfile->size), sizeof(unsigned int), &(myfile->bw));
			
			myfile->size = f_size(&(myfile->file));
			
			f_lseek(&(myfile->file), myfile->size);
				
			tempdata->crc = CalModbusCRC16Fun1(tempdata, sizeof(TestData)-2);
			myfile->res = f_write(&(myfile->file), tempdata, sizeof(TestData), &(myfile->bw));
			if(FR_OK == myfile->res)
			{
				myfile->size = f_size(&(myfile->file));
				SaveDateInfo(tempdata, myfile->size/sizeof(TestData));
				statues = My_Pass;
			}
				
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************
*FunctionName：ReadTestData
*Description：读取测试数据
*Input：filename -- 待读取数据的文件名
*		tempdata -- 存放地址，此地址需能存放readnum个数据的空间
*		index --  读取数据的索引（0开始）
*		readnum -- 读取的个数
*Output：None
*Author：xsx
*Data：
***************************************************************************************************/
MyState_TypeDef ReadTestData(TestData *tempdata, unsigned char *realreadnum, unsigned int index, unsigned char readnum)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile)
	{
		myfile->res = f_open(&(myfile->file), "0:/TestData.ncd", FA_READ);
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			myfile->res = f_lseek(&(myfile->file), index*sizeof(TestData)+4);
			if(FR_OK == myfile->res)
			{
				for(i=0; i<readnum; i++)
				{
					myfile->res = f_read(&(myfile->file), tempdata, sizeof(TestData), &(myfile->br));
					if((FR_OK == myfile->res) && (sizeof(TestData) == myfile->br))
						tempdata++;
					else
						break;
				}
				if(i > 0)
					statues = My_Pass;
				
				*realreadnum = i;
			}
			
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************
*FunctionName：写入一个未上传数据的id
*Description：
*Input：None
*Output：None
*Author：xsx
*Data：
***************************************************************************************************/
MyState_TypeDef SaveDateInfo(TestData *tempdata, unsigned int index)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	TestDateInfo_Def *my_TestDateInfo = NULL;
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/TestDateInfo.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			my_TestDateInfo = MyMalloc(sizeof(TestDateInfo_Def));
			if(my_TestDateInfo)
			{
				myfile->res = f_read(&(myfile->file), my_TestDateInfo, sizeof(TestDateInfo_Def), &(myfile->br));
				if(FR_OK == myfile->res)
				{
					if(0 == memcmp(&(my_TestDateInfo->year), &(tempdata->TestTime.year), 3))
					{
						my_TestDateInfo->num += 1;
					}
					else
					{
						my_TestDateInfo->year = tempdata->TestTime.year;
						my_TestDateInfo->month = tempdata->TestTime.month;
						my_TestDateInfo->day = tempdata->TestTime.day;
						my_TestDateInfo->index = index;
						my_TestDateInfo->num = 1;
					}
					
					myfile->res = f_write(&(myfile->file), my_TestDateInfo, sizeof(TestDateInfo_Def), &(myfile->bw));
					if(FR_OK == myfile->res)
						statues = My_Pass;
				}
			}
			MyFree(my_TestDateInfo);
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************/
/***************************************************************************************************/
/*************************************IP设置********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
MyState_TypeDef SaveNetData(NetData * netdata)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));

	if(myfile)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/NetSet.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);
				
			netdata->crc = CalModbusCRC16Fun1(netdata, sizeof(NetData)-2);
				
			myfile->res = f_write(&(myfile->file), netdata, sizeof(NetData), &(myfile->bw));
				
			if((FR_OK == myfile->res)&&(myfile->bw == sizeof(NetData)))
				statues = My_Pass;

			myfile->res = f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

MyState_TypeDef ReadNetData(NetData * netdata)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/NetSet.ncd", FA_READ);
		
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);
					
			myfile->res = f_read(&(myfile->file), netdata, sizeof(NetData), &(myfile->br));
			if((FR_OK == myfile->res)&&(myfile->br == sizeof(NetData))&&(netdata->crc == CalModbusCRC16Fun1(netdata, sizeof(NetData)-2)))
			{
				statues = My_Pass;
			}

			myfile->res = f_close(&(myfile->file));
		}
	}
	MyFree(myfile);
	
	return statues;
}
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************wifi密码保存************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
MyState_TypeDef SaveWifiData(WIFI_Def * wifi)
{
	FatfsFileInfo_Def * myfile = NULL;
	WIFI_Def * tempwifi = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	tempwifi = MyMalloc(sizeof(WIFI_Def));
	if(myfile && tempwifi)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		memset(tempwifi, 0, sizeof(WIFI_Def));

		myfile->res = f_open(&(myfile->file), "0:/WifiSet.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));

			for(i=0; i<255; i++)
			{
				f_lseek(&(myfile->file), i*sizeof(WIFI_Def));
				myfile->res = f_read(&(myfile->file), tempwifi, sizeof(WIFI_Def), &(myfile->br));
				if((FR_OK == myfile->res)&&(myfile->br == sizeof(WIFI_Def))&&(tempwifi->crc == CalModbusCRC16Fun1(tempwifi, sizeof(WIFI_Def)-2)))
				{
					if(pdPASS == CheckStrIsSame(wifi->ssid, tempwifi->ssid, MaxSSIDLen))
					{
						break;
					}
				}
				else
					break;
			}
			if(i<255)
			{
				f_lseek(&(myfile->file), i*sizeof(WIFI_Def));
				wifi->crc = CalModbusCRC16Fun1(wifi, sizeof(WIFI_Def)-2);
				myfile->res = f_write(&(myfile->file), wifi, sizeof(WIFI_Def), &(myfile->bw));
				if((FR_OK == myfile->res)&&(myfile->bw == sizeof(WIFI_Def)))
					statues = My_Pass;
			}

			f_close(&(myfile->file));
		}
	}
	MyFree(tempwifi);
	MyFree(myfile);
	
	return statues;
}

MyState_TypeDef ReadWifiData(WIFI_Def * wifi)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i = 0;
	WIFI_Def * tempwifi = NULL;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	tempwifi = MyMalloc(sizeof(WIFI_Def));
	
	if(myfile && tempwifi)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		memset(tempwifi, 0, sizeof(WIFI_Def));
		
		myfile->res = f_open(&(myfile->file), "0:/WifiSet.ncd", FA_READ);
		
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			for(i=0; i<(myfile->size / sizeof(WIFI_Def)); i++)
			{
				f_lseek(&(myfile->file), i*sizeof(WIFI_Def));
				
				memset(tempwifi, 0, sizeof(WIFI_Def));
				myfile->res = f_read(&(myfile->file), tempwifi, sizeof(WIFI_Def), &(myfile->br));
				if((FR_OK == myfile->res)&&(myfile->br == sizeof(WIFI_Def)))
				{
					if((tempwifi->crc == CalModbusCRC16Fun1(tempwifi, sizeof(WIFI_Def)-2))&&(pdPASS == CheckStrIsSame(wifi->ssid, tempwifi->ssid, MaxSSIDLen)))
					{
						memcpy(wifi->key, tempwifi->key, MaxKEYLen);
						statues = My_Pass;
						break;
					}
				}
			}

			f_close(&(myfile->file));
		}
	}
	
	MyFree(tempwifi);
	MyFree(myfile);
	
	return statues;
}

MyState_TypeDef ClearWifiData(WIFI_Def * wifi)
{
	FatfsFileInfo_Def * myfile = NULL;
	WIFI_Def * tempwifi = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned short i=0;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	tempwifi = MyMalloc(sizeof(WIFI_Def));
	
	if(myfile && tempwifi)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		memset(tempwifi, 0, sizeof(WIFI_Def));

		myfile->res = f_open(&(myfile->file), "0:/WifiSet.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			for(i=0; i<(myfile->size / sizeof(WIFI_Def)); i++)
			{
				f_lseek(&(myfile->file), i*sizeof(WIFI_Def));
				
				memset(tempwifi, 0, sizeof(WIFI_Def));
				myfile->res = f_read(&(myfile->file), tempwifi, sizeof(WIFI_Def), &(myfile->br));
				if((FR_OK == myfile->res)&&(myfile->br == sizeof(WIFI_Def)))
				{
					if((tempwifi->crc == CalModbusCRC16Fun1(tempwifi, sizeof(WIFI_Def)-2))&&(pdPASS == CheckStrIsSame(wifi->ssid, tempwifi->ssid, MaxSSIDLen)))
					{
						tempwifi->crc = 0;
						myfile->res = f_write(&(myfile->file), tempwifi, sizeof(WIFI_Def), &(myfile->bw));
						if((FR_OK == myfile->res)&&(myfile->bw == sizeof(WIFI_Def)))
							statues = My_Pass;
						break;
					}
				}
			}

			f_close(&(myfile->file));
		}
	}
	MyFree(tempwifi);
	MyFree(myfile);
	
	return statues;
}

