#ifndef __RCODE_F_H__
#define __RCODE_F_H__

#include	"Define.h"

#define	MAX_QR_CODE_LENGHT	300					//二维码最大长度
#define	MAX_SCAN_QR_TIME	7					//扫描二维码时间

#pragma pack(1)
typedef struct ReadQRCodeBuffer_tag {
	char originalcode[MAX_QR_CODE_LENGHT+1];						//原始的二维码
	unsigned short originalCodeLen;									//原始数据长度
	char decryptcode[MAX_QR_CODE_LENGHT+1];						//解密后的二维码
	char tempbuf[64];											//临时缓存
	char * pbuf1;												//临时指针1
	char * pbuf2;												//临时指针2
	MyTime_Def temptime;										//保存当前时间
	QRCode * cardQR;											//保存二维码
	Timer timer;												//计时器
	unsigned short motorLocation;								//电机位置
	unsigned char motorDir;										//电机方向
	ScanCodeResult scanresult;
}ReadQRCodeBuffer;
#pragma pack()

ScanCodeResult ScanCodeFun(QRCode * parm);

#endif

