/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SelectUserPage.h"
#include	"LCD_Driver.h"
#include	"MyMem.h"
#include	"SampleIDPage.h"
#include	"MyTest_Data.h"
#include	"CRC16.h"
#include	"PlaySong_Task.h"
#include	"UserDao.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static UserPageBuffer * S_UserPageBuffer = NULL;

/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);

static void ShowList(void);
static void SelectUser(unsigned char index);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/***************************************************************************************************
*FunctionName: createSelectUserActivity
*Description: 创建选择操作人界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyState_TypeDef createSelectUserActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "SelectUserActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
		return My_Pass;
	}
	
	return My_Fail;
}

/***************************************************************************************************
*FunctionName: activityStart
*Description: 显示主界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:32
***************************************************************************************************/
static void activityStart(void)
{
	if(S_UserPageBuffer)
	{	
		/*获取当前测试的数据指针*/
		S_UserPageBuffer->currenttestdata = GetCurrentTestItem();
	
		/*读取所有操作人*/
		ReadUserData(S_UserPageBuffer->user);
	
		S_UserPageBuffer->pageindex = 1;
		S_UserPageBuffer->selectindex = 0;
	
		ShowList();
		SelectUser(S_UserPageBuffer->selectindex);
	
		AddNumOfSongToList(9, 0);
	}
	
	SelectPage(84);
}

/***************************************************************************************************
*FunctionName: activityInput
*Description: 界面输入
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:59
***************************************************************************************************/
static void activityInput(unsigned char *pbuf , unsigned short len)
{
	if(S_UserPageBuffer)
	{
		/*命令*/
		S_UserPageBuffer->lcdinput[0] = pbuf[4];
		S_UserPageBuffer->lcdinput[0] = (S_UserPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_UserPageBuffer->lcdinput[0] == 0x1200)
		{
			DeleteCurrentTest();
			
			backToActivity(lunchActivityName);
		}
		
		/*上翻也*/
		else if(S_UserPageBuffer->lcdinput[0] == 0x1203)
		{			
			if(S_UserPageBuffer->pageindex > 1)
			{
				S_UserPageBuffer->pageindex--;
						
				S_UserPageBuffer->selectindex = 0;
						
				ShowList();
				SelectUser(S_UserPageBuffer->selectindex);
			}
		}
		/*下翻页*/
		else if(S_UserPageBuffer->lcdinput[0] == 0x1204)
		{			
			if(S_UserPageBuffer->pageindex < (MaxUserNum / MaxPageShowNum))
			{
				S_UserPageBuffer->tempUser = &S_UserPageBuffer->user[(S_UserPageBuffer->pageindex)*MaxPageShowNum];
			
				if(S_UserPageBuffer->tempUser->crc == CalModbusCRC16Fun1(S_UserPageBuffer->tempUser, sizeof(User_Type)-2))
				{
					S_UserPageBuffer->pageindex++;
						
					S_UserPageBuffer->selectindex = 0;
						
					ShowList();
					SelectUser(S_UserPageBuffer->selectindex);
				}
			}
		}
		/*确定*/
		else if(S_UserPageBuffer->lcdinput[0] == 0x1201)
		{
			if(S_UserPageBuffer->tempUser2 != NULL)
			{
				//如果是排队测试，则保存操作人到排队测试共用操作人
				if(S_UserPageBuffer->currenttestdata->testlocation > 0)
					SetPaiduiUser(S_UserPageBuffer->tempUser2);
				
				/*以当前选择的操作人作为本次测试数据的操作人*/
				memcpy(&(S_UserPageBuffer->currenttestdata->testdata.user), S_UserPageBuffer->tempUser2, sizeof(User_Type));
			
				startActivity(createSampleActivity, NULL);
			}
			else
			{
				AddNumOfSongToList(9, 0);
				SendKeyCode(1);
			}
		}
		/*选择操作人*/
		else if((S_UserPageBuffer->lcdinput[0] >= 0x1205)&&(S_UserPageBuffer->lcdinput[0] <= 0x1209))
		{			
			S_UserPageBuffer->selectindex = S_UserPageBuffer->lcdinput[0] - 0x1205+1;
			SelectUser(S_UserPageBuffer->selectindex);
		}
	}
}

/***************************************************************************************************
*FunctionName: activityFresh
*Description: 界面刷新
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:16
***************************************************************************************************/
static void activityFresh(void)
{

}

/***************************************************************************************************
*FunctionName: activityHide
*Description: 隐藏界面时要做的事
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:40
***************************************************************************************************/
static void activityHide(void)
{

}

/***************************************************************************************************
*FunctionName: activityResume
*Description: 界面恢复显示时要做的事
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:01:58
***************************************************************************************************/
static void activityResume(void)
{
	
	SelectPage(84);
}

/***************************************************************************************************
*FunctionName: activityDestroy
*Description: 界面销毁
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:02:15
***************************************************************************************************/
static void activityDestroy(void)
{
	activityBufferFree();
}

/***************************************************************************************************
*FunctionName: activityBufferMalloc
*Description: 界面数据内存申请
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 
***************************************************************************************************/
static MyState_TypeDef activityBufferMalloc(void)
{
	if(NULL == S_UserPageBuffer)
	{
		S_UserPageBuffer = MyMalloc(sizeof(UserPageBuffer));
		
		if(S_UserPageBuffer)	
		{
			memset(S_UserPageBuffer, 0, sizeof(UserPageBuffer));
	
			return My_Pass;
		}
		else
			return My_Fail;
	}
	else
		return My_Pass;
}

/***************************************************************************************************
*FunctionName: activityBufferFree
*Description: 界面内存释放
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:03:10
***************************************************************************************************/
static void activityBufferFree(void)
{
	MyFree(S_UserPageBuffer);
	S_UserPageBuffer = NULL;
}



/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：ShowList
*Description：显示列表内容
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月29日08:44:00
***************************************************************************************************/
static void ShowList(void)
{
	unsigned char i = 0;
	
	i = (S_UserPageBuffer->pageindex-1)*MaxPageShowNum;
	
	S_UserPageBuffer->tempUser = &(S_UserPageBuffer->user[i]);
	
	/*显示列表数据*/
	for(i=0; i<MaxPageShowNum; i++)
	{
		DisText(0x1210+i*8, S_UserPageBuffer->tempUser->user_name, MaxNameLen);
		
		S_UserPageBuffer->tempUser++;
	}
}

/***************************************************************************************************
*FunctionName：SelectUser
*Description：选择一个操作人，更改背景色
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月29日08:44:28
***************************************************************************************************/
static void SelectUser(unsigned char index)
{
	unsigned char i = 0;
	
	BasicPic(0x1240, 0, 140, 506, 402, 798, 470, 364, 142+(S_UserPageBuffer->selectindex-1)*72);
	
	if((S_UserPageBuffer->selectindex > 0) && (S_UserPageBuffer->selectindex <= MaxPageShowNum))
	{
		i = (S_UserPageBuffer->pageindex-1)*MaxPageShowNum + S_UserPageBuffer->selectindex-1;
		
		S_UserPageBuffer->tempUser2 = &(S_UserPageBuffer->user[i]);
		
		if(S_UserPageBuffer->tempUser2->crc == CalModbusCRC16Fun1(S_UserPageBuffer->tempUser2, sizeof(User_Type)-2))
		{
			BasicPic(0x1240, 1, 137, 266, 215, 559, 284, 363, 141+(S_UserPageBuffer->selectindex-1)*72);	
		}
		else
		{
			S_UserPageBuffer->tempUser2 = NULL;
			S_UserPageBuffer->selectindex = 0;
		}
	}
}

