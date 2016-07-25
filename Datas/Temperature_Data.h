#ifndef _TEMP_S_D_H__
#define _TEMP_S_D_H__


#include	"Define.h"



MyTemp_Def *GetGBTempData(void);

void SetCardTemperature(float temp);
float GetCardTemperature(void);

void SetEnviTemperature(float temp);
float GetEnviTemperature(void);

#endif

