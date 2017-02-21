#ifndef	_SERVER_F_H_H
#define _SERVER_F_H_H

#include	"Define.h"
#include	"tcp.h"
#include 	"api.h" 

#define	SERVERSENDBUFLEN	1024
#define	SERVERRECVBUFLEN	1024

typedef struct
{
	void *myp;									//临时指针
	char sendBuf[SERVERSENDBUFLEN];						//发送数据缓冲区
	unsigned short sendDataLen;					//发送数据长度
	char recvBuf[SERVERRECVBUFLEN];						//发送数据缓冲区
	unsigned int recvDataLen;					//发送数据长度
	
	struct ip_addr server_ipaddr;				//服务器ip
	struct netconn *clientconn;				//当前客户端
	struct netbuf *recvbuf;						//接收缓冲区
	struct pbuf *q;								//接收数据的链表
}MyServerData;

void CommunicateWithServerByLineNet(MyServerData * myServerData);
void CommunicateWithServerByWifi(MyServerData * myServerData);

MyState_TypeDef UpLoadData(char *URL, void * sendBuf, unsigned short sendLen, void * recvBuf, unsigned short recvLen,
	char * sendType);

#endif
