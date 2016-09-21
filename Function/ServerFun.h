#ifndef	_SERVER_F_H_H
#define _SERVER_F_H_H

#include	"Define.h"
#include	"tcp.h"
#include 	"api.h" 


typedef struct
{
	struct ip_addr server_ipaddr;				//服务器ip
	struct netconn *clientconn;				//当前客户端
	struct netbuf *recvbuf;						//接收缓冲区
	struct pbuf *q;								//接收数据的链表
	unsigned char *myp;							//临时指针
	mynetbuf * s_mybuf;							//临时网络数据包
}MyLwipData;


#pragma pack(1)
typedef struct
{
	unsigned char *myp;							//临时指针
	mynetbuf * s_mybuf;							//临时网络数据包
	unsigned short rxcount;						//接收数据长度
}MyWifiData;
#pragma pack()

MyState_TypeDef CommunicateWithServerByLineNet(void *sendnetbuf, void *recvnetbuf, unsigned char ip1, unsigned char ip2, 
	unsigned char ip3, unsigned char ip4, unsigned short maxrecvlen);
MyState_TypeDef CommunicateWithServerByWifi(void *sendnetbuf, void *recvnetbuf, unsigned short maxrecvlen);

MyState_TypeDef UpLoadData(char *URL, void * buf, unsigned short buflen);

#endif
