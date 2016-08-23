#ifndef __RCODE_F_H__
#define __RCODE_F_H__

#include	"Define.h"

#pragma pack(1)
typedef struct ReadCodeBuffer_tag {
	char originalcode[320];						//原始的二维码
	unsigned short rxcount;						//标记接收数据长度
	char decryptcode[320];						//解密后的二维码
	char tempbuf[64];							//临时缓存
	char * pbuf1;								//临时指针1
	char * pbuf2;								//临时指针2
	MyTime_Def temptime;						//保存当前时间
} ReadCodeBuffer;
#pragma pack()

ScanCodeResult ScanCodeFun(void * parm);

#endif

