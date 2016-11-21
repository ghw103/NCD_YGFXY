#ifndef __MLX90614_H
#define __MLX90614_H

#include "stm32f4xx.h"

#define	MLX90614_SCK_Pin			GPIO_Pin_1
#define	MLX90614_SDA_Pin			GPIO_Pin_0
#define	MLX90614_GpioGroup			GPIOF
#define	MLX90614_Rcc				RCC_AHB1Periph_GPIOF

#define SMBUS_SDA_H()	    GPIO_SetBits(MLX90614_GpioGroup, MLX90614_SDA_Pin)
#define SMBUS_SDA_L()	    GPIO_ResetBits(MLX90614_GpioGroup, MLX90614_SDA_Pin)
#define SMBUS_SDA_PIN()	    GPIO_ReadInputDataBit(MLX90614_GpioGroup, MLX90614_SDA_Pin) //读取引脚电平

#define	SMBUS_SCK_H()		GPIO_SetBits(MLX90614_GpioGroup, MLX90614_SCK_Pin)
#define	SMBUS_SCK_L()		GPIO_ResetBits(MLX90614_GpioGroup, MLX90614_SCK_Pin)

#define ACK	 0
#define	NACK 1
#define SA				0x00 //Slave address 单个MLX90614时地址为0x00,多个时地址默认为0x5a
#define RAM_ACCESS		0x00 //RAM access command
#define EEPROM_ACCESS	0x20 //EEPROM access command
#define RAM_TOBJ1		0x07 //To1 address in the eeprom

void SMBus_Init(void);
float GetCardTemperature(void); //获取温度值

#endif


