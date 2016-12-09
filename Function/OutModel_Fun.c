/***************************************************************************************************
*FileName：OutModel_Fun
*Description：排队模块功能
*Author：xsx
*Data：2016年3月17日17:11:23
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"OutModel_Fun.h"
#include	"MyMem.h"


#include 	"task.h"
#include 	"queue.h"

#include	"stdio.h"
#include	"string.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static ModelData_DefType GB_S_ModelData = 
	{
		.OneModel_Data = 
			{
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				},
				{
					.LED_Statues = R_OFF_G_ON,
					.Time = 0
				}
			},
		.T_Count = 1
	};

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
*FunctionName：UpOneModelData
*Description：更新一个排队位置状态
*Input：modelindex -- 排队位编号
		ledstatues -- 灯状态
		time -- 闪烁间隔时间
*Output：None
*Author：xsx
*Data：2016年3月17日17:31:54
***************************************************************************************************/
void UpOneModelData(unsigned char modelindex, TM1623_LED_State ledstatues, unsigned char time)
{
	GB_S_ModelData.OneModel_Data[modelindex].LED_Statues = ledstatues;
	GB_S_ModelData.OneModel_Data[modelindex].Time = time;
}

/***************************************************************************************************
*FunctionName：GetGBModelData
*Description：获取当前排队模块状态
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月17日17:38:20
***************************************************************************************************/
ModelData_DefType *GetGBModelData(void)
{
	return &GB_S_ModelData;
}


TM1623_KEY_State GetKeyStatues(unsigned char index)
{
	return GB_S_ModelData.OneModel_Data[index].Key_Statues;
}
/***************************************************************************************************
*FunctionName：OutModel_Init
*Description：初始化排队模块
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月17日17:39:33
***************************************************************************************************/
void OutModel_Init(void)
{
	unsigned char i=0;
	for(i=0; i<9; i++)
	{
		UpOneModelData(i, R_OFF_G_ON, 0);
	}
	GB_S_ModelData.T_Count = 1;
}

/***************************************************************************************************
*FunctionName：ToggleLedStatues
*Description：跳变led状态
*Input：None
*Output：None
*Author：xsx
*Data：2016年3月17日18:03:39
***************************************************************************************************/
void ToggleLedStatues(unsigned char ledindex)
{
	if(GB_S_ModelData.OneModel_Data[ledindex].LED_Statues == GetLedStatues(ledindex))
		ChangeTM1623LedStatues(ledindex, R_OFF_G_OFF);		//灭
	else
		ChangeTM1623LedStatues(ledindex, GB_S_ModelData.OneModel_Data[ledindex].LED_Statues);		//亮
}
	
/***************************************************************************************************
*FunctionName：
*Description：
*Input：None
*Output：None
*Author：xsx
*Data：
***************************************************************************************************/
void ChangeOutModelStatues(void)
{
	static unsigned char i=0;
	
	/*读取按键状态*/
	TM1623_ReadKey();
	
	/*更改led状态*/
	for(i=0; i<8; i++)
	{
		if(GB_S_ModelData.OneModel_Data[i].Time == 0)
			ChangeTM1623LedStatues(i, GB_S_ModelData.OneModel_Data[i].LED_Statues);
		else if((GB_S_ModelData.T_Count % GB_S_ModelData.OneModel_Data[i].Time) == 0)
			ToggleLedStatues(i);
		
		GB_S_ModelData.OneModel_Data[i].Key_Statues = GetTheKeyStatues(i);
	}

	GB_S_ModelData.T_Count++;
}

