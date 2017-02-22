#ifndef __ENCRYPT_T_H__
#define __ENCRYPT_T_H__

#include	"Define.h"


typedef struct PointBuffer_Tag
{
	FatfsFileInfo_Def myfile;
	unsigned char word[94][94];
	unsigned char key[60];
	unsigned char *q;
	unsigned char *k;
	unsigned char *t;
	unsigned char *s;
	unsigned short i;
	unsigned short j;
	unsigned short tempIndex;
	unsigned short tempV1;
	unsigned short tempV2;
}PointBuffer;

unsigned char MyDencrypt(char *source, char *target, unsigned short len);

#endif

