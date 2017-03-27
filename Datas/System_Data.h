/****************************************file start****************************************************/
#ifndef	SYSTEM_D_H
#define	SYSTEM_D_H

#include	"Define.h"

typedef enum
{
	Connect_Ok = 1,
	Connect_Error = 0
}PaiduiStatus;

void * GetTestDataForLab(void);
void SetTestStatusFlorLab(unsigned char status);
unsigned char GetTestStatusFlorLab(void);

//系统时间
void GetGB_Time(void * time);
void SetGB_Time(void * time);

//环境温度
float GetGB_EnTemperature(void);
void SetGB_EnTemperature(float temp);

//排队模块连接状态
void setPaiduiModuleStatus(PaiduiStatus paiduiStatus);
PaiduiStatus getPaiduiModuleStatus(void);
	
#endif

/****************************************end of file************************************************/

