/***************************************************************************************************
*FileName: WifiDao
*Description: 保存wifi密码
*Author: xsx_kair
*Data: 2017年2月16日11:37:59
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"WifiDao.h"
#include	"MyTools.h"
#include	"CRC16.h"
#include	"MyMem.h"

#include	"ff.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
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

MyState_TypeDef SaveWifiData(WIFI_Def * wifi)
{
	FatfsFileInfo_Def * myfile = NULL;
	WIFI_Def * tempwifi = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	tempwifi = MyMalloc(sizeof(WIFI_Def));
	if(myfile && tempwifi && wifi)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		memset(tempwifi, 0, sizeof(WIFI_Def));

		myfile->res = f_open(&(myfile->file), "0:/WifiSet.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			f_lseek(&(myfile->file), 0);
			
			for(i=0; i<255; i++)
			{
				memset(tempwifi, 0, sizeof(WIFI_Def));
				myfile->res = f_read(&(myfile->file), tempwifi, sizeof(WIFI_Def), &(myfile->br));
				
				if(FR_OK == myfile->res)
				{
					if(tempwifi->crc == CalModbusCRC16Fun1(tempwifi, sizeof(WIFI_Def)-2))
					{
						if(pdPASS == CheckStrIsSame(wifi->ssid, tempwifi->ssid, MaxSSIDLen))
						{
							f_lseek(&(myfile->file), i*sizeof(WIFI_Def));
							wifi->crc = CalModbusCRC16Fun1(wifi, sizeof(WIFI_Def)-2);
							myfile->res = f_write(&(myfile->file), wifi, sizeof(WIFI_Def), &(myfile->bw));
							if((FR_OK == myfile->res)&&(myfile->bw == sizeof(WIFI_Def)))
								statues = My_Pass;
							
							break;
						}
					}
				}
				else
					break;
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

MyState_TypeDef deleteWifi(WIFI_Def * wifi)
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

MyState_TypeDef ClearWifi(void)
{
	FRESULT res;
	
	res = f_unlink("0:/WifiSet.ncd");
	
	if(FR_OK == res)
		return My_Pass;
	else
		return My_Fail;
}

/****************************************end of file************************************************/
