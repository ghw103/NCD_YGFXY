#ifndef ADS8325_D_H_H
#define ADS8325_D_H_H

#include	"stm32f4xx.h"

#define	ADS_SCK_Pin			GPIO_Pin_5
#define	ADS_SDA_Pin			GPIO_Pin_6
#define	ADS_CS_Pin			GPIO_Pin_4
#define	ADS_GpioGroup		GPIOA
#define	ADS_Rcc				RCC_AHB1Periph_GPIOA

#define ADS_SDA_H()	    GPIO_SetBits(ADS_GpioGroup, ADS_SDA_Pin)
#define ADS_SDA_L()	    GPIO_ResetBits(ADS_GpioGroup, ADS_SDA_Pin)
#define ADS_SDA_PIN()	    GPIO_ReadInputDataBit(ADS_GpioGroup, ADS_SDA_Pin) //读取引脚电平

#define	ADS_SCK_H()		GPIO_SetBits(ADS_GpioGroup, ADS_SCK_Pin)
#define	ADS_SCK_L()		GPIO_ResetBits(ADS_GpioGroup, ADS_SCK_Pin)

#define	ADS_CS_H()		GPIO_SetBits(ADS_GpioGroup, ADS_CS_Pin)
#define	ADS_CS_L()		GPIO_ResetBits(ADS_GpioGroup, ADS_CS_Pin)


void ADS_IO_Init(void);
double ADS8325(void);

#endif


