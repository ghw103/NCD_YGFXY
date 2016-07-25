#ifndef PT8211_D_H_H
#define PT8211_D_H_H

#include	"stm32f4xx_gpio.h"

#define	I2S_CK_Pin			GPIO_Pin_3
#define	I2S_CK_Group		GPIOB
#define	I2S_CK_Rcc			RCC_AHB1Periph_GPIOB

#define	I2S_SD_Pin			GPIO_Pin_5
#define	I2S_SD_Group		GPIOB
#define	I2S_SD_Rcc			RCC_AHB1Periph_GPIOB

#define	I2S_WS_Pin			GPIO_Pin_15
#define	I2S_WS_Group		GPIOA
#define	I2S_WS_Rcc			RCC_AHB1Periph_GPIOA

#define	Audio_Mute_Pin			GPIO_Pin_4
#define	Audio_Mute_Group		GPIOE
#define	Audio_Mute_Rcc			RCC_AHB1Periph_GPIOE

#define	I2S_WS_H()			GPIO_SetBits(I2S_WS_Group, I2S_WS_Pin)
#define	I2S_WS_L()			GPIO_ResetBits(I2S_WS_Group, I2S_WS_Pin)

typedef enum
{
	Audio_Mode = 0,
	Mute_Mode = 1
}AudioMode;

extern void (*i2s_tx_callback)(void);

void PT8211_GPIOInit(void);
void SetAudioMode(AudioMode mode);
void StartPlay(void);
void StopPlay(void);
void I2S_DMA_Init(unsigned char *buf0, unsigned char *buf1, unsigned short num);
void ConfigI2S(unsigned short audiofreq);

#endif


