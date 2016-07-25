#ifndef MINFO_FUN_H
#define MINFO_FUN_H

#include 	"FreeRTOS.h"


portBASE_TYPE SelfTestFun_Init(void);
portBASE_TYPE GetSelfTestFunState(unsigned char * receivedchar , portTickType xBlockTime);
portBASE_TYPE SetSelfTestFunState(unsigned char txchar , portTickType xBlockTime);
void SelfTest_Function(void);

#endif

