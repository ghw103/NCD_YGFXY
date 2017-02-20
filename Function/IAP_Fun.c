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

#include	"Md5FileDao.h"
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
MyState_TypeDef checkMd5(void)
{
	char originMd5[40];				//原始MD5
	char currentMd5[40];				//当前MD5
	
	//读取文件中的md5
	memset(originMd5, 0, 40);
	ReadMd5File(originMd5);
	
	//计算更新固件的md5值
	memset(currentMd5, 0, 40);
	md5sum(currentMd5);
	
	//对比MD5
	if(true == CheckStrIsSame(originMd5, currentMd5, 32))
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
		showIapStatus("Erase old programs !\0");
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
						sprintf(statusBuf, "Update Firmware ---- %.2f%%\0", tempValue);
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
			memset(statusBuf, 0, 50);
			sprintf(statusBuf, "Update success !\0");
			showIapStatus(statusBuf);
			delay_ms(500);
			
			deleteAppFileIfExist();
			
			jumpToUserApplicationProgram();
		}
		else
		{
			memset(statusBuf, 0, 50);
			sprintf(statusBuf, "Update fail, Restarting !\0");
			showIapStatus(statusBuf);
			
			NVIC_SystemReset();
		}

	}
	//升级失败
	else
	{
		memset(statusBuf, 0, 50);
		sprintf(statusBuf, "Update fail, Restarting !\0");
		showIapStatus(statusBuf);
			
		NVIC_SystemReset();
	}
}

void BootLoaderMainFunction(void)
{
	//显示更新程序界面
	SelectPage(0);
	showIapProgess(0);
	
	//检查是否有新程序
	if(My_Pass == checkNewAppFileIsExist())
	{
		
		showIapStatus("New firmware detected !\0");
		delay_s(1);
		
		showIapStatus("Check file MD5 !\0");
		delay_ms(500);
		
		//如果有新固件，对比MD5
		if(My_Pass == checkMd5())
		{
			writeApplicationToFlash();
		}
		else
		{
			showIapStatus("MD5 check fail, Not update !\0");
			delay_ms(500);
			
			jumpToUserApplicationProgram();
		}
	}
	else
		jumpToUserApplicationProgram();
}


