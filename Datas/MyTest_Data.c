/***************************************************************************************************
*FileName : TestLinks_Data
*Description: ≤‚ ‘ ˝æ›¡¥±Ì
*Author:xsx
*Data: 2016ƒÍ5‘¬14»’17:14:02
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Õ∑Œƒº˛***************************************************/
/***************************************************************************************************/
#include	"MyTest_Data.h"

#include	"MyMem.h"
#include	"string.h"
#include 	"FreeRTOS.h"
#include 	"task.h"
/***************************************************************************************************/
/**************************************Â±ÄÈÉ®ÂèòÈáèÂ£∞Êòé*************************************************/
/***************************************************************************************************/
static ItemData * (itemdata[PaiDuiWeiNum]) = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static ItemData * CurrentItemData = NULL;
/***************************************************************************************************/
/**************************************Â±ÄÈÉ®ÂáΩÊï∞Â£∞Êòé*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************Ê≠£Êñá********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

unsigned char GetUsableLocation(void)
{
	unsigned char i=0;

	for(i=1; i<PaiDuiWeiNum; i++)
	{
		if(itemdata[i] == NULL)
			return i;
	}
	
	return 0xff;
}

unsigned char CreateANewTest(unsigned char index)
{
	unsigned char templ = GetUsableLocation();
	
	/*”–ø’œ–Œª÷√*/
	if(0xff != templ)
	{
		/*»Áπ˚≥£πÊ≤‚ ‘£¨«“Œﬁø®‘⁄≈≈∂”*/
		if(index == 0)
		{
			if(templ == 1)
				templ = 0;
			else
				return 1;
		}
		
		if(itemdata[templ] == NULL)
		{
			//”–≈≈∂”µƒø®º¥Ω´≤‚ ‘
			if((GetMinWaitTime() > 60) || (NULL != CurrentItemData))
			{
				itemdata[templ] = (ItemData *)MyMalloc(sizeof(ItemData));
				if(itemdata[templ])
				{
					memset(itemdata[templ], 0, sizeof(ItemData));
						
					itemdata[templ]->testlocation = templ;
						
					SetCurrentTestItem(itemdata[templ]);
						
					return 0;
				}
				else
					return 2;
			}
			else
				return 3;
		}
	}

	return 1;
}




ItemData * GetTestItemByIndex(unsigned char index)
{
	return itemdata[index];
}


unsigned short GetMinWaitTime(void)
{
	unsigned char index = 0;
	unsigned short min = 0xffff;
	unsigned short temp = 0;
	
	for(index = 1; index < PaiDuiWeiNum; index++)
	{
		if((itemdata[index])&&((itemdata[index]->statues == statues3) || ((itemdata[index]->statues == statues4))))
		{
			temp = timer_surplus(&(itemdata[index]->timer));
			if(temp < min)
				min = temp;
		}
	}
	
	return min;
}



void SetCurrentTestItem(ItemData * s_itemdata)
{
	CurrentItemData = s_itemdata;
}

ItemData * GetCurrentTestItem(void)
{
	return CurrentItemData;
}

/*…æ≥˝µ±«∞≤‚ ‘*/
MyState_TypeDef DeleteCurrentTest(void)
{
	ItemData * temp = GetCurrentTestItem();
	unsigned char i=0;
	if(temp)
	{
		MyFree(temp);
		memset(temp, 0, sizeof(ItemData));
		
		for(i=0; i<PaiDuiWeiNum; i++)
		{
			if(itemdata[i] == temp)
			{
				itemdata[i] = NULL;
				SetCurrentTestItem(itemdata[i]);
			}
		}
	}
	return My_Pass;
}

