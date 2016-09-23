#ifndef __SYSSET_P_H__
#define __SYSSET_P_H__


typedef struct SysSetPageBuffer_tag {
	unsigned short lcdinput[100];
}SysSetPageBuffer;

unsigned char DspSystemSetPage(void *  parm);

#endif

