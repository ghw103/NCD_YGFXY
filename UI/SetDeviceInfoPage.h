#ifndef __SDI_P_H__
#define __SDI_P_H__

#include	"Define.h"

typedef struct SetDeviceInfoPageBuffer_tag {
	DeviceInfo temp_deviceinfo;							//ÁÙÊ±Êý¾Ý
	unsigned char ismodify;
	unsigned short lcdinput[100];
}SetDeviceInfoPageBuffer;


unsigned char DspSetDeviceInfoPage(void *  parm);

#endif

