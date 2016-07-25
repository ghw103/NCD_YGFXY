#ifndef MAX4051_D_H_H
#define MAX4051_D_H_H

#include	"stm32f4xx_gpio.h"


#define	MAX_A_Pin			GPIO_Pin_4
#define	MAX_A_GpioGroup		GPIOF
#define	MAX_A_Rcc			RCC_AHB1Periph_GPIOF

#define	MAX_B_Pin			GPIO_Pin_2
#define	MAX_B_GpioGroup		GPIOF
#define	MAX_B_Rcc			RCC_AHB1Periph_GPIOF

#define	MAX_C_Pin			GPIO_Pin_3
#define	MAX_C_GpioGroup		GPIOF
#define	MAX_C_Rcc			RCC_AHB1Periph_GPIOF

#define	MAX_A_H()		GPIO_SetBits(MAX_A_GpioGroup, MAX_A_Pin)
#define	MAX_A_L()		GPIO_ResetBits(MAX_A_GpioGroup, MAX_A_Pin)

#define	MAX_B_H()		GPIO_SetBits(MAX_B_GpioGroup, MAX_B_Pin)
#define	MAX_B_L()		GPIO_ResetBits(MAX_B_GpioGroup, MAX_B_Pin)

#define	MAX_C_H()		GPIO_SetBits(MAX_C_GpioGroup, MAX_C_Pin)
#define	MAX_C_L()		GPIO_ResetBits(MAX_C_GpioGroup, MAX_C_Pin)

void MAX4051_Init(void);
void SelectChannel(unsigned char index);
unsigned char GetChannel(void);
#endif


