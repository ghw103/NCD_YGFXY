#ifndef __CODE_T_H__
#define __CODE_T_H__

#include	"Define.h"

MyState_TypeDef StartCodeScanTask(void);
MyState_TypeDef StartScanQRCode(void * parm);
MyState_TypeDef TakeScanQRCodeResult(ScanCodeResult *scanresult);

#endif

