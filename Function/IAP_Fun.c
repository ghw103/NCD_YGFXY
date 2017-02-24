/***************************************************************************************************
*FileName:IAP_Fun
*Description:IAP功能
*Author:xsx
*Data:2017年2月16日16:27:25
***************************************************************************************************/


/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"IAP_Fun.h"
#include 	"stm32f4xx.h"
#include	"Flash_Fun.h"
#include	"IAPMessage_Fun.h"

#include	"LCD_Driver.h"

#include	"RemoteSoftDao.h"
#include	"AppFileDao.h"

#include	"MyMem.h"
#include	"Md5.h"
#include	"Define.h"
#include	"MyTools.h"
#include	"Delay.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
pFunction JumpToApplication;
uint32_t JumpAddress;
const unsigned int FLASH_SECTORS[12]=
{
	FLASH_Sector_0,
	FLASH_Sector_1,
	FLASH_Sector_2, 
	FLASH_Sector_3,  
	FLASH_Sector_4, 
	FLASH_Sector_5,
	FLASH_Sector_6,
	FLASH_Sector_7, 
	FLASH_Sector_8,  
	FLASH_Sector_9, 
	FLASH_Sector_10,
	FLASH_Sector_11
};
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
*FunctionName: jumpToUserApplicationProgram
*Description: 跳转到应用程序
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月16日16:38:47
***************************************************************************************************/
void jumpToUserApplicationProgram(void)
{
	if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    {
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
      JumpToApplication = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
      JumpToApplication();
    }
}

/***************************************************************************************************
*FunctionName: checkMd5
*Description: 计算并对比MD5
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月20日14:02:35
***************************************************************************************************/
MyState_TypeDef checkMd5(RemoteSoftInfo * remoteSoftInfo)
{
	
	char currentMd5[40];						//计算的md5

	memset(currentMd5, 0, 40);
	md5sum(currentMd5);
	
	if(true == CheckStrIsSame(remoteSoftInfo->md5, currentMd5, 32))
		return My_Pass;
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName: writeApplicationToFlash
*Description: 更新固件到flash
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月16日17:06:02
***************************************************************************************************/
void writeApplicationToFlash(void)
{
	unsigned int fileSize = 0;
	unsigned int flashWriteAddr = APPLICATION_ADDRESS;
	unsigned short i = 0;
	unsigned short readSize = 0;
	unsigned char * dataBuf = NULL;
	unsigned int j = 0;
	char statusBuf[50];
	unsigned int hasWriteSize = 0;
	double tempValue = 0.0;
	
	dataBuf = MyMalloc(40*1024);
	
	if(dataBuf)
	{
		//擦除用户区域
		showIapStatus("Erase old programs !");
		EraseFlashSectors(FLASH_SECTORS[4], FLASH_SECTORS[11]);

		
		for(i=0; i<100; i++)
		{
			memset(dataBuf, 0xff, 40*1024);
			
			readSize = 1;							//标记有数据
			if(My_Pass == ReadAppFile(flashWriteAddr - APPLICATION_ADDRESS, dataBuf, 40*1024, &readSize, &fileSize))
			{
				if(readSize != 0)
				{
					if(My_Pass == writeFlash(flashWriteAddr, dataBuf, readSize / 4))
					{
						flashWriteAddr += readSize/4*4;
						
						hasWriteSize = (flashWriteAddr - APPLICATION_ADDRESS);
						tempValue = hasWriteSize;
						tempValue /= fileSize;
						tempValue *= 100;
						
						memset(statusBuf, 0, 50);
						sprintf(statusBuf, "Update Firmware ---- %.2f%%", tempValue);
						showIapStatus(statusBuf);
						
						showIapProgess(tempValue);
						
						delay_ms(500);
					}
					else
						break;
				}
				else
					break;
			}
			else
				break;
		}
		
		MyFree(dataBuf);
		
		if(readSize == 0)
		{
			showIapStatus("Update success !");
			delay_ms(500);
			
			//deleteAppFileIfExist();
			
			//jumpToUserApplicationProgram();
		}
		else
		{
			showIapStatus("Update fail, Restarting !");
			
			//deleteAppFileIfExist();
			
			NVIC_SystemReset();
		}

	}
	//升级失败
	else
	{
		showIapStatus("Update fail, Restarting !");
			
		//deleteAppFileIfExist();
			
		NVIC_SystemReset();
	}
}

void BootLoaderMainFunction(void)
{
	RemoteSoftInfo * remoteSoftInfo = NULL;		//读取的固件信息
	
	remoteSoftInfo = MyMalloc(sizeof(RemoteSoftInfo));
	if(remoteSoftInfo)
	{
		memset(remoteSoftInfo, 0, sizeof(RemoteSoftInfo));
		//显示更新程序界面
		showIapProgess(0);
		delay_ms(50);
		SetLEDLight(100);
		delay_ms(50);	
		
		//检查是否有新程序
		if(My_Pass == checkNewAppFileIsExist())
		{
			SelectPage(119);
			showIapStatus("New firmware detected !");
			delay_s(1);
			
			if(My_Pass == ReadRemoteSoftInfo(remoteSoftInfo))
			{
				showIapVersion(remoteSoftInfo->RemoteFirmwareVersion);
				showIapStatus("Check file MD5 !");
				delay_ms(500);
				
				if(My_Pass == checkMd5(remoteSoftInfo))
				{
					writeApplicationToFlash();
				}
				else
				{
					showIapStatus("MD5 check fail, Not update !");
					delay_ms(500);
				}
			}
			else
			{
				showIapStatus("file read fail !");
			}
		}
	}
	
	MyFree(remoteSoftInfo);
	
	jumpToUserApplicationProgram();
}


