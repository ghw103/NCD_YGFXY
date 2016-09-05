/***************************************************************************************************
*FileName:WifiFunction
*Description:一些WIFI模块的操作函数,函数只能在线程中使用
*Author:xsx
*Data:2016年4月30日16:06:36
***************************************************************************************************/


/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include	"WifiFunction.h"

#include	"ServerFun.h"
#include 	"Usart4_Driver.h"
#include	"QueueUnits.h"

#include	"CRC16.h"
#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

static xSemaphoreHandle xMutex = NULL;									//wifi互斥量


/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static MyState_TypeDef ComWithWIFI(char * cmd, const char *strcmp, char *buf, unsigned short buflen, portTickType xBlockTime);
static void ProgressWifiListData(WIFI_Def *wifis, char *buf);

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void InitMutex(void)
{
	if(xMutex == NULL)
	{
		vSemaphoreCreateBinary(xMutex);
		
		xSemaphoreGive(xMutex);
	}
}

unsigned char WaitWifiFree(portTickType xBlockTime)
{
	
	return xSemaphoreTake(xMutex, xBlockTime);
}

void SetWifiFree(void)
{
	xSemaphoreGive(xMutex);
}

static MyState_TypeDef ComWithWIFI(char * cmd, const char *strcmp, char *buf, unsigned short buflen, portTickType xBlockTime)
{
	unsigned short rxcount = 0;
	MyState_TypeDef statues = My_Fail;
	
	if(pdPASS == WaitWifiFree(100/portTICK_RATE_MS))
	{
		if(pdPASS == SendDataToQueue(GetUsart4TXQueue(), GetUsart4TXMutex(), cmd, strlen(cmd), 1, 50 * portTICK_RATE_MS, EnableUsart4TXInterrupt))
		{
			if(buf)
			{
				memset(buf, 0, buflen);
				while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), GetUsart4RXMutex(), buf+rxcount, 1, 1, xBlockTime))
					rxcount++;
				
				if(rxcount > 0)
				{
					if(strcmp)
					{
						if(strstr(buf, strcmp))
							statues = My_Pass;
					}
					else
						statues = My_Pass;
				}
			}
			else
				statues = My_Pass;
		}

		SetWifiFree();
	}

	return statues;
}


MyState_TypeDef WIFICheck(void)
{
	MyState_TypeDef statues = My_Fail;
	
	if(AT_Mode != GetWifiWorkMode())
	{
		/*进入at模式成功*/
		if(My_Pass != SetWifiWorkInAT(AT_Mode))
			return My_Fail;
	}
	
//	if(Wifi_Mode == GetNetCard())
	{
		SetWifiSocketA();
	
		SetWifiSocketB();
	}
//	else
	{
//		CloseSocketA();
//		CloseSocketB();
	}
	
	SetWifiDefaultWorkMode();
	
	if(My_Pass == CheckWifiMID())
		statues = My_Pass;

	RestartWifi();

	return statues;
}
/***************************************************************************************************
*FunctionName：SetWifiWorkInAT
*Description：设置wifi模块工作在AT模式
*Input：mode -- 工作模式
*Output：None
*Author：xsx
*Data：2016年3月14日10:43:46
***************************************************************************************************/
MyState_TypeDef SetWifiWorkInAT(WIFI_WorkMode_DefType mode)
{
	MyState_TypeDef statues = My_Fail;
	char *txbuf = NULL; 
	
	txbuf = MyMalloc(50);
	if(txbuf)
	{
		if(mode == AT_Mode)
		{
			if(My_Pass == ComWithWIFI("+++", "a", txbuf, 50, 500 * portTICK_RATE_MS))
			{
				if(My_Pass == ComWithWIFI("a", "+ok", txbuf, 50, 500 * portTICK_RATE_MS))
					statues = My_Pass;
			}
		}
		else
		{
			if(My_Pass == ComWithWIFI("AT+ENTM\r", "+ok", txbuf, 50, 50 * portTICK_RATE_MS))
				statues = My_Pass;
		}
		MyFree(txbuf);
	}
	
	return statues;
}

MyState_TypeDef SetWifiDefaultWorkMode(void)
{
	char *txbuf = NULL;
	MyState_TypeDef statues = My_Fail;
	
	txbuf = MyMalloc(50);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+TMODE=cmd\r", "+ok", txbuf, 50, 500 * portTICK_RATE_MS))
			statues = My_Pass;
	}
	
	MyFree(txbuf);
	
	return statues;
}

WIFI_WorkMode_DefType GetWifiWorkMode(void)
{
	char *txbuf = NULL; 
	WIFI_WorkMode_DefType mode = None;
	
	txbuf = MyMalloc(50);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("+++\r", "+++", txbuf, 50, 50 * portTICK_RATE_MS))
			mode = AT_Mode;
		else
			mode = Normal_Mode;
	}
	
	MyFree(txbuf);
	
	return mode;
}

