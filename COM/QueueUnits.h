#ifndef _QUEUE_U_H__
#define _QUEUE_U_H__

#include 	"FreeRTOS.h"
#include 	"queue.h"
#include	"semphr.h"

unsigned char WaittingForMutex(xSemaphoreHandle mutex, portTickType xBlockTime);
void GivexMutex(xSemaphoreHandle mutex);
unsigned char ReceiveDataFromQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *receivedstr , unsigned short len ,
	unsigned short * readSize, unsigned short itemsize, portTickType queueBlockTime, portTickType mutexBlockTime);
unsigned char SendDataToQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *sendstr , unsigned short len ,  
	unsigned short itemsize, portTickType queueBlockTime, portTickType mutexBlockTime, void (*fun)(void));

#endif

