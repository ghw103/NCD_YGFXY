/****************************************file start****************************************************/
#ifndef	MOTOR_F_H
#define MOTOR_F_H

#include 	"FreeRTOS.h"
#include	"Define.h"

void InitMotorData(void);
void MotorMoveTo(unsigned int location, unsigned char mode);
void StopMotor(void);

#endif

/****************************************end of file************************************************/
