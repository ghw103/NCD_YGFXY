#ifndef SELFCHECK_FUN_H
#define SELFCHECK_FUN_H

#include	"Define.h"

typedef enum
{ 
	SystemData_OK = 0,								//系统数据加载成功
	SystemData_ERROR = 1,							//系统数据加载失败
	
	Light_OK = 2,									//led正常
	Light_Error = 3,								//led异常
	
	AD_OK = 4,										//采集模块正常
	AD_ERROR = 5,									//采集模块错误
	
	Motol_OK = 6,									//传动模块正常
	Motol_ERROR = 7,								//传动模块错误
	
	Erweima_OK = 8,									//二维码模块正常
	Erweima_ERROR = 9,								//二维码模块错误
	
	SelfTestting = 99,								//自检中
	SelfTest_OK = 100								//自检成功
}ERROR_SelfTest;

void SelfTest_Function(void);
MyState_TypeDef readSelfTestStatus(ERROR_SelfTest * selfTest);

#endif

