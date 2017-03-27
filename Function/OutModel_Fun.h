#ifndef OUTM_F_H_H
#define OUTM_F_H_H

#include	"TM1623_Driver.h"
#include 	"FreeRTOS.h"

/*控制一个排队位的灯的闪烁状态，目前只支持一个颜色闪烁*/
typedef struct
{ 
	TM1623_LED_State LED_Statues;	//led显示的颜色
	TM1623_LED_State LEN_OffStatus;	//led灭的颜色
	TM1623_KEY_State Key_Statues;	//按键状态
	unsigned char Time;				//led闪烁间隔时间. 0表示常亮
}OneModelData_DefType;

typedef struct
{ 
	OneModelData_DefType OneModel_Data[9];		//共9个排队位置
	unsigned int T_Count;						//计时
}ModelData_DefType;

typedef struct
{ 
	TM1623_KEY_State keystatues;				//状态改变为
	unsigned char index;						//改变状态的按键索引
}KeyChange_Def;

void UpOneModelData(unsigned char modelindex, TM1623_LED_State ledstatues, TM1623_LED_State ledOffStatues, unsigned char time);
ModelData_DefType *GetGBModelData(void);
void OutModel_Init(void);
void TimeCountIncreme(void);
void ToggleLedStatues(unsigned char ledindex);

void ChangeOutModelStatues(void);

TM1623_KEY_State GetKeyStatues(unsigned char index);
#endif


