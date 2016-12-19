#ifndef __SDI_P_H__
#define __SDI_P_H__

#include	"SystemSet_Data.h"

typedef struct SetDeviceInfoPageBuffer_tag {
	SystemSetData systemSetData;
	unsigned char ismodify;
	unsigned short lcdinput[100];
}SetDeviceInfoPageBuffer;


unsigned char DspSetDeviceInfoPage(void *  parm);

#endif

