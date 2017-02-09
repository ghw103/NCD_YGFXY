/***************************************************************************************************
*FileName:	SystemSet_Dao
*Description:	系统参数dao
*Author: xsx_kair
*Data:	2016年12月16日16:52:22
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/

#include	"SystemSet_Dao.h"

#include	"CRC16.h"
#include	"MyMem.h"

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


/***************************************************************************************************
*FunctionName: SaveSystemSetData
*Description: 保存系统设置参数
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月16日16:57:32
***************************************************************************************************/
MyState_TypeDef SaveSystemSetData(SystemSetData * systemSetData)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef status = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile && systemSetData)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/SysSet.ncd", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);
			
			systemSetData->crc = CalModbusCRC16Fun1(systemSetData, sizeof(SystemSetData)-2);
			
			myfile->res = f_write(&(myfile->file), systemSetData, sizeof(SystemSetData), &(myfile->bw));
			
			//如果写入成功，则更新内存中的设备信息
			if((FR_OK == myfile->res)&&(myfile->bw == sizeof(SystemSetData)))
			{
				status = My_Pass;
				
				//保存成功后，更新内存中的数据
				setSystemSetData(systemSetData);
			}
				
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return status;
}

/***************************************************************************************************
*FunctionName: ReadSystemSetData
*Description: 从sd卡读取系统设置参数，如果读取失败，则清空内存中的数据，所有设置恢复默认
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月16日16:57:06
***************************************************************************************************/
MyState_TypeDef ReadSystemSetData(SystemSetData * systemSetData)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile && systemSetData)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/SysSet.ncd", FA_READ);
		
		if(FR_OK == myfile->res)
		{
			f_lseek(&(myfile->file), 0);
					
			myfile->res = f_read(&(myfile->file), systemSetData, sizeof(SystemSetData), &(myfile->br));
			
			//如果读取成功，也更新内存中的设备信息数据
			if((FR_OK == myfile->res)&&(myfile->br == sizeof(SystemSetData)))
				statues = My_Pass;
			else
				memset(systemSetData, 0, sizeof(SystemSetData));

			f_close(&(myfile->file));
		}
	}
	MyFree(myfile);
	
	return statues;
}

/****************************************end of file************************************************/
