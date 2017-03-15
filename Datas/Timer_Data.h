#ifndef _TIMER_S_D_H__
#define _TIMER_S_D_H__

#include	"Define.h"

void ResetGBClockTime(void);
void timer_set(Timer *t, unsigned int interval);
void timer_reset(Timer *t);
void timer_stop(Timer *t);
void timer_restart(Timer *t);
TimerState_Def timer_expired(Timer *t);
unsigned int timer_surplus(Timer *t);
unsigned int timer_used(Timer *t);
unsigned int timer_Count(Timer * t);
bool timerIsStartted(Timer * t);
#endif
