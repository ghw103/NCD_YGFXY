#ifndef __SUSER_P_H__
#define __SUSER_P_H__

#include	"Define.h"
#include	"MyTest_Data.h"
#include	"Timer_Data.h"

typedef struct UserPage_tag {
	unsigned char selectindex;										//选中的索引
	unsigned char pageindex;										//页面索引
	User_Type user[MaxUserNum];										//选中的人
	User_Type * tempuser;
	ItemData * currenttestdata;										//当前测试数据
	Timer timer;													//页面超时时间
	unsigned short lcdinput[100];									//lcd输入解析
}UserPageBuffer;

unsigned char DspSelectUserPage(void *  parm);

#endif

