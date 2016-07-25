#ifndef RTC_D_H
#define RTC_D_H

#include	"stm32f4xx_rtc.h"
#include	"Define.h"


MyState_TypeDef My_RTC_Init(void);
MyState_TypeDef SetTime(unsigned char year, unsigned char month, unsigned char day, unsigned char week, unsigned char hour,
	unsigned char minute, unsigned char second);
	
#endif

