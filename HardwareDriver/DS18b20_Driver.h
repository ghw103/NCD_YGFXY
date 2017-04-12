#ifndef __DS18B20_H_
#define __DS18B20_H_

#include	"stm32f4xx.h"
#include	"Define.h"

#define DS18B20_HIGH  1
#define DS18B20_LOW   0


/*---------------------------------------*/
#define DS18B20_CLK     RCC_AHB1Periph_GPIOG
#define DS18B20_PIN     GPIO_Pin_7               
#define DS18B20_PORT    GPIOG


#define DS18B20_DATA_OUT(a)	if (a)	\
                                   GPIO_SetBits(DS18B20_PORT,DS18B20_PIN);\
                                   else		\
                                   GPIO_ResetBits(DS18B20_PORT,DS18B20_PIN)

#define  DS18B20_DATA_IN()	  GPIO_ReadInputDataBit(DS18B20_PORT,DS18B20_PIN)


void DS18B20_Init(void);
void startDS18B20(void);
float readDS18B20Temp(void);
void DS18B20_ReadId ( uint8_t * ds18b20_id );					
																	 
#endif //__DS18B20_H_
