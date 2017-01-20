#ifndef	_UPLOAD_F_H_H
#define _UPLOAD_F_H_H

#include	"Define.h"
#include	"TestDataDao.h"
#include	"SystemSet_Data.h"

#pragma pack(1)
typedef struct UpLoadTestDataBuffer_Tag
{	
	ReadTestDataPackage readTestDataPackage;
	SystemSetData systemSetData;
	TestData * testData;
	char sendbuf[2048];
	char tempbuf[100];
}UpLoadTestDataBuffer;
#pragma pack()

void UpLoadFunction(void);

#endif
