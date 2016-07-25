#ifndef _WIFI_D_H_H
#define _WIFI_D_H_H

#include	"stm32f4xx.h"


#define	WPS_Pin			GPIO_Pin_12
#define	WPS_GpioGroup	GPIOC
#define	WPS_Rcc			RCC_AHB1Periph_GPIOC

#define	Reload_Pin			GPIO_Pin_0
#define	Reload_GpioGroup	GPIOD
#define	Reload_Rcc			RCC_AHB1Periph_GPIOD

#define	Reset_Pin			GPIO_Pin_1
#define	Reset_GpioGroup		GPIOD
#define	Reset_Rcc			RCC_AHB1Periph_GPIOD


void Wifi_GPIOInit(void);
void WifiReset(void);

#endif


