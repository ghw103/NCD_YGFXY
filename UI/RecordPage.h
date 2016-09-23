#ifndef __RECORD_P_H__
#define __RECORD_P_H__

#include	"MyTest_Data.h"



typedef struct Record_tag {
	unsigned char longpresscount;						//长按时间
	TestData testdata[DataNumInPage];					//测试数据
	unsigned char selectindex;							//选中的索引
	unsigned char pageindex;							//页面索引
	
	TestDataSaveHead saveddatahead;						//数据保存索引
	
	unsigned int maxpagenum;

	char buf[300];										//临时缓冲区
	unsigned int tempvalue1;
	unsigned int tempvalue2;
	TestData *tempdata;
	unsigned short lcdinput[100];
}RecordPageBuffer;

unsigned char DspRecordPage(void *  parm);

#endif

