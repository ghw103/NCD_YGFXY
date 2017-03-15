#ifndef __USART4_H__
#define __USART4_H__

#include	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

#define xRxQueue4_Len				6000
#define xTxQueue4_Len				1000

void Usart4_Init(void);
void EnableUsart4TXInterrupt(void);
xQueueHandle GetUsart4RXQueue(void);
xQueueHandle GetUsart4TXQueue(void);

#endif

