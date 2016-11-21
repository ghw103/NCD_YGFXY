#ifndef __USART2_H__
#define __USART2_H__

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#define xRxQueue2_Len				300
#define xTxQueue2_Len				20

void Usart2_Init(void);
void EnableUsart2TXInterrupt(void);
xQueueHandle GetUsart2RXQueue(void);
xQueueHandle GetUsart2TXQueue(void);
xSemaphoreHandle GetUsart2RXMutex(void);
xSemaphoreHandle GetUsart2TXMutex(void);

#endif

