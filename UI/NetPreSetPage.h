#ifndef __NETPRESET_P_H__
#define __NETPRESET_P_H__

#include	"UI_Data.h"
#include	"SystemSet_Data.h"

typedef struct NetPrePageBuffer_tag {
	unsigned short lcdinput[100];
}NetPrePageBuffer;


MyState_TypeDef createNetPreActivity(Activity * thizActivity, Intent * pram);

#endif

