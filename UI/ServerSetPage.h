#ifndef __SERVERSET_P_H__
#define __SERVERSET_P_H__

#include	"Define.h"

typedef struct ServerPageBuffer_tag {
	NetData myNetData;
	char buf[100];
	unsigned char ischanged;
}ServerPageBuffer;

unsigned char DspServerSetPage(void *  parm);

#endif

