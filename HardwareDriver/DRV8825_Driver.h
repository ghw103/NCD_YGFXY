#ifndef DRV_D_H_H
#define DRV_D_H_H

#include	"stm32f4xx.h"
#include	"Motor_Data.h"


#define	DRV_Power_Pin			GPIO_Pin_5
#define	DRV_Power_Group			GPIOG
#define	DRV_Power_RCC			RCC_AHB1Periph_GPIOG

#define	DRV_Sleep_Pin			GPIO_Pin_2
#define	DRV_Sleep_Group			GPIOG
#define	DRV_Sleep_RCC			RCC_AHB1Periph_GPIOG

#define	DRV_Dir_Pin				GPIO_Pin_3
#define	DRV_Dir_Group			GPIOG
#define	DRV_Dir_RCC				RCC_AHB1Periph_GPIOG



void DRV_Init(void);
void SetDRVSleepStatues(DRVSleep_Type statues);
void SetDRVPowerStatues(DRVPower_Type statues);
void SetDRVDir(DRVDir_Type value);
void motorDisable(void);

#endif


