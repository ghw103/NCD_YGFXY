/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SelectUserPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"LunchPage.h"
#include	"SampleIDPage.h"
#include	"MyTest_Data.h"
#include	"CRC16.h"
#include	"SDFunction.h"
#include	"PlaySong_Task.h"

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

static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);

static MyState_TypeDef PageInit(void *  parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void ShowList(void);
static void SelectUser(unsigned char index);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/***************************************************************************************************
*FunctionName：DspSelectUserPage
*Description：显示选择操作人界面
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月29日08:39:45
***************************************************************************************************/
unsigned char DspSelectUserPage(void *  parm)
{
	PageInfo * currentpage = NULL;
	
	if(My_Pass == GetCurrentPage(&currentpage))
	{
		currentpage->PageInit = PageInit;
		currentpage->PageUpDate = PageUpDate;
		currentpage->LCDInput = Input;
		currentpage->PageBufferMalloc = PageBufferMalloc;
		currentpage->PageBufferFree = PageBufferFree;
		
		currentpage->PageInit(currentpage->pram);
	}

	return 0;
}

/***************************************************************************************************
*FunctionName：Input
*Description：当前界面的按键输入
*Input：pbuf -- 按键指令
*		len -- 指令长度
*Output：None
*Author：xsx
*Data：2016年6月29日08:40:22
***************************************************************************************************/
static void Input(unsigned char *pbuf , unsigned short len)
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
			
			PageBufferFree();
			PageBackTo(ParentPage);
		}
		
		/*上翻也*/
		else if(S_UserPageBuffer->lcdinput[0] == 0x1203)
		{
			/*重置倒计时30s，如果超时，则取消此次测试*/
			timer_set(&(S_UserPageBuffer->timer), 10);
			
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
			/*重置倒计时30s，如果超时，则取消此次测试*/
			timer_set(&(S_UserPageBuffer->timer), 10);
			
			if(S_UserPageBuffer->pageindex < (MaxUserNum / MaxPageShowNum))
			{
				S_UserPageBuffer->tempuser = &S_UserPageBuffer->user[(S_UserPageBuffer->pageindex)*MaxPageShowNum];
			
				if(S_UserPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserPageBuffer->tempuser, sizeof(User_Type)-2))
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
			if(S_UserPageBuffer->tempuser != NULL)
			{
				//如果是排队测试，则保存操作人到排队测试共用操作人
				if(S_UserPageBuffer->currenttestdata->testlocation > 0)
					SetPaiduiUser(S_UserPageBuffer->tempuser);
				
				/*以当前选择的操作人作为本次测试数据的操作人*/
				memcpy(&(S_UserPageBuffer->currenttestdata->testdata.user), S_UserPageBuffer->tempuser, sizeof(User_Type));
			
				PageBufferFree();
				PageAdvanceTo(DspSampleIDPage, NULL);
			}
			else
			{
				AddNumOfSongToList(45, 0);
				SendKeyCode(1);
			}
		}
		/*选择操作人*/
		else if((S_UserPageBuffer->lcdinput[0] >= 0x1205)&&(S_UserPageBuffer->lcdinput[0] <= 0x1209))
		{
			/*重置倒计时30s，如果超时，则取消此次测试*/
			timer_set(&(S_UserPageBuffer->timer), 10);
			
			S_UserPageBuffer->selectindex = S_UserPageBuffer->lcdinput[0] - 0x1205+1;
			SelectUser(S_UserPageBuffer->selectindex);
		}
	}
}

/***************************************************************************************************
*FunctionName：PageUpData
*Description：当前页面刷新
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月29日08:40:47
***************************************************************************************************/
static void PageUpDate(void)
{
	if(TimeOut == timer_expired(&(S_UserPageBuffer->timer)))
	{
		AddNumOfSongToList(8, 0);
		DeleteCurrentTest();
		
		PageBufferFree();
		PageBackTo(ParentPage);
	}
}

/***************************************************************************************************
*FunctionName：PageInit
*Description：页面初始化
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月29日08:41:15
***************************************************************************************************/
static MyState_TypeDef PageInit(void * parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(84);
	
	/*重置倒计时30s，如果超时，则取消此次测试*/
	timer_set(&(S_UserPageBuffer->timer), 10);
	
	/*获取当前测试的数据指针*/
	S_UserPageBuffer->currenttestdata = GetCurrentTestItem();
	
	/*读取所有操作人*/
	ReadUserData(S_UserPageBuffer->user);
	
	S_UserPageBuffer->pageindex = 1;
	S_UserPageBuffer->selectindex = 0;
	
	ShowList();
	SelectUser(S_UserPageBuffer->selectindex);
	
	AddNumOfSongToList(45, 0);
	
	return My_Pass;
}

/***************************************************************************************************
*FunctionName：PageBufferMalloc
*Description：当前页面缓存申请
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月29日08:41:50
***************************************************************************************************/
static MyState_TypeDef PageBufferMalloc(void)
{	
	if(NULL == S_UserPageBuffer)
	{
		S_UserPageBuffer = (UserPageBuffer *)MyMalloc(sizeof(UserPageBuffer));
			
		if(NULL == S_UserPageBuffer)
			return My_Fail;
	}
	
	memset(S_UserPageBuffer, 0, sizeof(UserPageBuffer));
		
	return My_Pass;
}

/***************************************************************************************************
*FunctionName：PageBufferFree
*Description：当前页面的缓存释放
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月29日08:42:07
***************************************************************************************************/
static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_UserPageBuffer);
	S_UserPageBuffer = NULL;
	
	return My_Pass;
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
	
	S_UserPageBuffer->tempuser = &(S_UserPageBuffer->user[i]);
	
	/*显示列表数据*/
	for(i=0; i<MaxPageShowNum; i++)
	{
		DisText(0x1210+i*8, S_UserPageBuffer->tempuser->user_name, MaxNameLen);
		
		S_UserPageBuffer->tempuser++;
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
		
		S_UserPageBuffer->tempuser = &(S_UserPageBuffer->user[i]);
		
		if(S_UserPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserPageBuffer->tempuser, sizeof(User_Type)-2))
		{
			BasicPic(0x1240, 1, 137, 268, 225, 558, 272, 364, 142+(S_UserPageBuffer->selectindex-1)*72);	
		}
		else
		{
			S_UserPageBuffer->tempuser = NULL;
			S_UserPageBuffer->selectindex = 0;
		}
	}
}

