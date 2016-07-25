#ifndef __RECORD_P_H__
#define __RECORD_P_H__

#include	"MyTest_Data.h"



typedef struct Record_tag {
	unsigned char selectindex;							//选中的索引
	unsigned char pageindex;							//页面索引
	char filename[30];									//当前选择查看的文件日期
	MyTime_Def time;									//
	unsigned char longpresscount;						//长按时间
	TestData testdata[DataNumInPage];					//测试数据
	char buf[300];										//临时缓冲区
	unsigned short tempvalue1;
	unsigned short tempvalue2;
	TestData *tempdata;
}RecordPageBuffer;

unsigned char DspRecordPage(void *  parm);

#endif

