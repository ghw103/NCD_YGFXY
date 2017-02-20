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

#include	"AppFileDao.h"

#include	"MyMem.h"

#include	"Define.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
pFunction JumpToApplication;
uint32_t JumpAddress;
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
	unsigned int startAddr = 0;
	unsigned int flashWriteAddr = APPLICATION_ADDRESS;
	unsigned short i = 0;
	unsigned short readSize = 0;
	unsigned char * dataBuf = NULL;
	unsigned int j = 0;
	
	dataBuf = MyMalloc(40*1024);
	
	if(dataBuf)
	{
		//擦除用户区域
		EraseFlashSectors(getFlashSector(APPLICATION_ADDRESS), FLASH_Sector_11);
		
		for(i=0; i<100; i++)
		{
			memset(dataBuf, 0xff, 40*1024);
			if(My_Pass == ReadAppFile(flashWriteAddr - APPLICATION_ADDRESS, dataBuf, 40*1024, &readSize))
			{
				if(readSize != 0)
				{
					if(My_Pass == writeFlash(flashWriteAddr, dataBuf, readSize / 4))
					{
						flashWriteAddr += readSize/4*4;
					}
				}
				else
					break;
			}
		}
		
		MyFree(dataBuf);
		jumpToUserApplicationProgram();
	}
	//升级失败
	else
		jumpToUserApplicationProgram();
}


