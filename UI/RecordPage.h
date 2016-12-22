#ifndef __RECORD_P_H__
#define __RECORD_P_H__

#include	"MyTest_Data.h"
#include	"TestDataDao.h"
#include	"UI_Data.h"

typedef struct Record_tag {

	unsigned char selectindex;							//选中的索引
	unsigned int pageindex;							//页面索引
	unsigned int maxpagenum;
	
	ReadTestDataPackage readTestDataPackage;
	
	char buf[300];										//临时缓冲区
	unsigned int tempvalue1;
	unsigned int tempvalue2;
	TestData *tempdata;
	unsigned short lcdinput[100];
}RecordPageBuffer;

MyState_TypeDef createRecordActivity(Activity * thizActivity, Intent * pram);

#endif

