#ifndef __ADJUST_P_H__
#define __ADJUST_P_H__

#include	"Timer_Data.h"
#include	"MyTest_Data.h"
#include	"SystemSet_Data.h"
#include	"Test_Fun.h"
#include	"Define.h"
#include	"UI_Data.h"

typedef struct AdjustPageBuffer_tag {

	ItemData itemData;
	ScanCodeResult scancode;
	ResultState cardTestResult;
	unsigned char testErrorCount;
	AdjustData adjustData;								//������
	SystemSetData systemSetData;						//ϵͳ�������������ڱ���ʧ�ܻع�
	float targetresult;
	bool isAdjjing;										//�Ƿ�����У׼��
	char buf[100];
	unsigned short lcdinput[20];
	double adjResult;									//У׼���
}AdjustPageBuffer;

MyState_TypeDef createAdjActivity(Activity * thizActivity, Intent * pram);

#endif
