#ifndef __USART1_H__
#define __USART1_H__

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#define xRxQueue1_Len				50
#define xTxQueue1_Len				50

void Usart1_Init(void);
void EnableUsart1TXInterrupt(void);
xQueueHandle GetUsart1RXQueue(void);
xQueueHandle GetUsart1TXQueue(void);
xSemaphoreHandle GetUsart1RXMutex(void);
xSemaphoreHandle GetUsart1TXMutex(void);

#endif

