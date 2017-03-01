/***************************************************************************************************
*FileName: AppFileDao
*Description: 保存新软件的bin文件
*Author: xsx_kair
*Data: 2017年2月10日16:22:59
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"AppFileDao.h"

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

/***************************************************************************************************
*FunctionName: WriteTestData
*Description: 写测试数据到文件
*Input: testdata -- 测试数据地址
*		writeIndex -- 写入索引
*Output: None
*Return: 	My_Pass -- 保存成功
*			My_Fail -- 保存失败
*Author: xsx
*Date: 2016年12月8日10:55:53
***************************************************************************************************/
MyState_TypeDef WriteAppFile(char * file, unsigned short len, bool isNew)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile && file)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		
		if(isNew == true)
			myfile->res = f_open(&(myfile->file), "0:/UPDATE.BIN", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
		else
			myfile->res = f_open(&(myfile->file), "0:/UPDATE.BIN", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{	
			myfile->size = f_size(&(myfile->file));
			
			f_lseek(&(myfile->file), myfile->size);
			
			myfile->res = f_write(&(myfile->file), file, len, &(myfile->bw));
			if(myfile->res == FR_OK)
				statues = My_Pass;
			
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************
*FunctionName: ReadAppFile
*Description: 读取应用程序数据
*Input: startAddr -- 偏移地址， dataBuf -- 缓存, size -- 读取长度, *br -- 实际读取长度
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月16日14:55:26
***************************************************************************************************/
MyState_TypeDef ReadAppFile(unsigned int startAddr, unsigned char * dataBuf, unsigned short size, unsigned short *br,
	unsigned int *fileSize)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	*br = 0;
	
	if(myfile && dataBuf)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));

		myfile->res = f_open(&(myfile->file), "0:/UPDATE.BIN", FA_OPEN_EXISTING | FA_READ);

		if(FR_OK == myfile->res)
		{
			if(fileSize != NULL)
				*fileSize = f_size(&(myfile->file));
			
			f_lseek(&(myfile->file), startAddr);
			
			myfile->res = f_read(&(myfile->file), dataBuf, size, &(myfile->br));
			if(myfile->res == FR_OK)
			{
				statues = My_Pass;
				*br = myfile->br;
			}
			
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************
*FunctionName: checkNewAppFileIsExist
*Description: 检查是否存在新程序
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月16日15:12:19
***************************************************************************************************/
MyState_TypeDef checkNewAppFileIsExist(void)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		
		myfile->res = f_open(&(myfile->file), "0:/UPDATE.BIN", FA_OPEN_EXISTING | FA_READ);

		if(FR_OK == myfile->res)
		{	
			statues = My_Pass;
			
			f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

/***************************************************************************************************
*FunctionName: deleteAppFileIfExist
*Description: 如果存在，则删除新程序文件
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年2月16日15:13:20
***************************************************************************************************/
MyState_TypeDef deleteAppFileIfExist(void)
{
	FRESULT res;
	
	res = f_unlink("0:/UPDATE.BIN");
	
	if((FR_OK == res) || (FR_NO_FILE == res))
		return My_Pass;
	else
		return My_Fail;
}


/****************************************end of file************************************************/
