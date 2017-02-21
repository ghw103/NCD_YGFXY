/****************************************file start****************************************************/
#ifndef	SYSTEM_D_H
#define	SYSTEM_D_H

#include	"Define.h"


void * GetTestDataForLab(void);
void SetTestStatusFlorLab(unsigned char status);
unsigned char GetTestStatusFlorLab(void);

//系统时间
void GetGB_Time(void * time);
void SetGB_Time(void * time);

//环境温度
float GetGB_EnTemperature(void);
void SetGB_EnTemperature(float temp);

	
#endif

/****************************************end of file************************************************/

