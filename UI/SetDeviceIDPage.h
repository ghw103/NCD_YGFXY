#ifndef __SETDEVICEID_P_H__
#define __SETDEVICEID_P_H__

#include	"SystemSet_Data.h"

typedef struct SetDeviceIDPage_tag {
	SystemSetData systemSetData;
	unsigned char tempbuf[150];								//¡Ÿ ±÷∏’Î
	unsigned char ismodify;
	unsigned short lcdinput[100];
}SetDeviceIDPage;

unsigned char DspSetDeviceIDPage(void *  parm);

#endif

