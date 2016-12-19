/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"UpLoad_Fun.h"
#include	"ServerFun.h"
#include	"RTC_Driver.h"
#include	"SystemSet_Data.h"
#include	"SystemSet_Dao.h"

#include	"MyMem.h"
#include	"CRC16.h"

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	"stdio.h"
#include	"string.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static MyState_TypeDef ReadTime(void);
static MyState_TypeDef UpLoadDeviceInfo(void);
static MyState_TypeDef UpLoadTestData(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void UpLoadFunction(void)
{
	unsigned short i = 0;
	while(1)
	{
		if(My_Pass == ReadTime())
		{
			vTaskDelay(1000 / portTICK_RATE_MS);
			
			if(My_Pass == UpLoadDeviceInfo())
			{
				vTaskDelay(1000 / portTICK_RATE_MS);
				
				i=0;
				while(i <= 60)
				{
					if(My_Pass == UpLoadTestData())
						i++;
					else
						break;
					
					vTaskDelay(100 / portTICK_RATE_MS);
				}
				
				vTaskDelay((10-i)*1000 / portTICK_RATE_MS);
			}
			else
				vTaskDelay(10000 / portTICK_RATE_MS);
		}
		else
			vTaskDelay(10000 / portTICK_RATE_MS);
	}
}

static MyState_TypeDef ReadTime(void)
{
	char * buf = NULL;
	SystemSetData * systemSetData = NULL;
	MyState_TypeDef status = My_Fail;
	
	buf = MyMalloc(100);
	systemSetData = MyMalloc(sizeof(SystemSetData));
	
	if(buf && systemSetData)
	{
		memset(buf, 0, 100);
		
		getSystemSetData(systemSetData);
		
		if(systemSetData->crc == CalModbusCRC16Fun1(systemSetData, sizeof(SystemSetData) - 2))
		{
			sprintf(buf, "deviceBean.id=%s", systemSetData->deviceInfo.deviceid);
		
			if(My_Pass == UpLoadData("/NCD_YGFXY_Server/rtime.action", buf, strlen(buf)))
			{
				//RTC_SetTimeData2(buf+10);
				//SetGB_LineNetStatus(1);
				status = My_Pass;
			}
		}
	}
	MyFree(buf);
	MyFree(systemSetData);
	
	return status;
}

static MyState_TypeDef UpLoadDeviceInfo(void)
{
	MyState_TypeDef status = My_Fail;

	SystemSetData * systemSetData = NULL;
	char * buf = NULL;
	static unsigned char i=0;
	
	systemSetData = MyMalloc(sizeof(SystemSetData));
	
	if(systemSetData)
	{
		getSystemSetData(systemSetData);
		
		if(systemSetData->crc == CalModbusCRC16Fun1(systemSetData, sizeof(SystemSetData) - 2)) 
		{
			if(systemSetData->deviceInfo.isnew)
			{
				buf = MyMalloc(2048);
				
				if(buf)
				{
					memset(buf, 0, 2048);

					sprintf(buf, "deviceBean.id=%s&deviceBean.daddr=%s&deviceBean.name=%s&deviceBean.age=%s&deviceBean.sex=%s&deviceBean.phone=%s&deviceBean.job=%s&deviceBean.dsc=%s&deviceBean.disok=true",
						systemSetData->deviceInfo.deviceid,  systemSetData->deviceInfo.deviceunit, systemSetData->deviceInfo.deviceuser.user_name, systemSetData->deviceInfo.deviceuser.user_age+i, 
						systemSetData->deviceInfo.deviceuser.user_sex,	systemSetData->deviceInfo.deviceuser.user_phone, systemSetData->deviceInfo.deviceuser.user_job, systemSetData->deviceInfo.deviceuser.user_desc);
					
					if(My_Pass == UpLoadData("/NCD_YGFXY_Server/d_info.action", buf, strlen(buf)))
					{
						i++;
						systemSetData->deviceInfo.isnew = false ;
						
						if(My_Pass == SaveSystemSetData(systemSetData))
						{
							setSystemSetData(systemSetData);
							status = My_Pass;
						}
					}
					MyFree(buf);
				}
			}
			else
				status = My_Pass;
		}
		
		MyFree(systemSetData);
		
		return status;
	}
	else
		return My_Fail;
}

static MyState_TypeDef UpLoadTestData(void)
{
/*	MyState_TypeDef statues = My_Fail;
	unsigned short i=0, j;
	
	UpLoadTestDataBuffer * myUpLoadTestDataBuffer = NULL;
	
	myUpLoadTestDataBuffer = MyMalloc(sizeof(UpLoadTestDataBuffer));
	
	if(myUpLoadTestDataBuffer)
	{
		memset(myUpLoadTestDataBuffer, 0, sizeof(UpLoadTestDataBuffer));
		
		//读取设备信息
		ReadDeviceInfo(&(myUpLoadTestDataBuffer->deviceinfo));
		
		//读取测试数据头,失败则退出
		if(My_Pass != ReadTestDataHead(&(myUpLoadTestDataBuffer->myTestDataSaveHead)))
			goto END;
		
		//检测数据头是否校验正确，且有数据待发送
		if((myUpLoadTestDataBuffer->myTestDataSaveHead.crc != CalModbusCRC16Fun1(&(myUpLoadTestDataBuffer->myTestDataSaveHead), sizeof(TestDataSaveHead)-2)) ||
				myUpLoadTestDataBuffer->myTestDataSaveHead.readindex >= myUpLoadTestDataBuffer->myTestDataSaveHead.datanum)
			goto END;
		
		//读取测试数据,读取失败则退出
		if(My_Pass != ReadTestData(&(myUpLoadTestDataBuffer->testdata), myUpLoadTestDataBuffer->myTestDataSaveHead.readindex, 1))
			goto END;
		
		//校验读取的数据的正确性,如果不正确，则上传索引+1，略过此数据
		if(myUpLoadTestDataBuffer->testdata.crc != CalModbusCRC16Fun1(&(myUpLoadTestDataBuffer->testdata), sizeof(TestData)-2))
		{
			statues = My_Pass;
			ReadIndexPlus(1);
			goto END;
		}
		
		//上传测试数据
		memset(myUpLoadTestDataBuffer->sendbuf, 0, 2048);

		sprintf(myUpLoadTestDataBuffer->sendbuf, "tdata.cid=%s&tdata.citem=%s&tdata.cdw=%s&tdata.did=%s&tdata.t_name=%s&tdata.sampleid=%s&tdata.testtime=2016-11-24 16:42:12&tdata.e_t=%.1f&tdata.o_t=%.1f&tdata.outt=%d&tdata.c_l=%d&tdata.t_l=%d&tdata.b_l=%d&tdata.t_c_v=%.3f&tdata.a_p=%.3f&tdata.b_v=%.3f&tdata.a_v=%.3f",
			myUpLoadTestDataBuffer->testdata.temperweima.CardPiCi, myUpLoadTestDataBuffer->testdata.temperweima.ItemName, myUpLoadTestDataBuffer->testdata.temperweima.ItemMeasure, myUpLoadTestDataBuffer->deviceinfo.deviceid, myUpLoadTestDataBuffer->testdata.user.user_name, myUpLoadTestDataBuffer->testdata.sampleid,
			myUpLoadTestDataBuffer->testdata.TestTime.year, myUpLoadTestDataBuffer->testdata.TestTime.month, myUpLoadTestDataBuffer->testdata.TestTime.day,
			myUpLoadTestDataBuffer->testdata.TestTime.hour, myUpLoadTestDataBuffer->testdata.TestTime.min, myUpLoadTestDataBuffer->testdata.TestTime.sec,
			myUpLoadTestDataBuffer->testdata.TestTemp.E_Temperature, myUpLoadTestDataBuffer->testdata.TestTemp.O_Temperature,
			myUpLoadTestDataBuffer->testdata.time, myUpLoadTestDataBuffer->testdata.testline.C_Point[1], myUpLoadTestDataBuffer->testdata.testline.T_Point[1], myUpLoadTestDataBuffer->testdata.testline.B_Point[1], myUpLoadTestDataBuffer->testdata.testline.BasicBili,
			myUpLoadTestDataBuffer->testdata.tempadjust.parm, myUpLoadTestDataBuffer->testdata.testline.BasicResult, myUpLoadTestDataBuffer->testdata.testline.AdjustResult);

		if(My_Pass != UpLoadData("/NCD_YGFXY_Server/uptestdata.action", myUpLoadTestDataBuffer->sendbuf, strlen(myUpLoadTestDataBuffer->sendbuf)))
			goto END;
		
		//上传检测卡数据
		memset(myUpLoadTestDataBuffer->sendbuf, 0, 2048);

		sprintf(myUpLoadTestDataBuffer->sendbuf, "cardBean.id=%s&cardBean.item=%s&cardBean.n_v=%.3f&cardBean.l_v=%.3f&cardBean.h_v=%.3f&cardBean.dw=%s&cardBean.tl=%d&cardBean.bq_n=%d&cardBean.fend=%.3f&cardBean.bq1_a=%.3f&cardBean.bq1_b=%.3f&cardBean.bq1_c=%.3f&cardBean.bq2_a=%.3f&cardBean.bq2_b=%.3f&cardBean.bq2_c=%.3f&cardBean.waitt=%d&cardBean.cl=%d&cardBean.outt=20%02d-%02d-%02d",
			myUpLoadTestDataBuffer->testdata.temperweima.CardPiCi, myUpLoadTestDataBuffer->testdata.temperweima.ItemName, myUpLoadTestDataBuffer->testdata.temperweima.NormalResult, myUpLoadTestDataBuffer->testdata.temperweima.LowstResult, myUpLoadTestDataBuffer->testdata.temperweima.HighestResult,
			myUpLoadTestDataBuffer->testdata.temperweima.ItemMeasure, myUpLoadTestDataBuffer->testdata.temperweima.ItemLocation, myUpLoadTestDataBuffer->testdata.temperweima.ItemBiaoQuNum, myUpLoadTestDataBuffer->testdata.temperweima.ItemFenDuan, myUpLoadTestDataBuffer->testdata.temperweima.ItemBiaoQu[0][0],
			myUpLoadTestDataBuffer->testdata.temperweima.ItemBiaoQu[0][1], myUpLoadTestDataBuffer->testdata.temperweima.ItemBiaoQu[0][2],myUpLoadTestDataBuffer->testdata.temperweima.ItemBiaoQu[1][0],	myUpLoadTestDataBuffer->testdata.temperweima.ItemBiaoQu[1][1], myUpLoadTestDataBuffer->testdata.temperweima.ItemBiaoQu[1][2],
			myUpLoadTestDataBuffer->testdata.temperweima.CardWaitTime, myUpLoadTestDataBuffer->testdata.temperweima.CLineLocation, myUpLoadTestDataBuffer->testdata.temperweima.CardBaoZhiQi.year, myUpLoadTestDataBuffer->testdata.temperweima.CardBaoZhiQi.month, myUpLoadTestDataBuffer->testdata.temperweima.CardBaoZhiQi.day);

		if(My_Pass != UpLoadData("/NCD_YGFXY_Server/upcard.action", myUpLoadTestDataBuffer->sendbuf, strlen(myUpLoadTestDataBuffer->sendbuf)))
			goto END;
		
		//上传测试曲线
		for(i=0; i<4; i++)
		{
			memset(myUpLoadTestDataBuffer->sendbuf, 0, 2048);
			sprintf(myUpLoadTestDataBuffer->sendbuf, "tdata.cid=%s&sindex=%d&series=[",
				myUpLoadTestDataBuffer->testdata.temperweima.CardPiCi, i+1);
			
			for(j=0; j<110; j++)
			{
				if((i == 3) && (j >= 100))
					break;
				
				if(j == 0)
					sprintf(myUpLoadTestDataBuffer->tempbuf, "%d", myUpLoadTestDataBuffer->testdata.testline.TestPoint[i*110+j]);
				else
					sprintf(myUpLoadTestDataBuffer->tempbuf, ",%d", myUpLoadTestDataBuffer->testdata.testline.TestPoint[i*110+j]);
				strcat(myUpLoadTestDataBuffer->sendbuf, myUpLoadTestDataBuffer->tempbuf);
			}
			
			sprintf(myUpLoadTestDataBuffer->tempbuf, "]");
			strcat(myUpLoadTestDataBuffer->sendbuf, myUpLoadTestDataBuffer->tempbuf);
			

			if(My_Pass != UpLoadData("/NCD_YGFXY_Server/upseriesdata.action", myUpLoadTestDataBuffer->sendbuf, strlen(myUpLoadTestDataBuffer->sendbuf)))
				goto END;
		}*/
		
/*		//上传测试人
		memset(myUpLoadTestDataBuffer->sendbuf, 0, 1024);

		sprintf(myUpLoadTestDataBuffer->sendbuf, "tdata.cid=%s&tdata.t_name=%s&tdata.t_age=%s&tdata.t_sex=%s&tdata.t_phone=%s&tdata.t_job=%s&tdata.t_desc=%s",
			myUpLoadTestDataBuffer->testdata.temperweima.CardPiCi, myUpLoadTestDataBuffer->testdata.user.user_name, myUpLoadTestDataBuffer->testdata.user.user_age, 
			myUpLoadTestDataBuffer->testdata.user.user_sex, myUpLoadTestDataBuffer->testdata.user.user_phone, myUpLoadTestDataBuffer->testdata.user.user_job,
			myUpLoadTestDataBuffer->testdata.user.user_desc);

		if(My_Pass != UpLoadData("/NCD_YGFXY_Server/uptester.action", myUpLoadTestDataBuffer->sendbuf, strlen(myUpLoadTestDataBuffer->sendbuf)))
			goto END;*/
		
/*		ReadIndexPlus(1);
		statues = My_Pass;
	}
	
	END:
		MyFree(myUpLoadTestDataBuffer);

	return statues;*/
}
