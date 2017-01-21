#ifndef __PRERCARD_P_H__
#define __PRERCARD_P_H__

#include	"Define.h"
#include	"MyTest_Data.h"
#include	"Test_Fun.h"
#include	"UI_Data.h"

typedef struct PreReadPageBuffer_tag {
	ItemData * currenttestdata;
	ResultState cardpretestresult;
	unsigned char preTestErrorCount;				//如果是未加样，则需再次测试确保真的没加样，当前变量表示重测次数
	ScanCodeResult scancode;
	Timer timer2;
	TestData temptestdata;							//临时测试数据缓冲区
	QRCode temperweima;						//临时二维码数据缓冲区
	char buf[100];
	unsigned short lcdinput[100];
}PreReadPageBuffer;

MyState_TypeDef createPreReadCardActivity(Activity * thizActivity, Intent * pram);

#endif

