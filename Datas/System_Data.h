/****************************************file start****************************************************/
#ifndef	SYSTEM_D_H
#define	SYSTEM_D_H

void SetGB_DeviceInfo(void *info);
void *GetGB_DeviceInfo(void);
void * GetTestDataForLab(void);

void SetTestStatusFlorLab(unsigned char status);
unsigned char GetTestStatusFlorLab(void);

//条码枪数据
void GetGB_BarCode(void *buf);
void SetGB_BarCode(void *buf);

//系统时间
void GetGB_Time(void * time);
void SetGB_Time(void * time);
#endif

/****************************************end of file************************************************/

