#ifndef __USERM_P_H__
#define __USERM_P_H__

#include	"Define.h"
#include	"UI_Data.h"

typedef struct UserMPageBuffer_tag {
	unsigned char selectindex;										//选中的索引
	unsigned char pageindex;										//页面索引
	unsigned char filemaxitem;							//当前文件保存数据数目
	unsigned char filemaxpagenum;						//当前文件保存数据的页数
	User_Type user[MaxUserNum];									//选中的人
	User_Type * tempuser;
	User_Type tempnewuser;
	unsigned short lcdinput[100];
}UserMPageBuffer;

MyState_TypeDef createUserManagerActivity(Activity * thizActivity, Intent * pram);

#endif

