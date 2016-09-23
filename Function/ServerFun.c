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
#include	"System_Data.h"
#include 	"Usart4_Driver.h"
#include 	"usbd_cdc_vcp.h"

#include	"MyMem.h"

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

MyState_TypeDef CommunicateWithServerByLineNet(void *sendnetbuf, void *recvnetbuf, unsigned char ip1, unsigned char ip2, 
	unsigned char ip3, unsigned char ip4, unsigned short maxrecvlen)
{
	err_t err;
	
	MyState_TypeDef status = My_Fail;
	
	MyLwipData * myclientdata = NULL;
	
	myclientdata = MyMalloc(sizeof(MyLwipData));
	
	if(myclientdata)
	{
		IP4_ADDR(&myclientdata->server_ipaddr,ip1,ip2,ip3,ip4);
		
		//创建连接
		myclientdata->clientconn = netconn_new(NETCONN_TCP);
		//创建失败
		if(myclientdata->clientconn == NULL)
			goto END;
		
		//尝试连接远程服务器
		err = netconn_connect(myclientdata->clientconn, &myclientdata->server_ipaddr, 8080);
		//连接失败
		if(err != ERR_OK)
			goto END2;
		
		//设置接收数据超时时间100MS
		myclientdata->clientconn->recv_timeout = 1000;
		
		//发送数据
		if(sendnetbuf)
		{
			myclientdata->s_mybuf = sendnetbuf;
			err = netconn_write(myclientdata->clientconn, myclientdata->s_mybuf->data, myclientdata->s_mybuf->datalen, NETCONN_COPY );
			//发送失败
			if(err != ERR_OK)
				goto END1;
		}
		
		//接收数据
		if(recvnetbuf)
		{
			myclientdata->s_mybuf = recvnetbuf;
			err = netconn_recv(myclientdata->clientconn, &myclientdata->recvbuf);
			if(err == ERR_OK)
			{
				myclientdata->s_mybuf->datalen = netbuf_copy(myclientdata->recvbuf, myclientdata->s_mybuf->data, maxrecvlen);
				
				netbuf_delete(myclientdata->recvbuf);
				
				status = My_Pass;
			}
		}
		
		END1:
			netconn_close(myclientdata->clientconn);
		
		END2:
			netconn_delete(myclientdata->clientconn);
	}
	
	END:
		MyFree(myclientdata);
		return status;
}

MyState_TypeDef CommunicateWithServerByWifi(void *sendnetbuf, void *recvnetbuf, unsigned short maxrecvlen)
{
	MyWifiData *mywifidata;
	MyState_TypeDef status = My_Fail;
	
	mywifidata = MyMalloc(sizeof(MyWifiData));
	if(mywifidata)
	{
		//发送数据
		if(sendnetbuf)
		{
			mywifidata->s_mybuf = sendnetbuf;
			mywifidata->myp = mywifidata->s_mybuf->data;
			
			while(mywifidata->s_mybuf->datalen > 100)
			{
				SendDataToQueue(GetUsart4TXQueue(), GetUsart4TXMutex(), mywifidata->myp, 100, 1, 100 / portTICK_RATE_MS, EnableUsart4TXInterrupt);
				mywifidata->s_mybuf->datalen -= 100;
				mywifidata->myp += 100;
			}
		
			SendDataToQueue(GetUsart4TXQueue(), GetUsart4TXMutex(), mywifidata->myp, mywifidata->s_mybuf->datalen, 1, 100 / portTICK_RATE_MS, EnableUsart4TXInterrupt);
		}
		
		//接收数据
		if(recvnetbuf)
		{
			mywifidata->rxcount = 0;
			mywifidata->s_mybuf = recvnetbuf;
			mywifidata->myp = mywifidata->s_mybuf->data;
			
			while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), GetUsart4RXMutex(), mywifidata->myp+mywifidata->rxcount, 1, 1, 100 / portTICK_RATE_MS))
			{
				if(mywifidata->rxcount < maxrecvlen-1)
					mywifidata->rxcount++;	
			}
			
			if(mywifidata->rxcount > 0)
				status = My_Pass;
		}
	}
	
	MyFree(mywifidata);
	return status;
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
	MyState_TypeDef statues = My_Fail;
	mynetbuf mybuf;
	mynetbuf recvbuf;
	char * temp;
	
	mybuf.data = MyMalloc(buflen+512);
	recvbuf.data = MyMalloc(1024);
	if(mybuf.data && recvbuf.data)
	{
		memset(mybuf.data, 0, buflen+512);
		sprintf(mybuf.data, "POST %s HTTP/1.1\nHost: 123.57.94.39:8080\nConnection: keep-alive\nContent-Length: %d\nContent-Type:application/x-www-form-urlencoded;charset=GBK\n\n%s", URL, buflen, (char *)buf);
		mybuf.datalen = strlen(mybuf.data);
		
		memset(recvbuf.data, 0, 1024);
		if(My_Pass == CommunicateWithServerByLineNet(&mybuf, &recvbuf, 123, 57, 94, 39, 1000))
		{
			temp = strstr(recvbuf.data, "myresult->");
			if(temp)
			{
				//USB_PutStr("lwip_mode ok\n", 13);
				memset(buf, 0, buflen);
				memcpy(buf, temp, strlen(temp));
				statues = My_Pass;
				goto END1;
			}
			else
				;//USB_PutStr("lwip_mode error\n", 16);
		}
		
		memset(recvbuf.data, 0, 1024);
		if(My_Pass == CommunicateWithServerByWifi(&mybuf, &recvbuf, 1000))
		{
			temp = strstr(recvbuf.data, "myresult->");
			if(temp)
			{
				//USB_PutStr("wifi_mode ok\n", 13);
				memset(buf, 0, buflen);
				memcpy(buf, temp, strlen(temp));
				statues = My_Pass;
				goto END1;
			}
			else
				;//USB_PutStr("wifi_mode error\n", 16);
		}
		
		END1:
			;
	}
	MyFree(mybuf.data);
	MyFree(recvbuf.data);
	
	return statues;
}

