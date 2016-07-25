#ifndef _CARDS_S_D_H__
#define _CARDS_S_D_H__


#include	"Define.h"
#include 	"FreeRTOS.h"

typedef enum
{ 
	NoCard = 0,								//ÎÞ¿¨
	CardIN = 1								//ÓÐ¿¨
}CardState_Def;

void ClearCardStatuesQueue(void);
unsigned char GetCardStatuesFromQueue(CardState_Def * receivedchar , portTickType xBlockTime);
unsigned char SendCardStatuesToQueue(CardState_Def *txchar , portTickType xBlockTime);
CardState_Def GetCardState(void);
void SetCardState(CardState_Def value);

#endif

