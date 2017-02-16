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
#include	"System_Data.h"
#include	"Timer_Data.h"

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



/***************************************************************************************************/
/***************************************************************************************************/
/***************************************wifi密码保存************************************************/
/***************************************************************************************************/
/***************************************************************************************************/


/*********************************************************************************************/
/*********************************************************************************************/
/************************************写入老化数据*********************************************/
/*********************************************************************************************/
/*********************************************************************************************/

MyState_TypeDef SavereTestData(ReTestData *reTestData, unsigned char type)
{
	FatfsFileInfo_Def * myfile = NULL;
	char *buf;
	MyState_TypeDef statues = My_Fail;
	
	myfile = MyMalloc(sizeof(FatfsFileInfo_Def));
	buf = MyMalloc(1024);
	
	if(myfile && reTestData && buf)
	{
		memset(myfile, 0, sizeof(FatfsFileInfo_Def));
		
		myfile->res = f_open(&(myfile->file), "0:/laohua.csv", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
			
		if(FR_OK == myfile->res)
		{
			myfile->size = f_size(&(myfile->file));
			f_lseek(&(myfile->file), myfile->size);
			
			if(myfile->size == 0)
			{
				memset(buf, 0, 1024);
				sprintf(buf, "测试次数,测试时间,测试时长(秒),结果描述,[DA-AD],[DA-AD],[DA-AD],LED状态,环境温度,机壳内温度,检测卡温度,[T值-T位置],[C值-C位置],[B值-B位置],峰高比,原始结果,校准结果,当前音频起始时间,当前音频结束时间,当前音频时长,音频总时长,音频播放次数\r");
				myfile->res = f_write(&(myfile->file), buf, strlen(buf), &(myfile->bw));
				if(FR_OK != myfile->res)
					goto END;
			}
			
			//保存测试数据
			if(type == 0)
			{
				memset(buf, 0, 1024);
				sprintf(buf, "%d/%d,%d-%d-%d %d:%d:%d,%d,%s,[100-%.3f],[200-%.3f],[300-%.3f],%d,%.1f,%.1f,%.1f,[%d-%d],[%d-%d],[%d-%d],%.3f,%.3f,%.3f\r", reTestData->retestedcount, reTestData->retestcount, reTestData->itemData.testdata.TestTime.year
					, reTestData->itemData.testdata.TestTime.month, reTestData->itemData.testdata.TestTime.day, reTestData->itemData.testdata.TestTime.hour, reTestData->itemData.testdata.TestTime.min, reTestData->itemData.testdata.TestTime.sec
					, timer_Count(&(reTestData->oneretesttimer)), reTestData->result, reTestData->advalue1, reTestData->advalue2, reTestData->advalue3, reTestData->ledstatus, reTestData->itemData.testdata.TestTemp.E_Temperature, reTestData->itemData.testdata.TestTemp.I_Temperature
					, reTestData->itemData.testdata.TestTemp.O_Temperature, reTestData->itemData.testdata.testline.T_Point[0], reTestData->itemData.testdata.testline.T_Point[1], reTestData->itemData.testdata.testline.C_Point[0]
					, reTestData->itemData.testdata.testline.C_Point[1], reTestData->itemData.testdata.testline.B_Point[0], reTestData->itemData.testdata.testline.B_Point[1], reTestData->itemData.testdata.testline.BasicBili
					, reTestData->itemData.testdata.testline.BasicResult, reTestData->itemData.testdata.testline.AdjustResult);
				myfile->res = f_write(&(myfile->file), buf, strlen(buf), &(myfile->bw));
				
				if(FR_OK != myfile->res)
					goto END;
				
				statues = My_Pass;
			}
			//保存音频测试数据
			else
			{
				memset(buf, 0, 1024);
				sprintf(buf, ",,,,,,,,,,,,,,,,,%d-%d-%d %d:%d:%d,%d-%d-%d %d:%d:%d,%d,%d,%d\r", reTestData->startplayTime.year, reTestData->startplayTime.month, reTestData->startplayTime.day
					,reTestData->startplayTime.hour, reTestData->startplayTime.min, reTestData->startplayTime.sec, reTestData->endplayTime.year, reTestData->endplayTime.month
					, reTestData->endplayTime.day, reTestData->endplayTime.hour, reTestData->endplayTime.min, reTestData->endplayTime.sec, timer_Count(&(reTestData->oneplaytimer))
					, timer_Count(&(reTestData->playtimer)), reTestData->playcount);
				myfile->res = f_write(&(myfile->file), buf, strlen(buf), &(myfile->bw));
				
				if(FR_OK != myfile->res)
					goto END;
				
				statues = My_Pass;
			}
			
			END:
				f_close(&(myfile->file));
		}
	}
	
	MyFree(myfile);
	MyFree(buf);
	
	return statues;
}

