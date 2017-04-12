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
static xSemaphoreHandle xWifiMutex = NULL;									//WIFI互斥量

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
static MyState_TypeDef ComWithWIFI(char * cmd, const char *strcmp, char *buf, unsigned short buflen, portTickType xBlockTime);
static void ProgressWifiListData(WIFI_Def *wifis, char *buf);
static MyState_TypeDef SetWifiServerInfo(void);
static MyState_TypeDef SetWifiDefaultWorkMode(void);
static MyState_TypeDef SetWifiWorkInSTAMode(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: takeWifiMutex, giveWifixMutex
*Description: 
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年3月7日09:41:10
***************************************************************************************************/
MyState_TypeDef takeWifiMutex(portTickType xBlockTime)
{
	if(pdPASS == xSemaphoreTake(xWifiMutex, xBlockTime))
		return My_Pass;
	else
		return My_Fail;
}
void giveWifixMutex(void)
{
	xSemaphoreGive(xWifiMutex);
}

/***************************************************************************************************
*FunctionName: ComWithWIFI与wifi模块通信
*Description: 
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年3月7日09:41:31
***************************************************************************************************/
static MyState_TypeDef ComWithWIFI(char * cmd, const char *strcmp, char *buf, unsigned short buflen, portTickType xBlockTime)
{
	MyState_TypeDef statues = My_Pass;
	unsigned char errorCnt = 0;
	
	//清空队列数据
	while(pdPASS == ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, buf, buflen, NULL, 1, 10 / portTICK_RATE_MS, 1 / portTICK_RATE_MS));
	
	for(errorCnt = 0; errorCnt < 3; errorCnt++)
	{
		if(pdPASS == SendDataToQueue(GetUsart4TXQueue(), NULL, cmd, strlen(cmd), 1, 500 * portTICK_RATE_MS, 0, EnableUsart4TXInterrupt))
		{
			if(buf)
			{
				memset(buf, 0, buflen);
					
				ReceiveDataFromQueue(GetUsart4RXQueue(), NULL, buf, buflen, NULL, 1, xBlockTime, 0);

				if(strcmp)
				{
					if(strstr(buf, strcmp) == NULL)
						statues = My_Fail;
				}
			}
		}
		else
			statues = My_Fail;
		
		if(statues == My_Pass)
			break;
	}

	return statues;
}


