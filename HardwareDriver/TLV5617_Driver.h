#ifndef TLV5617_D_H
#define TLV5617_D_H

#define	DA_SCK_Pin			GPIO_Pin_10
#define	DA_SCK_Group		GPIOB
#define	DA_SCK_Rcc			RCC_AHB1Periph_GPIOB

#define	DA_SDA_Pin			GPIO_Pin_11
#define	DA_SDA_Group		GPIOB
#define	DA_SDA_Rcc			RCC_AHB1Periph_GPIOB

#define	DA_CS_Pin			GPIO_Pin_9
#define	DA_CS_Group			GPIOE
#define	DA_CS_Rcc			RCC_AHB1Periph_GPIOE

#define DA_SDA_H()	    	GPIO_SetBits(DA_SDA_Group, DA_SDA_Pin)
#define DA_SDA_L()	    	GPIO_ResetBits(DA_SDA_Group, DA_SDA_Pin)
#define DA_SDA_PIN()	    GPIO_ReadInputDataBit(DA_SDA_Group, DA_SDA_Pin) //读取引脚电平

#define	DA_SCK_H()			GPIO_SetBits(DA_SCK_Group, DA_SCK_Pin)
#define	DA_SCK_L()			GPIO_ResetBits(DA_SCK_Group, DA_SCK_Pin)

#define	DA_CS_H()			GPIO_SetBits(DA_CS_Group, DA_CS_Pin)
#define	DA_CS_L()			GPIO_ResetBits(DA_CS_Group, DA_CS_Pin)

void DA_IO_Init(void);
	
unsigned short GetGB_LedValue(void);
void SetGB_LedValue(unsigned short value);

unsigned short GetGB_CLineValue(void);
void SetGB_CLineValue(unsigned short value);

#endif

