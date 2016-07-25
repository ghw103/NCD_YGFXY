#ifndef CARDL_D_H_H
#define CARDL_D_H_H

#include	"stm32f4xx.h"


#define	PreLimit_Pin			GPIO_Pin_14
#define	PreLimit_Group			GPIOE
#define	PreLimit_RCC			RCC_AHB1Periph_GPIOE

#define	BackLimit_Pin			GPIO_Pin_13
#define	BackLimit_Group			GPIOE
#define	BackLimit_RCC			RCC_AHB1Periph_GPIOE

#define	CardIn_Pin				GPIO_Pin_15
#define	CardIn_Group			GPIOE
#define	CardIn_RCC				RCC_AHB1Periph_GPIOE

#define	BackLimited 			!GPIO_ReadInputDataBit(BackLimit_Group, BackLimit_Pin)
#define	PreLimited 				!GPIO_ReadInputDataBit(PreLimit_Group, PreLimit_Pin)
#define	CardPinIn 				GPIO_ReadInputDataBit(CardIn_Group, CardIn_Pin)


void Limit_Init(void);


#endif


