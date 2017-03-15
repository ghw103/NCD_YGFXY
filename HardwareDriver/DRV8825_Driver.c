/***************************************************************************************************
*FileName：DRV8825_Driver
*Description：电机驱动
*Author：xsx
*Data：2016年4月23日11:03:11
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"DRV8825_Driver.h"
#include	"CardLimit_Driver.h"


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
*FunctionName：DRV_Init
*Description：电机IO初始化
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月23日11:30:52
***************************************************************************************************/
void DRV_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(DRV_Power_RCC | DRV_Dir_RCC | DRV_Sleep_RCC, ENABLE); /*使能LED灯使用的GPIO时钟*/
	
	GPIO_InitStructure.GPIO_Pin = DRV_Power_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(DRV_Power_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DRV_Sleep_Pin; 
	GPIO_Init(DRV_Sleep_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DRV_Dir_Pin; 
	GPIO_Init(DRV_Dir_Group, &GPIO_InitStructure);
	
	SetDRVSleepStatues(OnLine);
	
	SetDRVPowerStatues(NonamalPower);
	
	SetDRVDir(Reverse);

}

/***************************************************************************************************
*FunctionName：SetDRVSleepStatues
*Description：设置电机在线或者离线
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月27日08:47:37
***************************************************************************************************/
void SetDRVSleepStatues(DRVSleep_Type statues)
{
	if(statues)
	{
		if(PreLimited)
			GPIO_ResetBits(DRV_Sleep_Group, DRV_Sleep_Pin);
		else
			GPIO_SetBits(DRV_Sleep_Group, DRV_Sleep_Pin);	
	}
	else
	{
		if(PreLimited)
			GPIO_SetBits(DRV_Sleep_Group, DRV_Sleep_Pin);
		else
			GPIO_ResetBits(DRV_Sleep_Group, DRV_Sleep_Pin);	
	}
}

/***************************************************************************************************
*FunctionName：SetDRVSleepStatues
*Description：设置电机在线或者离线
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月27日08:47:37
***************************************************************************************************/
void SetDRVPowerStatues(DRVPower_Type statues)
{
	if(statues)
		GPIO_ResetBits(DRV_Power_Group, DRV_Power_Pin);
	else
		GPIO_SetBits(DRV_Power_Group, DRV_Power_Pin);
}

/***************************************************************************************************
*FunctionName：SetDRVDir
*Description：设置电机方向
*Input：None
*Output：None
*Author：xsx
*Data：2016年4月27日08:52:43
***************************************************************************************************/
void SetDRVDir(DRVDir_Type value)
{
	if(value)
		GPIO_ResetBits(DRV_Dir_Group, DRV_Dir_Pin);
	else
		GPIO_SetBits(DRV_Dir_Group, DRV_Dir_Pin);
	
	SetGB_MotorDir(value);
}

/***************************************************************************************************
*FunctionName: motorDisable
*Description: 失能电机
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年3月10日10:39:34
***************************************************************************************************/
void motorDisable(void)
{
	GPIO_ResetBits(DRV_Sleep_Group, DRV_Sleep_Pin);
}



