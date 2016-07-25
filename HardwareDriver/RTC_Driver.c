 /***************************************************************************************************
*FileName：RTC_Driver
*Description：内部RTC
*Author：xsx
*Data：2016年3月15日15:39:48
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"RTC_Driver.h"
#include	"Delay.h"
#include	"MyMem.h"

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
*FunctionName：RTC_Init
*Description：RTC初始化
*Input：None
*Output：0 -- 失败
*		1 -- 成功
*Author：xsx
*Data：2016年3月15日15:46:16
***************************************************************************************************/
MyState_TypeDef My_RTC_Init(void)
{
	unsigned char i=0;
	
	RTC_InitTypeDef RTC_InitStructure;
	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset RTC Domain */
//	RCC_BackupResetCmd(ENABLE);
//	RCC_BackupResetCmd(DISABLE);

	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)
	{
		/* Enable the LSE OSC */
		RCC_LSEConfig(RCC_LSE_ON);

		/* Wait till LSE is ready */  
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
			i++;
			delay_ms(200);
			
			/*返回失败*/
			if(i > 20)
				return My_Fail;
		}

		/* Select the RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟 
		/* Configure the RTC data register and RTC prescaler */
		/* ck_spre(1Hz) = RTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
		RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
		RTC_Init(&RTC_InitStructure);
		
		SetTime(0, 1, 1, 1, 12, 12, 12);
		
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
	}
	
	return My_Pass;
}
/***************************************************************************************************
*FunctionName：SetTime
*Description：设置时间
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月15日16:45:11
***************************************************************************************************/
MyState_TypeDef SetTime(unsigned char year, unsigned char month, unsigned char day, unsigned char week, unsigned char hour,
	unsigned char minute, unsigned char second)
{
	MyTime_Def * temptime = MyMalloc(sizeof(MyTime_Def));
	
	if(temptime == NULL)
		return My_Fail;
	
	temptime->date.RTC_Year = year;
	temptime->date.RTC_Month = month;
	temptime->date.RTC_Date = day;
	temptime->date.RTC_WeekDay = week;
	temptime->time.RTC_Hours = hour;
	temptime->time.RTC_Minutes = minute;
	temptime->time.RTC_Seconds = second;
	
	if((SUCCESS == RTC_SetDate(RTC_Format_BIN,&temptime->date))&&(SUCCESS == RTC_SetTime(RTC_Format_BIN,&temptime->time)))
	{
		MyFree(temptime);
		return My_Pass;
	}
	else
	{
		MyFree(temptime);
		return My_Fail;
	}
}


