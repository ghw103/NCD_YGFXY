#ifndef _CARDS_S_D_H__
#define _CARDS_S_D_H__


#include	"Define.h"

typedef enum
{ 
	NoCard = 0,								//ÎÞ¿¨
	CardIN = 1								//ÓÐ¿¨
}CardState_Def;


CardState_Def GetCardState(void);
void SetCardState(CardState_Def value);

#endif

