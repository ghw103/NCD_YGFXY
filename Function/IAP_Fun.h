#ifndef __IAP_F_H__
#define __IAP_F_H__

#include	"Define.h"
#include	"RemoteSoft_Data.h"

void jumpToUserApplicationProgram(void);
void writeApplicationToFlash(void);
MyState_TypeDef checkMd5(RemoteSoftInfo * remoteSoftInfo);
void BootLoaderMainFunction(void);

#endif

