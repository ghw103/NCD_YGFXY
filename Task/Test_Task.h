/****************************************file start****************************************************/
#ifndef	_TEST_T_H
#define	_TEST_T_H

#include	"Define.h"



char StartvTestTask(void);

MyState_TypeDef StartTest(void * parm);

MyState_TypeDef TakeTestResult(ResultState *testsult);

void clearTestResult(void);
#endif

/****************************************end of file************************************************/
