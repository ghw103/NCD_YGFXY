/****************************************file start****************************************************/
#ifndef	INTENT_H_H
#define	INTENT_H_H

#include	"Define.h"

#pragma pack(1)
typedef struct Intent_tag
{
	void * data;
	unsigned short datalen;
}Intent;
#pragma pack()

Intent * createIntent(void * data, unsigned short datalen);
void readIntent(Intent * intent, void * data, unsigned short datalen);

#endif

/****************************************end of file************************************************/
