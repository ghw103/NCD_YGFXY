/****************************************file start****************************************************/
#ifndef	_TEST_T_H
#define	_TEST_T_H

#include	"Define.h"



MyState_TypeDef StartvTestTask(void);

MyState_TypeDef StartTest(void * parm);

MyState_TypeDef TakeTestResult(ResultState *testsult);
MyState_TypeDef StopTest(void);

#endif

/****************************************end of file************************************************/
