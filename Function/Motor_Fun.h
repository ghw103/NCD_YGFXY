#ifndef __MOTOR_F_H__
#define __MOTOR_F_H__

#include 	"FreeRTOS.h"
#include	"Define.h"

void InitMotorData(void);
void MotorMoveTo(unsigned int location, unsigned char mode);
void StopMotor(void);
#endif

