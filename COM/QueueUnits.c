/***************************************************************************************************
*FileName:QueueUnits
*Description:¶ÓÁÐ²Ù×÷¹¤¾ßÀà
*Author:xsx
*Data:2016Äê4ÔÂ22ÈÕ14:28:03
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Í·ÎÄ¼þ***************************************************/
/***************************************************************************************************/
#include	"QueueUnits.h"


/***************************************************************************************************/
/**************************************¾Ö²¿±äÁ¿ÉùÃ÷*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************¾Ö²¿º¯ÊýÉùÃ÷*************************************************/
/***************************************************************************************************/
static unsigned char WaittingForMutex(xSemaphoreHandle mutex, portTickType xBlockTime);
static void GivexMutex(xSemaphoreHandle mutex);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************ÕýÎÄ********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static unsigned char WaittingForMutex(xSemaphoreHandle mutex, portTickType xBlockTime)
{
	return xSemaphoreTake(mutex, xBlockTime);
}
static void GivexMutex(xSemaphoreHandle mutex)
{
	xSemaphoreGive(mutex);
}

/***************************************************************************************************
*FunctionName£ºReceiveCharFromQueue
*Description£º½ÓÊÕÒ»¸ö×Ö½Úµ½¶ÓÁÐ
*Input£ºqueue -- Ä¿±ê¶ÓÁÐ
*		mutex -- ´Ë¶ÓÁÐµÄ»¥³âÁ¿£¬¿ÉÎªnull
*		sendstr -- ½ÓÊÕÊý¾ÝµÄ´æ·ÅµØÖ·
*		xBlockTime -- ×èÈûÊ±¼ä
*Output£ºNone
*Author£ºxsx
*Data£º2016Äê4ÔÂ22ÈÕ15:36:44
***************************************************************************************************/
unsigned char ReceiveCharFromQueue(xQueueHandle queue, xSemaphoreHandle mutex, void * receivedchar , portTickType xBlockTime)
{
	unsigned char * p = (unsigned char *)receivedchar;
	unsigned char statues = pdFAIL;
	
	if(queue == NULL)
		return pdFAIL;
	
	if(mutex != NULL)
	{
		if(pdFAIL == WaittingForMutex(mutex, xBlockTime))
			return pdFAIL;
	}

	statues = xQueueReceive( queue, p, xBlockTime );
	
	if(mutex != NULL)
		GivexMutex(mutex);
	
	return statues;
}

/***************************************************************************************************
*FunctionName£ºSendCharToQueue
*Description£º·¢ËÍÒ»¸ö×Ö½Úµ½¶ÓÁÐ
*Input£ºqueue -- Ä¿±ê¶ÓÁÐ
*		mutex -- ´Ë¶ÓÁÐµÄ»¥³âÁ¿£¬¿ÉÎªnull
*		sendstr -- ·¢ËÍµÄÊý¾Ý
*		xBlockTime -- ×èÈûÊ±¼ä
*Output£ºNone
*Author£ºxsx
*Data£º2016Äê4ÔÂ22ÈÕ15:36:15
***************************************************************************************************/
unsigned char SendCharToQueue(xQueueHandle queue, xSemaphoreHandle mutex, unsigned char receivedchar , portTickType xBlockTime,
	void (*fun)(void))
{
	unsigned char statues = pdFAIL;
	
	if(queue == NULL)
		return pdFAIL;
	
	if(mutex != NULL)
	{
		if(pdFAIL == WaittingForMutex(mutex, xBlockTime))
			return pdFAIL;
	}

	statues = xQueueSend( queue, &receivedchar, xBlockTime );
	
	if(mutex != NULL)
		GivexMutex(mutex);
	
	if(fun != NULL)
		fun();
	
	return statues;
} 

/***************************************************************************************************
*FunctionName£ºReceiveStrFromQueue
*Description£º´Ó¶ÓÁÐ½ÓÊÕÊý¾Ý
*Input£ºqueue -- Ä¿±ê¶ÓÁÐ
*		mutex -- ´Ë¶ÓÁÐµÄ»¥³âÁ¿£¬¿ÉÎªnull
*		sendstr -- ´æ·Å½ÓÊÕÊý¾ÝµÄµØÖ·
*		len -- ½ÓÊÕµÄÊý¾Ý³¤¶È£¨×Ö½Ú³¤£©
*		xBlockTime -- ×èÈûÊ±¼ä
*Output£ºNone
*Author£ºxsx
*Data£º2016Äê4ÔÂ22ÈÕ15:35:40
***************************************************************************************************/
unsigned char ReceiveStrFromQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *receivedstr , unsigned short len , 
	portTickType xBlockTime)
{
	unsigned short i=0;
	unsigned char *pdata = (unsigned char *)receivedstr;
	unsigned char statues = pdFAIL;
	
	if(queue == NULL)
		return pdFAIL;
	
	if(mutex != NULL)
	{
		if(pdFAIL == WaittingForMutex(mutex, xBlockTime))
			return pdFAIL;
	}
	
	for(i=0; i<len; i++)
	{
		if(pdPASS == xQueueReceive(queue, pdata , xBlockTime))
		{
			pdata++;
			statues = pdPASS;
		}
		else
		{
			statues = pdFAIL;
			break;
		}
	}
	
	if(mutex != NULL)
		GivexMutex(mutex);
	
	return statues;
}
/***************************************************************************************************
*FunctionName£ºSendStrToQueue
*Description£º·¢ËÍÊý¾Ýµ½¶ÓÁÐ
*Input£ºqueue -- Ä¿±ê¶ÓÁÐ
*		mutex -- ´Ë¶ÓÁÐµÄ»¥³âÁ¿£¬¿ÉÎªnull
*		sendstr -- ·¢ËÍµÄÊý¾ÝµØÖ·
*		len -- ·¢ËÍµÄÊý¾Ý³¤¶È£¨×Ö½Ú³¤£©
*		xBlockTime -- ×èÈûÊ±¼ä
*Output£ºNone
*Author£ºxsx
*Data£º2016Äê4ÔÂ22ÈÕ15:33:38
***************************************************************************************************/
unsigned char SendStrToQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *sendstr , unsigned short len ,  
	portTickType xBlockTime, void (*fun)(void))
{
	unsigned short i=0;
	unsigned char *pdata = (unsigned char *)sendstr;
	unsigned char statues = pdFAIL;
	
	if(queue == NULL)
		return pdFAIL;
	
	if(mutex != NULL)
	{
		if(pdFAIL == WaittingForMutex(mutex, xBlockTime))
			return pdFAIL;
	}

	for(i=0; i<len; i++)
	{
		if(pdPASS == xQueueSend(queue, pdata , xBlockTime))
		{
			pdata++;
			statues = pdPASS;
		}
		else
		{
			statues = pdFAIL;
			break;
		}
	}
	
	if(mutex != NULL)
		GivexMutex(mutex);
	
	if(fun != NULL)
		fun();
	
	return statues;
}


