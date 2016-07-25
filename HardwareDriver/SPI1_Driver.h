#ifndef __SPI1_D_H
#define __SPI1_D_H

#include 	"stm32f4xx.h"

#define	SPI1_CS_ENABLE()  GPIO_ResetBits(GPIOA,GPIO_Pin_15)
#define SPI1_CS_DISABLE() GPIO_SetBits(GPIOA,GPIO_Pin_15)


void	SPI1_Init(void);
unsigned char	SPI1_ReadWriteByte(unsigned char writedat);

#endif
