#ifndef	_SERVER_F_H_H
#define _SERVER_F_H_H

#include	"Define.h"

void SendDataToNCDServer(char *buf, unsigned short len);
MyState_TypeDef RecvDataFromNCDServer(char *buf);

void SendDataToUserServer(char *buf, unsigned short len);
MyState_TypeDef UpLoadData(char *URL, void * buf, unsigned short buflen);

#endif
