#ifndef _QUEUE_U_H__
#define _QUEUE_U_H__

#include 	"FreeRTOS.h"
#include 	"queue.h"
#include	"semphr.h"


unsigned char ReceiveDataFromQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *receivedstr , unsigned short len ,
	unsigned short itemsize, portTickType xBlockTime);
unsigned char SendDataToQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *sendstr , unsigned short len ,  
	unsigned short itemsize, portTickType xBlockTime, void (*fun)(void));

#endif

