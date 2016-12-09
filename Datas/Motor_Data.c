/***************************************************************************************************
*FileName:Motor_Data
*Description:电机数据
*Author:xsx
*Data:2016年5月11日20:22:59
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"Motor_Data.h"
#include	"Define.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static LimitState_Def GB_PreLimitState = NotLimitted;							//前限位状态
static LimitState_Def GB_BackLimitState = NotLimitted;							//后限位状态
static unsigned short GB_MotorLocation = 10000;									//电机当前位置
static unsigned short GB_MotorTargetLocation = 0;								//目标位置
static DRVDir_Type GB_MotorDir = Reverse;										//电机方向

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

LimitState_Def GetMotorPreLimitState(void)
{
	return GB_PreLimitState;
}

void SetMotorPreLimitState(LimitState_Def value)
{
	GB_PreLimitState = value;
}

LimitState_Def GetMotorBackLimitState(void)
{
	return GB_BackLimitState;
}

void SetMotorBackLimitState(LimitState_Def value)
{
	GB_BackLimitState = value;
}

unsigned short GetGB_MotorLocation(void)
{
	return GB_MotorLocation;
}

void SetGB_MotorLocation(unsigned short value)
{
	GB_MotorLocation = value;
}

unsigned short GetGB_MotorTargetLocation(void)
{
	return GB_MotorTargetLocation;
}

void SetGB_MotorTargetLocation(unsigned short value)
{
	GB_MotorTargetLocation = value;
}


unsigned short GetGB_MotorDir(void)
{
	return GB_MotorDir;
}

void SetGB_MotorDir(DRVDir_Type dir)
{
	GB_MotorDir = dir;
}



