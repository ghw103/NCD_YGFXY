/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List**********************************************/
/***************************************************************************************************/
#include	"System_Data.h"
#include	"MyTest_Data.h"
#include	"SystemSet_Data.h"

#include	"CRC16.h"

#include	<string.h>
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

static ItemData	S_ItemData;										//测试数据,用于纽康度生物实验室使用

static unsigned char S_TestStatus = 0;							//测试状态

static MyTime_Def GB_Time;											//系统时间

static float GB_EnTemperature = 0;
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/


/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
void * GetTestDataForLab(void)
{
	//从系统设置数据中获取测试时led的亮度值
	S_ItemData.ledLight = getTestLedLightIntensity(getGBSystemSetData());
	
	return &S_ItemData;
}

void SetTestStatusFlorLab(unsigned char status)
{
	S_TestStatus = status;
}

unsigned char GetTestStatusFlorLab(void)
{
	return S_TestStatus;
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/

void GetGB_Time(void * time)
{
	memcpy(time, &GB_Time, sizeof(MyTime_Def));
}

void SetGB_Time(void * time)
{	
	memcpy(&GB_Time, time, sizeof(MyTime_Def));
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/

float GetGB_EnTemperature(void)
{
	return GB_EnTemperature;
}

void SetGB_EnTemperature(float temp)
{	
	GB_EnTemperature = temp;
}

/****************************************end of file************************************************/
