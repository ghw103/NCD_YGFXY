#ifndef __MYSPI_D_H
#define __MYSPI_D_H

#include 	"stm32f4xx.h"

#define	MySPI_CS_ENABLE()  GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define MySPI_CS_DISABLE() GPIO_SetBits(GPIOB,GPIO_Pin_12)

#define	MySPI_CLK_L			GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define	MySPI_CLK_H			GPIO_SetBits(GPIOB,GPIO_Pin_13)

#define	MySPI_MOSI_L		GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define	MySPI_MOSI_H		GPIO_SetBits(GPIOB,GPIO_Pin_14)

#define MySPI_MISO_IN()	    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) //读取引脚电平


void	MySPI_Init(void);
unsigned char	MySPI_ReadWriteByte(unsigned char writedat);

#endif
