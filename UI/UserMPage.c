/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"UserMPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"CRC16.h"
#include	"SystemSetPage.h"
#include	"SDFunction.h"
#include	"MyTools.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static UserMPageBuffer * S_UserMPageBuffer = NULL;

/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);
static MyState_TypeDef PageInit(void * parm);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);

static void ShowList(void);
static void ShowDetail(void);

static void AddANewUser(void);
static void DeleteAUser(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspUserMPage(void *  parm)
{
	PageInfo * currentpage = NULL;
	
	if(My_Pass == GetCurrentPage(&currentpage))
	{
		currentpage->PageInit = PageInit;
		currentpage->PageUpDate = PageUpDate;
		currentpage->LCDInput = Input;
		currentpage->PageBufferMalloc = PageBufferMalloc;
		currentpage->PageBufferFree = PageBufferFree;
		currentpage->tempP = &S_UserMPageBuffer;
		
		currentpage->PageInit(currentpage->pram);
	}
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_UserMPageBuffer)
	{
		/*命令*/
		S_UserMPageBuffer->lcdinput[0] = pbuf[4];
		S_UserMPageBuffer->lcdinput[0] = (S_UserMPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_UserMPageBuffer->lcdinput[0] == 0x1B00)
		{
			PageBufferFree();
			PageBackTo(ParentPage);
		}
		
		/*上翻也*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1B03)
		{
			if(S_UserMPageBuffer->pageindex > 1)
			{
				S_UserMPageBuffer->pageindex--;
					
				S_UserMPageBuffer->selectindex = 0;
					
				ShowList();
				ShowDetail();
			}
		}
		/*下翻页*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1B04)
		{
			if(S_UserMPageBuffer->pageindex < (MaxUserNum / MaxPageShowNum))
			{
				S_UserMPageBuffer->tempuser = &S_UserMPageBuffer->user[(S_UserMPageBuffer->pageindex)*MaxPageShowNum];
			
				if(S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, sizeof(User_Type)-2))
				{
					S_UserMPageBuffer->pageindex++;
					
					S_UserMPageBuffer->selectindex = 0;
						
					ShowList();
					ShowDetail();
				}
			}
		}
		/*删除*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1B01)
		{
			DeleteAUser();
		}
		/*修改或者添加*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1B02)
		{
			AddANewUser();
		}
		/*选择操作人*/
		else if((S_UserMPageBuffer->lcdinput[0] >= 0x1B07)&&(S_UserMPageBuffer->lcdinput[0] <= 0x1B0B))
		{
			S_UserMPageBuffer->tempuser = &S_UserMPageBuffer->user[(S_UserMPageBuffer->pageindex - 1)*MaxPageShowNum + S_UserMPageBuffer->lcdinput[0] - 0x1B07];
			
			if(S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, sizeof(User_Type)-2))
			{
				S_UserMPageBuffer->selectindex = S_UserMPageBuffer->lcdinput[0] - 0x1B07+1;
				ShowDetail();
			}
		}
		/*姓名*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1B60)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_name, 0, MaxNameLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_name, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
		/*年龄*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1B65)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_age, 0, MaxAgeLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_age, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
		/*性别*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1B67)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_sex, 0, MaxSexLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_sex, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
		/*联系方式*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1B70)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_phone, 0, MaxPhoneLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_phone, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
		/*职位*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1B78)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_job, 0, MaxJobLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_job, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
		/*备注*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1B80)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_desc, 0, MaxDescLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_desc, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
	}
}

static void PageUpDate(void)
{

}

static MyState_TypeDef PageInit(void * parm)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;
	
	SelectPage(106);
	
	/*读取所有操作人*/
	ReadUserData(S_UserMPageBuffer->user);
	
	S_UserMPageBuffer->pageindex = 1;
	S_UserMPageBuffer->selectindex = 0;
	
	ShowList();
	ShowDetail();

	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	if(NULL == S_UserMPageBuffer)
	{
		S_UserMPageBuffer = (UserMPageBuffer *)MyMalloc(sizeof(UserMPageBuffer));
			
		if(NULL == S_UserMPageBuffer)
			return My_Fail;
	}
	
	memset(S_UserMPageBuffer, 0, sizeof(UserMPageBuffer));
		
	return My_Pass;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_UserMPageBuffer);
	S_UserMPageBuffer = NULL;
	
	return My_Pass;
}

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
static void ShowList(void)
{
	unsigned char i = 0;
	
	i = (S_UserMPageBuffer->pageindex-1)*MaxPageShowNum;
	
	S_UserMPageBuffer->tempuser = &(S_UserMPageBuffer->user[i]);
	
	/*显示列表数据*/
	for(i=0; i<MaxPageShowNum; i++)
	{
		ClearText(0x1B10+8*i, 16);
		
		if(S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, sizeof(User_Type)-2))
			DisText(0x1B10+8*i, S_UserMPageBuffer->tempuser->user_name, strlen(S_UserMPageBuffer->tempuser->user_name));
		
		S_UserMPageBuffer->tempuser++;
	}
}

