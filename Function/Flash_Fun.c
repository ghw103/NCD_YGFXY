/***************************************************************************************************
*FileName:Flash_Fun
*Description:芯片flash操作
*Author:xsx
*Data:2017年2月17日09:57:36
***************************************************************************************************/


/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"Flash_Fun.h"
#include 	"stm32f4xx.h"
#include	"stm32f4xx_flash.h"

#include	"MyMem.h"

#include	"Define.h"

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
*FunctionName: getFlashSector
*Description: 根据地址获取地址所在flash 块号
*Input: startAddress -- 地址
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月17日10:14:39
***************************************************************************************************/
unsigned short getFlashSector(unsigned int startAddress)
{
  if((startAddress < ADDR_FLASH_SECTOR_1) && (startAddress >= ADDR_FLASH_SECTOR_0))
  {
    return FLASH_Sector_0;  
  }
  else if((startAddress < ADDR_FLASH_SECTOR_2) && (startAddress >= ADDR_FLASH_SECTOR_1))
  {
    return FLASH_Sector_1;  
  }
  else if((startAddress < ADDR_FLASH_SECTOR_3) && (startAddress >= ADDR_FLASH_SECTOR_2))
  {
    return FLASH_Sector_2;  
  }
  else if((startAddress < ADDR_FLASH_SECTOR_4) && (startAddress >= ADDR_FLASH_SECTOR_3))
  {
    return FLASH_Sector_3;  
  }
  else if((startAddress < ADDR_FLASH_SECTOR_5) && (startAddress >= ADDR_FLASH_SECTOR_4))
  {
    return FLASH_Sector_4;  
  }
  else if((startAddress < ADDR_FLASH_SECTOR_6) && (startAddress >= ADDR_FLASH_SECTOR_5))
  {
    return FLASH_Sector_5;  
  }
  else if((startAddress < ADDR_FLASH_SECTOR_7) && (startAddress >= ADDR_FLASH_SECTOR_6))
  {
    return FLASH_Sector_6;  
  }
  else if((startAddress < ADDR_FLASH_SECTOR_8) && (startAddress >= ADDR_FLASH_SECTOR_7))
  {
    return FLASH_Sector_7;  
  }
  else if((startAddress < ADDR_FLASH_SECTOR_9) && (startAddress >= ADDR_FLASH_SECTOR_8))
  {
    return FLASH_Sector_8;  
  }
  else if((startAddress < ADDR_FLASH_SECTOR_10) && (startAddress >= ADDR_FLASH_SECTOR_9))
  {
    return FLASH_Sector_9;  
  }
  else if((startAddress < ADDR_FLASH_SECTOR_11) && (startAddress >= ADDR_FLASH_SECTOR_10))
  {
    return FLASH_Sector_10;  
  }
  else
  {
    return FLASH_Sector_11;  
  }
}

/***************************************************************************************************
*FunctionName: EraseFlash
*Description: 擦除用户的flash区域
*Input: startSectorIndex -- 起始擦除块编号
*		eraseNum -- 擦除块数目
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月17日10:00:06
***************************************************************************************************/
MyState_TypeDef EraseFlashSectors(unsigned int startSectorIndex, unsigned int endSectorIndex)
{
	unsigned int index = 0U;
	FLASH_Status status = FLASH_COMPLETE;
	
	FLASH_Unlock();
	
	for(index = startSectorIndex; index <= endSectorIndex; )
	{
		status = FLASH_EraseSector(index, VoltageRange_3);
		if(FLASH_COMPLETE != status)
			break;
		
		index += 0x08;
	}
	
	FLASH_Lock();
	
	if(status == FLASH_COMPLETE)
		return My_Pass;
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName: EraseFlashSector
*Description: 擦除一个块
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月24日15:28:46
***************************************************************************************************/
MyState_TypeDef EraseFlashSector(unsigned int sectorIndex)
{

	FLASH_Status status = FLASH_COMPLETE;
	
	FLASH_Unlock();
	
	status = FLASH_EraseSector(sectorIndex, VoltageRange_3);

	FLASH_Lock();
	
	if(status == FLASH_COMPLETE)
		return My_Pass;
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName: writeFlash
*Description: 写flash
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月17日11:02:43
***************************************************************************************************/
MyState_TypeDef writeFlash(unsigned int FlashAddress, void * Data , unsigned int DataLength)
{
	uint32_t i = 0;
	FLASH_Status status = FLASH_COMPLETE;
	unsigned int * p = (unsigned int *)Data;
	
	FLASH_Unlock();
	
	for (i = 0; (i < DataLength) && (FlashAddress <= (APPLICATION_END_ADDRESS-4)); i++)
	{
		status = FLASH_ProgramWord(FlashAddress, *p);
		if(status != FLASH_COMPLETE)
		{
			break;
		}
		
		FlashAddress += 4;
		
		p++;
	}
	
	FLASH_Lock();
	
	if(status == FLASH_COMPLETE)
		return My_Pass;
	else
		return My_Fail;
}

/***************************************************************************************************
*FunctionName: writeFlash
*Description: 写flash
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月17日11:02:43
***************************************************************************************************/
void readFlash(unsigned int FlashAddress, void * Data , unsigned int DataLength)
{
	uint32_t i = 0;
	unsigned int * p = (unsigned int *)Data;
	
	for(i=0; i<DataLength; i++)
	{
		p[i] = *(unsigned int *)FlashAddress;
		FlashAddress += 4;
	}
}

void testFlashWriteAndReadFunction(void)
{
	unsigned char * dataBuf = NULL;
	unsigned int j = 0;
	unsigned int flashWriteAddr = APPLICATION_ADDRESS;
	
	dataBuf = MyMalloc(1024);
	
	if(dataBuf)
	{
		for(j=0; j<1024; j++)
			dataBuf[j] = j;
		
		//擦除用户区域
		EraseFlashSectors(getFlashSector(APPLICATION_ADDRESS), FLASH_Sector_11);
		
//		for(j=4; j<12; j++)
//		{
			if(My_Pass == writeFlash(FLASH_Sector_4, dataBuf, 256))
			{

			}
			
			if(My_Pass == writeFlash(FLASH_Sector_4+1024, dataBuf, 256))
			{
	
			}
//		}
		
		//擦除用户区域
		EraseFlashSectors(getFlashSector(APPLICATION_ADDRESS), FLASH_Sector_11);
	}
	
	MyFree(dataBuf);
}