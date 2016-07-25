#ifndef __SUSER_P_H__
#define __SUSER_P_H__

#include	"Define.h"
#include	"MyTest_Data.h"
#include	"Timer_Data.h"

#define		TextAddr1	0x1c10
#define		TextAddr2	0x1610
#define		TextAddr3	0x1620
#define		TextAddr4	0x1630
#define		TextAddr5	0x1640
#define		TextAddr6	0x1650


typedef struct UserPage_tag {
	unsigned char selectindex;										//选中的索引
	unsigned char pageindex;										//页面索引
	User_Type user[MaxUserNum];										//选中的人
	User_Type * tempuser;
	ItemData * currenttestdata;										//当前测试数据
	Timer timer;													//页面超时时间
}UserPageBuffer;

unsigned char DspSelectUserPage(void *  parm);

#endif

