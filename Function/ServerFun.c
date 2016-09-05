/***************************************************************************************************
*FileName:
*Description:
*Author:
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"ServerFun.h"
#include	"QueueUnits.h"
#include	"Net_Data.h"

#include	"MyMem.h"

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	"tcp.h"
#include 	"api.h" 

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/
static struct ip_addr server_ipaddr;
static struct netconn *tcp_clientconn;
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

MyState_TypeDef CommunicateWithServerByLineNet(void *sendnetbuf, void *recvnetbuf)
{
	err_t err;
	struct netbuf *recvbuf;
	struct pbuf *q;
	unsigned char *myp = NULL;
	MyState_TypeDef status = My_Fail;
	mynetbuf * s_mybuf = sendnetbuf;
	
	IP4_ADDR(&server_ipaddr,192,168,0,15);
	
	tcp_clientconn = netconn_new(NETCONN_TCP);
	netconn_bind(tcp_clientconn , IP_ADDR_ANY , 9601);
	err = netconn_connect(tcp_clientconn,&server_ipaddr,8080);
	tcp_clientconn->recv_timeout = 1000;
	//连接失败
	if(err != ERR_OK)
	{
		netconn_delete(tcp_clientconn);
		return status;
	}
	
	netconn_write( tcp_clientconn, s_mybuf->data, s_mybuf->datalen, NETCONN_COPY );
	
	memset(s_mybuf->data, 0, s_mybuf->datalen);
	err = netconn_recv(tcp_clientconn,&recvbuf);
	if(err == ERR_OK)
	{
		s_mybuf = recvnetbuf;
		
		q = recvbuf->p;
		
		s_mybuf->datalen = q->tot_len;
		myp = s_mybuf->data;
		
		for(; q!=NULL; q=q->next)
		{
			memcpy(myp, q->payload, q->len);
			myp += q->len;
		}
		
		netbuf_delete(recvbuf);
		
		status = My_Pass;
	}
	
	netconn_close( tcp_clientconn );
	netconn_delete(tcp_clientconn);
}


/***************************************************************************************************
*FunctionName：
*Description：
*Input：None
*Output：None
*Author：xsx
*Data：
***************************************************************************************************/
MyState_TypeDef UpLoadData(char *URL, void * buf, unsigned short buflen)
{
	char *data = NULL;
	MyState_TypeDef statues = My_Fail;
	mynetbuf mybuf;
	
	mybuf.data = MyMalloc(buflen+1024);
	if(mybuf.data)
	{
		memset(mybuf.data, 0, buflen+1024);
		sprintf(mybuf.data, "POST %s HTTP/1.1\nHost: 123.57.94.39\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\nReferer: %s\n\n%s", URL, buflen, URL, (char *)buf);
		mybuf.datalen = strlen(mybuf.data);
		
		if(My_Pass == CommunicateWithServerByLineNet(&mybuf, &mybuf))
		{
			if(strstr(data, "CREATED"))
				statues = My_Pass;
		}
	}
	MyFree(mybuf.data);
	
	return statues;
}

