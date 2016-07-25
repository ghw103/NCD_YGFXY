#ifndef __SPI2_D_H
#define __SPI2_D_H

#define	SPI2_CS_ENABLE()  GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define SPI2_CS_DISABLE() GPIO_SetBits(GPIOB,GPIO_Pin_12)



void SPI2_Init(void);
unsigned char SPI2_ReadWrite(unsigned char writedat);
void SPI2_SetSpeed(unsigned char SPI_BaudRatePrescaler);

#endif