/***************************************************************************************************
*FunctionName£ºReceiveDataFromQueue
*Description£º´Ó¶ÓÁÐÖÐ¶ÁÈ¡ÓÐÏÞ³¤¶ÈµÄÊý¾Ý
*Input£ºqueue -- Ä¿±ê¶ÓÁÐ
*		mutex -- ´Ë¶ÓÁÐµÄ»¥³âÁ¿£¬¿ÉÎªnull
*		receivedstr -- ´æ·Å½ÓÊÕÊý¾ÝµÄµØÖ·
*		len -- ½ÓÊÕµÄÊý¾Ý³¤¶È(³¤¶ÈÎª´Ë¶ÓÁÐµ¥ÔªÊý¾Ý´óÐ¡µÄ¸öÊý£¬ÓëÊý¾Ý½ÓÊÕÊý¾ÝµÄ×Ö½Ú³¤¶ÈÎÞ¹Ø)
*		xBlockTime -- ×èÈûÊ±¼ä
*Output£ºNone
*Author£ºxsx
*Data£º2016Äê4ÔÂ22ÈÕ15:35:40
***************************************************************************************************/
unsigned char ReceiveDataFromQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *receivedstr , unsigned short len , 
	portTickType xBlockTime)
{
	unsigned short i=0;
	unsigned char *pdata = (unsigned char *)receivedstr;
	unsigned char statues = pdFAIL;
	
	Queue_t * const pxQueue = ( Queue_t * ) queue;
	
	if(queue == NULL)
		return pdFAIL;
	
	if(mutex != NULL)
	{
		if(pdFAIL == WaittingForMutex(mutex, xBlockTime))
			return pdFAIL;
	}
	
	for(i=0; i<len; i++)
	{
		if(pdPASS == xQueueReceive(queue, pdata , xBlockTime))
		{
			pdata += pxQueue->uxItemSize;
			statues = pdPASS;
		}
		else
		{
			statues = pdFAIL;
			break;
		}
	}
	
	if(mutex != NULL)
		GivexMutex(mutex);
	
	return statues;
}
/***************************************************************************************************
*FunctionName£ºSendDataToQueue
*Description£º·¢ËÍÊý¾Ýµ½¶ÓÁÐ
*Input£ºqueue -- Ä¿±ê¶ÓÁÐ
*		mutex -- ´Ë¶ÓÁÐµÄ»¥³âÁ¿£¬¿ÉÎªnull
*		sendstr -- ·¢ËÍµÄÊý¾ÝµØÖ·
*		len -- ½ÓÊÕµÄÊý¾Ý³¤¶È(³¤¶ÈÎª´Ë¶ÓÁÐµ¥ÔªÊý¾Ý´óÐ¡µÄ¸öÊý£¬ÓëÊý¾Ý½ÓÊÕÊý¾ÝµÄ×Ö½Ú³¤¶ÈÎ
*		xBlockTime -- ×èÈûÊ±¼ä
*Output£ºNone
*Author£ºxsx
*Data£º2016Äê4ÔÂ22ÈÕ15:33:38
***************************************************************************************************/
unsigned char SendDataToQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *sendstr , unsigned short len ,  
	portTickType xBlockTime, void (*fun)(void))
{
	unsigned short i=0;
	unsigned char *pdata = (unsigned char *)sendstr;
	unsigned char statues = pdFAIL;
	Queue_t * const pxQueue = ( Queue_t * ) queue;
	
	if(queue == NULL)
		return pdFAIL;
	
	if(mutex != NULL)
	{
		if(pdFAIL == WaittingForMutex(mutex, xBlockTime))
			return pdFAIL;
	}

	for(i=0; i<len; i++)
	{
		if(pdPASS == xQueueSend(queue, pdata , xBlockTime))
		{
			pdata += pxQueue->uxItemSize;
			statues = pdPASS;
		}
		else
		{
			statues = pdFAIL;
			break;
		}
	}
	
	if(mutex != NULL)
		GivexMutex(mutex);
	
	if(fun != NULL)
		fun();
	
	return statues;
}

