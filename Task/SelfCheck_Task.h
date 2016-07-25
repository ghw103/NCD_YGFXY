#ifndef __SELFC_T_H__
#define __SELFC_T_H__

#include 	"FreeRTOS.h"
#include 	"task.h"

void StartSelfCheckTask(void);
void SelfCheckTaskRun(void);
portBASE_TYPE GetSelfTestTaskState(unsigned char * receivedchar , portTickType xBlockTime);

#endif

