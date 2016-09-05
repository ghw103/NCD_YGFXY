#ifndef	_SERVER_F_H_H
#define _SERVER_F_H_H

#include	"Define.h"

MyState_TypeDef CommunicateWithServerByLineNet(void *sendnetbuf, void *recvnetbuf);


MyState_TypeDef UpLoadData(char *URL, void * buf, unsigned short buflen);

#endif