static void ShowDetail(void)
{
	unsigned char i = 0;
	
	ClearText(0x1B60, 10);
	ClearText(0x1B65, 3);
	ClearText(0x1B67, 1);
	ClearText(0x1B70, 16);
	ClearText(0x1B78, 16);
	ClearText(0x1B80, 16);
	BasicPic(0x1B50, 0, 140, 506, 402, 798, 470, 364, 142+(S_UserMPageBuffer->selectindex-1)*72);
	
	if((S_UserMPageBuffer->selectindex > 0) && (S_UserMPageBuffer->selectindex <= MaxPageShowNum))
	{
		i = (S_UserMPageBuffer->pageindex-1)*MaxPageShowNum + S_UserMPageBuffer->selectindex-1;
	
		S_UserMPageBuffer->tempuser = &(S_UserMPageBuffer->user[i]);
		
		if(S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, sizeof(User_Type)-2))
		{
			memcpy(&(S_UserMPageBuffer->tempnewuser), S_UserMPageBuffer->tempuser, sizeof(User_Type));
			DisText(0x1B60, S_UserMPageBuffer->tempuser->user_name, strlen(S_UserMPageBuffer->tempuser->user_name));
			DisText(0x1B65, S_UserMPageBuffer->tempuser->user_age, strlen(S_UserMPageBuffer->tempuser->user_age));
			DisText(0x1B67, S_UserMPageBuffer->tempuser->user_sex, strlen(S_UserMPageBuffer->tempuser->user_sex));
			DisText(0x1B70, S_UserMPageBuffer->tempuser->user_phone, strlen(S_UserMPageBuffer->tempuser->user_phone));
			DisText(0x1B78, S_UserMPageBuffer->tempuser->user_job, strlen(S_UserMPageBuffer->tempuser->user_job));
			DisText(0x1B80, S_UserMPageBuffer->tempuser->user_desc, strlen(S_UserMPageBuffer->tempuser->user_desc));
			BasicPic(0x1B50, 1, 140, 506, 402, 798, 470, 158, 136+(S_UserMPageBuffer->selectindex-1)*72);
		}
	}
	else
		memset(&(S_UserMPageBuffer->tempnewuser), 0, sizeof(User_Type));
}


static void AddANewUser(void)
{
	unsigned char i=0;
	unsigned char type = 0;
	
	if(S_UserMPageBuffer->tempnewuser.crc == CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2))
	{
		/*检查是否已存在*/
		S_UserMPageBuffer->tempuser = &(S_UserMPageBuffer->user[0]);
		
		for(i=0; i<MaxUserNum; i++)
		{
			if( (S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, sizeof(User_Type)-2)) &&
				(pdPASS == CheckStrIsSame(S_UserMPageBuffer->tempuser->user_name, S_UserMPageBuffer->tempnewuser.user_name, MaxNameLen)))
			{
				memcpy(S_UserMPageBuffer->tempuser, &(S_UserMPageBuffer->tempnewuser), sizeof(User_Type));
				type = 1;
				break;
			}
			S_UserMPageBuffer->tempuser++;
		}
		
		/*如果不存在，则新建*/
		if(i >= MaxUserNum)
		{
			S_UserMPageBuffer->tempuser = &(S_UserMPageBuffer->user[0]);
		
			for(i=0; i<MaxUserNum; i++)
			{
				if(S_UserMPageBuffer->tempuser->crc != CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, sizeof(User_Type)-2))
				{
					memcpy(S_UserMPageBuffer->tempuser, &(S_UserMPageBuffer->tempnewuser), sizeof(User_Type));
					type = 2;
					break;
				}
				S_UserMPageBuffer->tempuser++;
			}
		}
		
		/*如果有空间*/
		if(i < MaxUserNum)
		{
			/*保存数据*/
			if(My_Fail == SaveUserData(&(S_UserMPageBuffer->user[0])))
			{
				if(type == 1)
					SendKeyCode(6);
				else if(type == 2)
					SendKeyCode(4);
				ReadUserData(S_UserMPageBuffer->user);
			}
			else
			{
				if(type == 1)
					SendKeyCode(5);
				else if(type == 2)
					SendKeyCode(3);
			}
			
			memset(&(S_UserMPageBuffer->tempnewuser), 0, sizeof(User_Type));
			S_UserMPageBuffer->selectindex = 0;

			ShowList();
			ShowDetail();
		}
		else
			SendKeyCode(4);
	}
}

static void DeleteAUser(void)
{
	if((S_UserMPageBuffer->selectindex > 0) && (S_UserMPageBuffer->selectindex <= MaxPageShowNum))
	{
		S_UserMPageBuffer->tempuser = &(S_UserMPageBuffer->user[(S_UserMPageBuffer->pageindex-1)*MaxPageShowNum + S_UserMPageBuffer->selectindex-1]);
		memset(S_UserMPageBuffer->tempuser, 0, sizeof(User_Type));
		
		
		/*保存数据*/
		
		if(My_Fail == SaveUserData(&(S_UserMPageBuffer->user[0])))
			SendKeyCode(2);
		else
			SendKeyCode(1);
		
		memset(S_UserMPageBuffer->user, 0, sizeof(User_Type)*MaxUserNum);
		ReadUserData(S_UserMPageBuffer->user);
		
		S_UserMPageBuffer->pageindex = 1;
		S_UserMPageBuffer->selectindex = 0;
			
		ShowList();
		ShowDetail();
	}

}

