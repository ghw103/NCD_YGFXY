/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"SelectUserPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"Users_Data.h"
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
static UserPageBuffer * GB_UserPageBuffer = NULL;

/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpData(void);

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
	SysPage * myPage = GetSysPage();

	myPage->CurrentPage = DspSelectUserPage;
	myPage->LCDInput = Input;
	myPage->PageUpData = PageUpData;
	myPage->ParentPage = DspLunchPage;
	myPage->ChildPage = DspSampleIDPage;
	myPage->PageInit = PageInit;
	myPage->PageBufferMalloc = PageBufferMalloc;
	myPage->PageBufferFree = PageBufferFree;
	
	SelectPage(54);
	
	myPage->PageInit(NULL);

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
	unsigned short *pdata = NULL;
	SysPage * myPage = GetSysPage();
	
	pdata = MyMalloc((len/2)*sizeof(unsigned short));
	if(pdata == NULL)
		return;
	
	/*命令*/
	pdata[0] = pbuf[4];
	pdata[0] = (pdata[0]<<8) + pbuf[5];
	
	/*返回*/
	if(pdata[0] == 0x1c00)
	{
		DeleteCurrentTest();
		
		PageBufferFree();
		myPage->ParentPage(NULL);
	}
	
	/*上翻也*/
	else if(pdata[0] == 0x1c02)
	{
		if(GB_UserPageBuffer)
		{
			if(GB_UserPageBuffer->pageindex > 1)
			{
				GB_UserPageBuffer->pageindex--;
					
				GB_UserPageBuffer->selectindex = 0;
					
				ShowList();
				SelectUser(GB_UserPageBuffer->selectindex);
			}
		}
	}
	/*下翻页*/
	else if(pdata[0] == 0x1c03)
	{
		if(GB_UserPageBuffer)
		{
			if(GB_UserPageBuffer->pageindex < (MaxUserNum / MaxPageShowNum))
			{
				GB_UserPageBuffer->tempuser = &GB_UserPageBuffer->user[(GB_UserPageBuffer->pageindex)*MaxPageShowNum];
		
				if(GB_UserPageBuffer->tempuser->crc == CalModbusCRC16Fun1(GB_UserPageBuffer->tempuser, sizeof(User_Type)-2))
				{
					GB_UserPageBuffer->pageindex++;
					
					GB_UserPageBuffer->selectindex = 0;
					
					ShowList();
					SelectUser(GB_UserPageBuffer->selectindex);
				}
			}
		}
	}
	/*确定*/
	else if(pdata[0] == 0x1c01)
	{
		if(GB_UserPageBuffer->selectindex != 0)
		{
			/*以当前选择的操作人作为本次测试数据的操作人*/
			memcpy(&(GB_UserPageBuffer->currenttestdata->testdata.user), GB_UserPageBuffer->user, sizeof(User_Type));
		
			PageBufferFree();
			myPage->ChildPage(NULL);
		}
		else
		{
			AddNumOfSongToList(45, 0);
			SendKeyCode(1);
		}
	}
	/*选择操作人*/
	else if((pdata[0] >= 0x1c04)&&(pdata[0] <= 0x1c0e))
	{
		GB_UserPageBuffer->tempuser = &GB_UserPageBuffer->user[(GB_UserPageBuffer->pageindex - 1)*MaxPageShowNum + pdata[0] - 0x1c04];
		
		if(GB_UserPageBuffer->tempuser->crc == CalModbusCRC16Fun1(GB_UserPageBuffer->tempuser, sizeof(User_Type)-2))
		{
			GB_UserPageBuffer->selectindex = pdata[0] - 0x1c04+1;
			SelectUser(GB_UserPageBuffer->selectindex);
		}
	}

	MyFree(pdata);
}

/***************************************************************************************************
*FunctionName：PageUpData
*Description：当前页面刷新
*Input：None
*Output：None
*Author：xsx
*Data：2016年6月29日08:40:47
***************************************************************************************************/
static void PageUpData(void)
{
	if(TimeOut == timer_expired(&(GB_UserPageBuffer->timer)))
	{
		AddNumOfSongToList(8, 0);
		DeleteCurrentTest();
		
		PageBufferFree();
		GetSysPage()->ParentPage(NULL);
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
	
	/*重置倒计时30s，如果超时，则取消此次测试*/
	timer_set(&(GB_UserPageBuffer->timer), 10);
	
	/*获取当前测试的数据指针*/
	GB_UserPageBuffer->currenttestdata = GetCurrentTestItem();
	
	/*读取所有操作人*/
	ReadUserData(GB_UserPageBuffer->user);
	
	GB_UserPageBuffer->pageindex = 1;
	GB_UserPageBuffer->selectindex = 0;
	
	ShowList();
	SelectUser(GB_UserPageBuffer->selectindex);
	
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
	if(GB_UserPageBuffer)
		return My_Fail;
	
	GB_UserPageBuffer = (UserPageBuffer *)MyMalloc(sizeof(UserPageBuffer));
			
	if(GB_UserPageBuffer)
	{
		memset(GB_UserPageBuffer, 0, sizeof(UserPageBuffer));
		
		return My_Pass;
	}
	else
		return My_Fail;
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
	MyFree(GB_UserPageBuffer);
	GB_UserPageBuffer = NULL;
	
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
	
	i = (GB_UserPageBuffer->pageindex-1)*MaxPageShowNum;
	
	GB_UserPageBuffer->tempuser = &(GB_UserPageBuffer->user[i]);
	
	/*显示列表数据*/
	for(i=0; i<MaxPageShowNum; i++)
	{
		ClearText(0x1c10+i*16, 32);
		
		if(GB_UserPageBuffer->tempuser->crc != 0)
			DisText(0x1c10+i*16, GB_UserPageBuffer->tempuser->user_name, strlen(GB_UserPageBuffer->tempuser->user_name));
		
		GB_UserPageBuffer->tempuser++;
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
	
	BasicPic(0x1cb0, 0, 54, 600, 1, 840, 31, 392, 123+(GB_UserPageBuffer->selectindex-1)*34);
	
	if((GB_UserPageBuffer->selectindex > 0) && (GB_UserPageBuffer->selectindex <= MaxPageShowNum))
	{
		i = (GB_UserPageBuffer->pageindex-1)*MaxPageShowNum + GB_UserPageBuffer->selectindex-1;
		
		GB_UserPageBuffer->tempuser = &(GB_UserPageBuffer->user[i]);
		
		if(GB_UserPageBuffer->tempuser->crc == CalModbusCRC16Fun1(GB_UserPageBuffer->tempuser, sizeof(User_Type)-2))
			BasicPic(0x1cb0, 1, 54, 600, 1, 840, 31, 392, 123+(GB_UserPageBuffer->selectindex-1)*34);
	}
}

