/***************************************************************************************************
*FileName:
*Description:
*Author: xsx_kair
*Data:
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"BackDoorData.h"

#include	"MyTools.h"

#include	<string.h>
#include	"stdio.h"
#include 	"stdlib.h"
#include	<math.h>
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
const BackDoorData s_BackDoorData[3]=
{
	//PCT
	{
		.data1 = 0.2,
		.data2 = {0.3, 2, 5, 10, 50, 100},
		.data3 = {0.5, 10, 50, 50},
		.data4 = {0.5, 10},
		.data5 = {0.5, 10}
	},
	//CRP
	{
		.data1 = 0.5,
		.data2 = {0.5, 3, 10, 20, 50, 200},
		.data3 = {0.5, 3, 20, 50},
		.data4 = {3, 10},
		.data5 = {3, 10}
	},
	//D_Dimer
	{
		.data1 = 0.2,
		.data2 = {0.2, 0.5, 1, 2, 5, 10},
		.data3 = {0.5, 1, 5, 5},
		.data4 = {0.5, 10},
		.data5 = {0.5, 10}
	},
};

static unsigned char S_ItemIndex = 0;									//测试项目
static unsigned char S_CategoryIndex = 0;								//当前检测功能
static unsigned char S_TestIndex = 0;									//当前检测功能的次数
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

void SetS_TestItemName(void * name)
{
	if(CheckStrIsSame("PCT", name, strlen(name)))
		SetS_ItemIndex(0);
	else if(CheckStrIsSame("CRP", name, strlen(name)))
		SetS_ItemIndex(1);
	else if(CheckStrIsSame("D-Dimer", name, strlen(name)))
		SetS_ItemIndex(2);
}

void SetS_ItemIndex(unsigned char index)
{
	S_ItemIndex = index;
}

unsigned char GetS_ItemIndex(void)
{
	return S_ItemIndex;
}

void SetS_CategoryIndex(unsigned char index)
{
	S_CategoryIndex = index;
}

unsigned char GetS_CategoryIndex(void)
{
	return S_CategoryIndex;
}

void SetS_TestIndex(unsigned char index)
{
	S_TestIndex = index;
}

unsigned char GetS_TestIndex(void)
{
	return S_TestIndex;
}


double GetCurrentData(void)
{
	double a,b,c;
	double temp1;
	
	switch(S_CategoryIndex)
	{
		case 0:
			temp1 = s_BackDoorData[S_ItemIndex].data1;
				break;
		case 1:
			temp1 = s_BackDoorData[S_ItemIndex].data2[S_TestIndex / data2_t];
				break;
		case 2:
			temp1 = s_BackDoorData[S_ItemIndex].data3[S_TestIndex / data3_t];
				break;
		case 3:
			temp1 = s_BackDoorData[S_ItemIndex].data4[S_TestIndex / data4_t];
				break;
		case 4:
			temp1 = s_BackDoorData[S_ItemIndex].data5[S_TestIndex / data5_t];
				break;
		default:
			temp1 = s_BackDoorData[S_ItemIndex].data1;
				break;
	}
	
	a = rand()%101;
	a *= 0.01;
	a *= temp1*0.075;
	
	b = pow((-1), ((rand()%2)+1));
	
	a *= b;
	
	return (temp1 + a);
}
/****************************************end of file************************************************/
