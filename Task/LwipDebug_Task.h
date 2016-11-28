#ifndef _LWIP_U_S_T_H__
#define _LWIP_U_S_T_H__

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"
#include	"semphr.h"

void StartvLwipDebugTask(void);
xQueueHandle GetLwipDebugQueue(void);

#endif




