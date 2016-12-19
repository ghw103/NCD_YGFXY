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
	
#endif

/****************************************end of file************************************************/

