#ifndef __IAP_F_H__
#define __IAP_F_H__

#include	"Define.h"

void jumpToUserApplicationProgram(void);
void writeApplicationToFlash(void);
MyState_TypeDef checkMd5(void);
void BootLoaderMainFunction(void);

#endif

