#ifndef	_UPLOAD_F_H_H
#define _UPLOAD_F_H_H

#include	"Define.h"
#include	"TestDataDao.h"
#include	"SystemSet_Data.h"

#define	UPLOADSENDBUFLEN	2048
#define	UPLOADRECVBUFLEN	1024
#define	UPLOADTEMPBUFLEN	128

#pragma pack(1)
typedef struct UpLoadDeviceDataBuffer_Tag
{
	SystemSetData systemSetData;
	char sendBuf[UPLOADSENDBUFLEN];
	char recvBuf[UPLOADRECVBUFLEN];
	char tempBuf[UPLOADTEMPBUFLEN];
	unsigned short i;
	unsigned short j;
}UpLoadDeviceDataBuffer;
#pragma pack()

#pragma pack(1)
typedef struct UpLoadTestDataBuffer_Tag
{
	PageRequest pageRequest;
	Page page;
	SystemSetData systemSetData;
	TestData * testData;
	char sendBuf[UPLOADSENDBUFLEN];
	char recvBuf[UPLOADRECVBUFLEN];
	char tempBuf[UPLOADTEMPBUFLEN];
	unsigned char k;
	unsigned short i;
	unsigned short j;
}UpLoadTestDataBuffer;
#pragma pack()

void UpLoadFunction(void);

#endif
