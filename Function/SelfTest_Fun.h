#ifndef MINFO_FUN_H
#define MINFO_FUN_H

#include	"Define.h"

MyState_TypeDef GetSelfCheckStatus(unsigned char * receivedchar , portTickType xBlockTime);

MyState_TypeDef SetSelfCheckStatus(unsigned char txchar , portTickType xBlockTime);

void SelfTest_Function(void);

#endif