MyState_TypeDef WIFIInit(void)
{
	MyState_TypeDef statues = My_Fail;
	
	vSemaphoreCreateBinary(xWifiMutex);
	
	SetWifiWorkInAT(AT_Mode);

	SetWifiServerInfo();

	SetWifiDefaultWorkMode();
	
	SetWifiWorkInSTAMode();
	
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
	
	if(mode == GetWifiWorkMode())
		return My_Pass;
	
	txbuf = MyMalloc(500);
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

WIFI_WorkMode_DefType GetWifiWorkMode(void)
{
	char *txbuf = NULL;
	WIFI_WorkMode_DefType mode = None;
	
	txbuf = MyMalloc(500);
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

static MyState_TypeDef SetWifiDefaultWorkMode(void)
{
	char *txbuf = NULL;
	MyState_TypeDef statues = My_Fail;
	
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+TMODE=throughput\r", "+ok", txbuf, 50, 500 * portTICK_RATE_MS))
			statues = My_Pass;
	}
	
	MyFree(txbuf);
	
	return statues;
}

static MyState_TypeDef SetWifiWorkInSTAMode(void)
{
	char *txbuf = NULL;
	MyState_TypeDef statues = My_Fail;
	
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WMODE=STA\r", "+ok", txbuf, 50, 500 * portTICK_RATE_MS))
			statues = My_Pass;
	}
	
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef ScanApList(WIFI_Def *wifis)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;

	txbuf = MyMalloc(1000);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WSCAN\r", NULL, txbuf, 900, 5000 / portTICK_RATE_MS))
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
			memset(wifis, 0, sizeof(WIFI_Def));
			for(j=0; j<5; j++)
			{
				if(j == 0)
					tempbuf2 = strtok(tempbuf3, ",");
				else
					tempbuf2 = strtok(NULL, ",");
					
				if(tempbuf2)
				{
					if(j == 1)
						memcpy(wifis->ssid, tempbuf2, strlen(tempbuf2)) ;
					else if(j == 3)
					{
						if(strstr(tempbuf2, "OPEN"))
							memcpy(wifis->auth, "OPEN", 4);
						else if(strstr(tempbuf2, "SHARED"))
							memcpy(wifis->auth, "SHARED", 6);
						else if(strstr(tempbuf2, "WPAPSK"))
							memcpy(wifis->auth, "WPAPSK", 6);
						else if(strstr(tempbuf2, "WPA2PSK"))
							memcpy(wifis->auth, "WPA2PSK", 7);
							
						if(strstr(tempbuf2, "NONE"))
							memcpy(wifis->encry, "NONE", 4);
						else if(strstr(tempbuf2, "WEP-H"))
							memcpy(wifis->encry, "WEP-H", 5);
						else if(strstr(tempbuf2, "WEP-A"))
							memcpy(wifis->encry, "WEP-A", 5);
						else if(strstr(tempbuf2, "TKIP"))
							memcpy(wifis->encry, "TKIP", 4);
						else if(strstr(tempbuf2, "AES"))
							memcpy(wifis->encry, "AES", 3);
					}
					else if(j == 4)
					{
						wifis->indicator = strtol(tempbuf2 , NULL , 10);
						
						if(wifis->indicator > 10)
							wifis++;
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
	unsigned char i=0;
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(500);
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
			{
				//重启
				RestartWifi();
				
				vTaskDelay(8000 / portTICK_RATE_MS);
				
				for(i=0; i<3; i++)
				{
					if(My_Pass == SetWifiWorkInAT(AT_Mode))
					{
						memset(txbuf, 0, 100);
						if(My_Pass == WifiIsConnectted(txbuf))
						{
							if(strstr(txbuf, wifis->ssid) != NULL)
								statues = My_Pass;
						}
						
						break;
					}
					
					vTaskDelay(1000 / portTICK_RATE_MS);
				}
			}
		}
	}

	MyFree(txbuf);
	
	return statues;
}


MyState_TypeDef GetWifiStaIP(IP_Def * ip)
{
	char *txbuf = NULL;
	char * tempp1 = NULL;
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WANN\r", "+ok", txbuf, 100, 100 / portTICK_RATE_MS))
		{
			tempp1 = strtok(txbuf, ",");
			if(tempp1)
			{
				tempp1 = strtok(NULL, ",");
				
				if(tempp1)
				{
					ip->ip_1 = strtol(tempp1, &tempp1, 10);
					ip->ip_2 = strtol(tempp1+1, &tempp1, 10);
					ip->ip_3 = strtol(tempp1+1, &tempp1, 10);
					ip->ip_4 = strtol(tempp1+1, &tempp1, 10);
				}
			}
			
			statues = My_Pass;
		}
	}
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef GetWifiStaMac(char *mac)
{
	char *txbuf = NULL;
	char * tempp1 = NULL;
	
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WSMAC\r", "+ok", txbuf, 100, 100 / portTICK_RATE_MS))
		{
			tempp1 = strtok(txbuf, "=");
			if(tempp1)
			{
				tempp1 = strtok(NULL, "=");
				
				if(tempp1)
					memcpy(mac, tempp1, 12);
			}
			
			statues = My_Pass;
		}
	}
	MyFree(txbuf);
	
	return statues;
}

MyState_TypeDef WifiIsConnectted(char * ssid)
{
	char *txbuf = NULL; 
	char * tempp1 = NULL;
	char * tempp2 = NULL;
	unsigned char len = 0;
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+WSLK\r", "+ok", txbuf, 100, 50 / portTICK_RATE_MS))
		{
			if(!((strstr(txbuf, "Disconnected")) || (strstr(txbuf, "RF Off"))))
			{
				//总长度
				len = strlen(txbuf);
				tempp1 = strtok(txbuf, "=");
				if(tempp1)
				{
					//减去头长度
					len -= strlen(tempp1);
					//减去尾和mac长度
					len -= 22;
					
					tempp1 = strtok(NULL, "=");
				
					if(tempp1)
					{
						tempp2 = strtok(tempp1, "(");
						
						if(tempp2)
							memcpy(ssid, tempp2, strlen(tempp2));
						
					}
				}
				statues = My_Pass;
			}
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
	txbuf = MyMalloc(500);
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
	txbuf = MyMalloc(500);
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
	txbuf = MyMalloc(500);
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

static MyState_TypeDef SetWifiServerInfo(void)
{
	char *txbuf = NULL; 
	MyState_TypeDef statues = My_Fail;
	
	/*发送数据缓冲区*/
	txbuf = MyMalloc(500);
	if(txbuf)
	{
		if(My_Pass == ComWithWIFI("AT+NETP=TCP,CLIENT,8080,116.62.108.201\r\n", "+ok", txbuf, 100, 1000 * portTICK_RATE_MS))
			statues = My_Pass;
	}
	MyFree(txbuf);
	
	return statues;
}
