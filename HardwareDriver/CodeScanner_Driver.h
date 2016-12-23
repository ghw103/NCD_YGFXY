#ifndef CS_D_H_H
#define CS_D_H_H

#include	"stm32f4xx.h"

#define	Trig_Pin			GPIO_Pin_10
#define	Trig_GpioGroup		GPIOG
#define	Trig_Rcc			RCC_AHB1Periph_GPIOG


void CodeScanner_GPIOInit(void);
void OpenCodeScanner(void);
void CloseCodeScanner(void);

#endif


