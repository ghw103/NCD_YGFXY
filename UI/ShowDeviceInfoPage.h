#ifndef __SDEVICEINFO_P_H__
#define __SDEVICEINFO_P_H__


#include	"Define.h"

typedef struct ShowDeviceInfoPageBuffer_Tag
{
	unsigned char presscount;
	DeviceInfo s_deviceinfo;
	unsigned short lcdinput[100];
}ShowDeviceInfoPageBuffer;

unsigned char DspShowDeviceInfoPage(void *  parm);
DeviceInfo * GetDeviceInfo(void);

#endif

