#ifndef	_UPLOAD_F_H_H
#define _UPLOAD_F_H_H

#include	"Define.h"
#include	"TestDataDao.h"

#pragma pack(1)
typedef struct UpLoadTestDataBuffer_Tag
{	
	DeviceInfo deviceinfo;
	TestDataHead myTestDataSaveHead;
	TestData testdata;
	
	char sendbuf[2048];
	char tempbuf[10];
}UpLoadTestDataBuffer;
#pragma pack()

void UpLoadFunction(void);

#endif
