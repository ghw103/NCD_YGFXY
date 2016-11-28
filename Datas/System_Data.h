/****************************************file start****************************************************/
#ifndef	SYSTEM_D_H
#define	SYSTEM_D_H

#include	"Define.h"

void * GetTestDataForLab(void);
void SetTestStatusFlorLab(unsigned char status);
unsigned char GetTestStatusFlorLab(void);

//条码枪数据
void GetGB_BarCode(void *buf);
void SetGB_BarCode(void *buf);

//系统时间
void GetGB_Time(void * time);
void SetGB_Time(void * time);

//系统网络配置
void GetGB_NetConfigureData(NetData * netdata);
void SetGB_NetConfigureData(NetData * netdata);
NetIP_Type GetGB_IpModeData(void);
	
#endif

/****************************************end of file************************************************/

