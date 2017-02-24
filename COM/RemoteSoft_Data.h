#ifndef _REMOTESOFT_D_H__
#define _REMOTESOFT_D_H__


#include	"Define.h"

#pragma pack(1)
typedef struct
{
	unsigned short RemoteFirmwareVersion;
	char md5[32];
	char desc[200];
	bool isSuccessDownloadFirmware;
}RemoteSoftInfo; 
#pragma pack()

void setGbRemoteFirmwareVersion(unsigned short version);
unsigned short getGbRemoteFirmwareVersion(void);
void setIsSuccessDownloadFirmware(bool status);
bool getIsSuccessDownloadFirmware(void);
void setGbRemoteFirmwareMd5(char * md5);
bool checkMd5IsSame(char * sMd5, char * dMd5);

#endif

