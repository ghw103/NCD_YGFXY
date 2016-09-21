#ifndef RTC_D_H
#define RTC_D_H

#include	"Define.h"

#define	RX_SCK_Pin			GPIO_Pin_6
#define	RX_SCK_Group		GPIOB
#define	RX_SCK_Rcc			RCC_AHB1Periph_GPIOB

#define	RX_SDA_Pin			GPIO_Pin_8
#define	RX_SDA_Group		GPIOB
#define	RX_SDA_Rcc			RCC_AHB1Periph_GPIOB

#define RX_SDA_H()	    	GPIO_SetBits(RX_SDA_Group, RX_SDA_Pin)
#define RX_SDA_L()	    	GPIO_ResetBits(RX_SDA_Group, RX_SDA_Pin)
#define RX_SDA_PIN()	    GPIO_ReadInputDataBit(RX_SDA_Group, RX_SDA_Pin) //读取引脚电平

#define	RX_SCK_H()			GPIO_SetBits(RX_SCK_Group, RX_SCK_Pin)
#define	RX_SCK_L()			GPIO_ResetBits(RX_SCK_Group, RX_SCK_Pin)

#define	RX_DelayTime		5

void RTC_BSPInit(void);
MyState_TypeDef RTC_SetTimeData(MyTime_Def * data);
MyState_TypeDef RTC_SetTimeData2(char * buf);
void RTC_GetTimeData(MyTime_Def * time);
#endif

