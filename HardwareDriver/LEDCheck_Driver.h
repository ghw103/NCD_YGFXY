/****************************************file start****************************************************/
#ifndef _LEDCHECK_D_H
#define _LEDCHECK_D_H

#include	"stm32f4xx_gpio.h"
#include	"Define.h"

#define	LEDCheck_Pin			GPIO_Pin_11
#define	LEDCheck_Group			GPIOE
#define	LEDCheck_RCC			RCC_AHB1Periph_GPIOE

void LEDCheck_Init(void);
MyLEDCheck_TypeDef ReadLEDStatus(void);

#endif

/****************************************end of file************************************************/
