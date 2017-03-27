#ifndef _TM1623_D_H_H
#define _TM1623_D_H_H

#include	"stm32f4xx.h"

#define	TM1623_SCK_Pin			GPIO_Pin_8
#define	TM1623_SCK_Group		GPIOA
#define	TM1623_SCK_Rcc			RCC_AHB1Periph_GPIOA

#define	TM1623_SDA_Pin			GPIO_Pin_9
#define	TM1623_SDA_Group		GPIOC
#define	TM1623_SDA_Rcc			RCC_AHB1Periph_GPIOC

#define	TM1623_STB_Pin			GPIO_Pin_8
#define	TM1623_STB_Group		GPIOG
#define	TM1623_STB_Rcc			RCC_AHB1Periph_GPIOG


typedef enum
{ 
	R_OFF_G_OFF = 0,
	R_ON_G_OFF = 1,
	R_OFF_G_ON = 2,
	R_ON_G_ON = 3,
}TM1623_LED_State;

typedef enum
{ 
	KEY_Pressed = 1,
	KEY_NoPressed = 0,
}TM1623_KEY_State;

void TM1623_Init(void);
void TM1623_WriteByte(unsigned char data);
void TM1623_Send_CMD(unsigned char cmd);
void TM1623_ReadKey(void);

void ChangeTM1623LedStatues(unsigned char ledindex, TM1623_LED_State statues);
TM1623_LED_State GetLedStatues(unsigned char ledindex);

TM1623_KEY_State GetTheKeyStatues(unsigned char keyindex);
unsigned char getTM1623KeyData(unsigned char index);
#endif
















