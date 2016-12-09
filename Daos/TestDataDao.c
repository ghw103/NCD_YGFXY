/***************************************************************************************************
*FileName: TestDataDao
*Description: 测试数据dao
*Author: xsx_kair
*Data: 2016年12月8日10:43:26
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"TestDataDao.h"

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
*Output: None
*Return: 	My_Pass -- 保存成功
*			My_Fail -- 保存失败
*Author: xsx
*Date: 2016年12月8日10:55:53
***************************************************************************************************/
MyState_TypeDef WriteTestData(TestData * testdata)
{
	FatfsFileInfo_Def * myfile = NULL;
	TestDataHead * myTestDataHead;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	myTestDataHead = MyMalloc(sizeof(TestDataHead));
	
	if(myfile && myTestDataHead && testdata)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		memset(myTestDataHead, 0, sizeof(TestDataHead));
		
		myfile->res = f_open(&(myfile->file), "0:/TD.NCD", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			//读取数据头
			f_lseek(&(myfile->file), 0);
			f_read(&(myfile->file), myTestDataHead, sizeof(TestDataHead), &(myfile->br));
			
			if(myTestDataHead->crc != CalModbusCRC16Fun1(myTestDataHead, sizeof(TestDataHead)-2))
			{
				myTestDataHead->datanum = 0;
				myTestDataHead->readindex = 0;
			}
			
			myTestDataHead->datanum++;
			
			myTestDataHead->crc = CalModbusCRC16Fun1(myTestDataHead, sizeof(TestDataHead)-2);
			
			//写入数据
			f_lseek(&(myfile->file), (myTestDataHead->datanum-1) * sizeof(TestData) + sizeof(TestDataHead));

			testdata->crc = CalModbusCRC16Fun1(testdata, sizeof(TestData)-2);
			
			myfile->res = f_write(&(myfile->file), testdata, sizeof(TestData), &(myfile->bw));
			if(myfile->res != FR_OK)
				goto END;
			
			myfile->res = f_sync(&(myfile->file));
			if(myfile->res != FR_OK)
				goto END;
			
			//更新数据头
			f_lseek(&(myfile->file), 0);
			myfile->res = f_write(&(myfile->file), myTestDataHead, sizeof(TestDataHead), &(myfile->bw));
			if(myfile->res != FR_OK)
				goto END;
			
			myfile->res = f_sync(&(myfile->file));
			if(myfile->res != FR_OK)
				goto END;
			
			if(FR_OK == myfile->res)
				statues = My_Pass;
			
			END:
				f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	MyFree(myTestDataHead);
	
	return statues;
}

/***************************************************************************************************
*FunctionName: ReadTestData
*Description: 读取测试数据
*Input: readpackage -- 保护读取信息以及读取缓冲区
*Output: None
*Return: 	My_Pass -- 读取成功
*			My_Fail -- 读取失败
*Author: xsx
*Date: 2016年12月8日11:25:18
***************************************************************************************************/
MyState_TypeDef ReadTestData(ReadTestDataPackage * readpackage)
{
	FatfsFileInfo_Def * myfile = NULL;
	MyState_TypeDef statues = My_Fail;
	unsigned char i=0;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	
	if(myfile && readpackage)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		
		readpackage->readDataNum = 0;
		
		myfile->res = f_open(&(myfile->file), "0:/TD.NCD", FA_READ);
		
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			
			//读取数据头
			f_lseek(&(myfile->file), 0);
			f_read(&(myfile->file), &(readpackage->testDataHead), sizeof(TestDataHead), &(myfile->br));
			
			//数据头校验失败，则读取失败，返回读取数目为0
			if(readpackage->testDataHead.crc != CalModbusCRC16Fun1(&(readpackage->testDataHead), sizeof(TestDataHead)-2))
				goto END;
			
			//起始读取索引大于数据总数，则失败，返回读取数目为0
			if((readpackage->startReadIndex + 1) > readpackage->testDataHead.datanum)
				goto END;
			
			if(readpackage->maxReadNum > (readpackage->testDataHead.datanum - readpackage->startReadIndex))
				readpackage->maxReadNum = (readpackage->testDataHead.datanum - readpackage->startReadIndex);
			
			myfile->res = f_lseek(&(myfile->file), (readpackage->testDataHead.datanum - (readpackage->maxReadNum + readpackage->startReadIndex))*sizeof(TestData)+sizeof(TestDataHead));
			if(FR_OK == myfile->res)
			{
				for(i=0; i<readpackage->maxReadNum; i++)
				{
					myfile->res = f_read(&(myfile->file), &(readpackage->testData[readpackage->maxReadNum - 1 - i]), sizeof(TestData), &(myfile->br));
					
					if((FR_OK == myfile->res) && 
						(readpackage->testData[readpackage->maxReadNum - 1 - i].crc == CalModbusCRC16Fun1(&(readpackage->testData[readpackage->maxReadNum - 1 - i]), sizeof(TestData)-2)))
						readpackage->readDataNum++;
					else
						break;
				}
				statues = My_Pass;
			}
			
			END:
				f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	
	return statues;
}

/****************************************end of file************************************************/