MyState_TypeDef ScanApList(WIFI_Def *wifis)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;

	txbuf = MyMalloc(1000);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WSCAN\r", NULL, txbuf, 1000, 1000 / portTICK_RATE_MS))
			ProgressWifiListData(wifis, txbuf);
		
		MyFree(txbuf);
	}
	
	return statues;
}

static void ProgressWifiListData(WIFI_Def *wifis, char *buf)
{
	static char *tempbuf, *tempbuf2, *tempbuf3;
	static char * (wifistr[MaxWifiListNum]);
	unsigned char i,j;
	static char buff[100];
	
	tempbuf = strstr(buf, "\r\n");

	if(NULL == tempbuf)
		return;
	
	for(i=0; i<MaxWifiListNum; i++)
	{
		if(i == 0)
			wifistr[i] = strtok(tempbuf+2, "\n\r");
		else
			wifistr[i] = strtok(NULL, "\n\r");
		
		if(NULL == wifistr[i])
			break;
	}
	
	for(i=0; i<MaxWifiListNum; i++)
	{
		tempbuf3 = wifistr[i];
		if(tempbuf3)
		{
			for(j=0; j<5; j++)
			{
				if(j == 0)
					tempbuf2 = strtok(tempbuf3, ",");
				else
					tempbuf2 = strtok(NULL, ",");
					
				if(tempbuf2)
				{
					if(j == 1)
						memcpy(wifis[i].ssid, tempbuf2, strlen(tempbuf2)) ;
					else if(j == 3)
					{
						if(strstr(tempbuf2, "OPEN"))
							memcpy(wifis[i].auth, "OPEN", 4);
						else if(strstr(tempbuf2, "SHARED"))
							memcpy(wifis[i].auth, "SHARED", 6);
						else if(strstr(tempbuf2, "WPAPSK"))
							memcpy(wifis[i].auth, "WPAPSK", 6);
						else if(strstr(tempbuf2, "WPA2PSK"))
							memcpy(wifis[i].auth, "WPA2PSK", 7);
							
						if(strstr(tempbuf2, "NONE"))
							memcpy(wifis[i].encry, "NONE", 4);
						else if(strstr(tempbuf2, "WEP-H"))
							memcpy(wifis[i].encry, "WEP-H", 5);
						else if(strstr(tempbuf2, "WEP-A"))
							memcpy(wifis[i].encry, "WEP-A", 5);
						else if(strstr(tempbuf2, "TKIP"))
							memcpy(wifis[i].encry, "TKIP", 4);
						else if(strstr(tempbuf2, "AES"))
							memcpy(wifis[i].encry, "AES", 3);
					}
					else if(j == 4)
						wifis[i].indicator = strtol(tempbuf2 , NULL , 10);
						
					if(j == 4)
					{
						sprintf(buff, "ssid:%s security:%s,%s xinhao:%d\r\n", wifis[i].ssid, wifis[i].auth, wifis[i].encry, wifis[i].indicator);
						//SendDataToNCDServer(buff, strlen(buff));
					}
				}
			}				
		}
		else
			break;
	}
}

MyState_TypeDef ConnectWifi(WIFI_Def *wifis)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(100);
	if(txbuf)
	{
		/*设置ssid*/
		memset(txbuf, 0, 100);
		sprintf(txbuf, (const char *)"AT+WSSSID=%s\r", wifis->ssid);
		if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 1000 * portTICK_RATE_MS))
		{
			memset(txbuf, 0, 100);
			sprintf(txbuf, (const char *)"AT+WSKEY=%s,%s,%s\r", wifis->auth, wifis->encry, wifis->key);
			if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 1000* portTICK_RATE_MS))
				statues = My_Pass;
		}
	}

	MyFree(txbuf);
	
	return statues;
}


MyState_TypeDef WifiIsConnectted(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WSLK\r", "+ok", txbuf, 500, 50 / portTICK_RATE_MS))
		{
			if(!((strstr(txbuf, "DisConnected")) || (strstr(txbuf, "RF Off"))))
				statues = My_Pass;
		}
	}
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef RestartWifi(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(50);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+Z\r", "+ok", txbuf, 50, 50 * portTICK_RATE_MS))
		{
			statues = My_Pass;	
		}
	}
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef CheckWifiMID(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(50);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+MID\r", "+ok", txbuf, 50, 50 * portTICK_RATE_MS))
		{
			if(strstr(txbuf, "USR-WIFI232-S"))
				statues = My_Pass;
		}
	}
	MyFree(txbuf);
	
	return statues;
}

