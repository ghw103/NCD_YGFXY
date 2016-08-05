#ifndef __SELFC_P_H__
#define __SELFC_P_H__

typedef struct SelCheckPage_Tag
{
	unsigned short lcdinput[100];
}SelCheckPage;

unsigned char DspSelfCheckPage(void *  parm);

#endif

