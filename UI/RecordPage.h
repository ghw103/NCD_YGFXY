#ifndef __RECORD_P_H__
#define __RECORD_P_H__

#include	"MyTest_Data.h"



typedef struct Record_tag {
	unsigned char longpresscount;						//长按时间
	TestData testdata[DataNumInPage];					//测试数据
	unsigned char selectindex;							//选中的索引
	unsigned char pageindex;							//页面索引
	TestDateInfo_Def testdatainfo[TestDataDateRepeatNum];					//数据保存的日期管理信息
	unsigned short maxdatanum;
	unsigned short maxpagenum;
	MyTime_Def datatime;									//当前选择查看的日期
	char buf[300];										//临时缓冲区
	unsigned int tempvalue1;
	unsigned int tempvalue2;
	TestData *tempdata;
	unsigned short lcdinput[100];
}RecordPageBuffer;

unsigned char DspRecordPage(void *  parm);

#endif