unsigned char GetWifiIndicator(void)
{
	char *txbuf = NULL;
	unsigned char ind = 0;								//0代表未连wifi，所以最低信号强度1
	char *s = NULL;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(50);
	if(txbuf)
	{
		s = txbuf;
		if(My_Pass == ComWithWIFI("AT+WSLQ\r", "+ok", txbuf, 50, 50 * portTICK_RATE_MS))
		{
			if(strstr(txbuf, "Disconnected"))
			{
				ind = 0;
			}
			else
			{
				s = strtok(txbuf, ",");
				if(s)
				{
					s = strtok(NULL, ",");
					if(s)
					{
						ind = strtol(s, NULL, 10);
						if(ind == 0)
							ind = 1;
					}
					else
						ind = 1;
				}
				else
					ind = 1;
			}
		}
		else
			ind = 0;
	}
	MyFree(txbuf);
	
	return ind;
}


MyState_TypeDef SetWifiSocketA(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;

	txbuf = MyMalloc(100);
	if(txbuf)
	{
		{
			sprintf(txbuf, (const char *)"AT+NETP=TCP,CLIENT,9602,%d.%d.%d.%d\r", GetGB_NetData()->serverip.ip_1,GetGB_NetData()->serverip.ip_2,GetGB_NetData()->serverip.ip_3,GetGB_NetData()->serverip.ip_4);
		
			if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 500 * portTICK_RATE_MS))
				statues = My_Pass;
				
		}
	}

	MyFree(txbuf);

	return statues;
}

MyState_TypeDef GetWifiSocketAState(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(100);
	if(txbuf)
	{
		sprintf(txbuf, (const char *)"AT+TCPLK\r");
		
		if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 500 * portTICK_RATE_MS))
		{
			if(strstr(txbuf, "on"))
				statues = My_Pass;
		}
	}
	
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef SendDataBySocketA(mynetbuf *netbuf)
{
	char *txbuf = NULL;
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(1000);
	if(txbuf)
	{
/*		sprintf(txbuf, (const char *)"AT+SEND=%d\r", netbuf->datalen);
		
		if(My_Pass == ComWithWIFI(txbuf, ">", txbuf, 100, 500 * portTICK_RATE_MS))
		{
			ComWithWIFI(txbuf, NULL, NULL, 0, 100 / portTICK_RATE_MS);
				statues = My_Pass;
		}*/
		memset(txbuf, 0, 1000);
		memcpy(txbuf, netbuf->data, netbuf->datalen);
		strcat(txbuf, "\r");

		MyFree(netbuf->data);
		
		if(My_Pass == ComWithWIFI(txbuf, NULL, txbuf, 1000, 4000 / portTICK_RATE_MS))
		{
			netbuf->datalen = strlen(txbuf);
			netbuf->data = MyMalloc(netbuf->datalen+10);
			memcpy(netbuf->data, txbuf, netbuf->datalen);
			
			statues = My_Pass;
		}
	}
	
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef RevDataBySocketA(mynetbuf *netbuf)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(2000);
	if(txbuf)
	{
		sprintf(txbuf, (const char *)"AT+RECV=800,1\r");
		
		if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 1000, 1000 / portTICK_RATE_MS))
		{
			netbuf->datalen = strlen(txbuf);
			netbuf->data = MyMalloc(netbuf->datalen+10);
			if(netbuf->data)
			{
				memset(netbuf->data, 0, netbuf->datalen+10);
				memcpy(netbuf->data, txbuf, netbuf->datalen);
				statues = My_Pass;
			}
		}
	}
	
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef CloseSocketA(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(100);
	if(txbuf)
	{
		sprintf(txbuf, (const char *)"AT+NETP=TCP,CLIENT,9602,127.0.0.1\r");
		
		if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 500 * portTICK_RATE_MS))
			statues = My_Pass;
	}
	
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef SetWifiSocketB(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(100);
	if(txbuf)
	{
		/*重启*/
		sprintf(txbuf, (const char *)"AT+SOCKB=TCP,9601,192.168.2.100\r");
		
		if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 500 / portTICK_RATE_MS))
			statues = My_Pass;
	}
	
	MyFree(txbuf);
	
	return statues;
}
MyState_TypeDef GetWifiSocketBState(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(100);
	if(txbuf)
	{
		sprintf(txbuf, (const char *)"AT+TCPLKB\r");
		
		if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 500 * portTICK_RATE_MS))
		{
			if(strstr(txbuf, "on"))
				statues = My_Pass;
		}

	}
	
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef CloseSocketB(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(100);
	if(txbuf)
	{
		sprintf(txbuf, (const char *)"AT+SOCKB=TCP,9601,127.0.0.1\r");
		
		if(My_Pass == ComWithWIFI(txbuf, "+ok", txbuf, 100, 500 * portTICK_RATE_MS))
			statues = My_Pass;

	}
	
	MyFree(txbuf);
	
	return statues;
}

static void AnalyRecvData(char *buf, mynetbuf *netbuf)
{
	
}
