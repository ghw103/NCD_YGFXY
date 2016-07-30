#ifndef _QUEUE_U_H__
#define _QUEUE_U_H__

#include 	"FreeRTOS.h"
#include 	"queue.h"
#include	"semphr.h"

unsigned char ReceiveCharFromQueue(xQueueHandle queue, xSemaphoreHandle mutex, void * receivedchar , portTickType xBlockTime);
unsigned char SendCharToQueue(xQueueHandle queue, xSemaphoreHandle mutex, unsigned char receivedchar , portTickType xBlockTime,
	void (*fun)(void));
unsigned char ReceiveStrFromQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *receivedstr , unsigned short len , 
	portTickType xBlockTime);
unsigned char SendStrToQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *sendstr , unsigned short len ,  
	portTickType xBlockTime, void (*fun)(void));

unsigned char ReceiveDataFromQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *receivedstr , unsigned short len ,
	unsigned short itemsize, portTickType xBlockTime);
unsigned char SendDataToQueue(xQueueHandle queue, xSemaphoreHandle mutex, void *sendstr , unsigned short len ,  
	unsigned short itemsize, portTickType xBlockTime, void (*fun)(void));

#endif

