#ifndef __USART3_H__
#define __USART3_H__

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#define xRxQueue3_Len				10
#define xTxQueue3_Len				100

void Usart3_Init(void);
void EnableUsart3TXInterrupt(void);
xQueueHandle GetUsart3RXQueue(void);
xQueueHandle GetUsart3TXQueue(void);
xSemaphoreHandle GetUsart3Mutex(void);
	
#endif

