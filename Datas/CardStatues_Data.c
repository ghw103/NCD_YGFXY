/***************************************************************************************************
*FileName:CardStatues_Data
*Description:��⿨λ��״̬
*Author:xsx
*Data:2016��5��11��20:22:59
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/
#include	"CardStatues_Data.h"


#include 	"task.h"
#include 	"queue.h"
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/
static CardState_Def  GB_CardState = NoCard;									//�޿�
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/



CardState_Def GetCardState(void)
{
	return GB_CardState;
}

void SetCardState(CardState_Def value)
{
	GB_CardState = value;
}