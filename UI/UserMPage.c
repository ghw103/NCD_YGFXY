/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"UserMPage.h"
#include	"LCD_Driver.h"
#include	"MyMem.h"
#include	"CRC16.h"
#include	"SystemSetPage.h"
#include	"SDFunction.h"
#include	"MyTools.h"
#include	"SleepPage.h"

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
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);

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

/***************************************************************************************************
*FunctionName: createSelectUserActivity
*Description: 创建选择操作人界面
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月21日09:00:09
***************************************************************************************************/
MyState_TypeDef createUserManagerActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "UserManagerActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_UserMPageBuffer)
	{
		//读取系统设置
		getSystemSetData(&(S_UserMPageBuffer->systemSetData));
		
		timer_set(&(S_UserMPageBuffer->timer), S_UserMPageBuffer->systemSetData.ledSleepTime);
		
		/*读取所有操作人*/
		ReadUserData(S_UserMPageBuffer->user);
		
		S_UserMPageBuffer->pageindex = 1;
		S_UserMPageBuffer->selectindex = 0;
		
		ShowList();
		ShowDetail();
	}
	SelectPage(106);
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
	if(S_UserMPageBuffer)
	{
		/*命令*/
		S_UserMPageBuffer->lcdinput[0] = pbuf[4];
		S_UserMPageBuffer->lcdinput[0] = (S_UserMPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		//重置休眠时间
		timer_restart(&(S_UserMPageBuffer->timer));
		
		/*返回*/
		if(S_UserMPageBuffer->lcdinput[0] == 0x1d00)
		{
			backToFatherActivity();
		}
		
		/*上翻也*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d03)
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
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d04)
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
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d01)
		{
			DeleteAUser();
		}
		/*修改或者添加*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d02)
		{
			AddANewUser();
		}
		/*选择操作人*/
		else if((S_UserMPageBuffer->lcdinput[0] >= 0x1d07)&&(S_UserMPageBuffer->lcdinput[0] <= 0x1d0B))
		{
			S_UserMPageBuffer->tempuser = &S_UserMPageBuffer->user[(S_UserMPageBuffer->pageindex - 1)*MaxPageShowNum + S_UserMPageBuffer->lcdinput[0] - 0x1d07];
			
			if(S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, sizeof(User_Type)-2))
			{
				S_UserMPageBuffer->selectindex = S_UserMPageBuffer->lcdinput[0] - 0x1d07+1;
				ShowDetail();
			}
		}
		/*姓名*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d50)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_name, 0, MaxNameLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_name, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
		/*年龄*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d60)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_age, 0, MaxAgeLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_age, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
		/*性别*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d70)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_sex, 0, MaxSexLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_sex, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
		/*联系方式*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d80)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_phone, 0, MaxPhoneLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_phone, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
		/*职位*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1d90)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_job, 0, MaxJobLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_job, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
		}
		/*备注*/
		else if(S_UserMPageBuffer->lcdinput[0] == 0x1da0)
		{
			memset(S_UserMPageBuffer->tempnewuser.user_desc, 0, MaxDescLen);
			memcpy(S_UserMPageBuffer->tempnewuser.user_desc, &pbuf[7], GetBufLen(&pbuf[7] , 2*pbuf[6]));
			S_UserMPageBuffer->tempnewuser.crc = CalModbusCRC16Fun1(&(S_UserMPageBuffer->tempnewuser), sizeof(User_Type)-2);
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
	if(S_UserMPageBuffer)
	{
		if(TimeOut == timer_expired(&(S_UserMPageBuffer->timer)))
			startActivity(createSleepActivity, NULL);
	}
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
	if(S_UserMPageBuffer)
	{
		timer_restart(&(S_UserMPageBuffer->timer));
	}
	
	SelectPage(106);
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
	if(NULL == S_UserMPageBuffer)
	{
		S_UserMPageBuffer = MyMalloc(sizeof(UserMPageBuffer));
		
		if(S_UserMPageBuffer)
		{
			memset(S_UserMPageBuffer, 0, sizeof(UserMPageBuffer));
	
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
	MyFree(S_UserMPageBuffer);
	S_UserMPageBuffer = NULL;
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
		ClearText(0x1d10+8*i, 16);
		
		if(S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, sizeof(User_Type)-2))
			DisText(0x1d10+8*i, S_UserMPageBuffer->tempuser->user_name, strlen(S_UserMPageBuffer->tempuser->user_name));
		
		S_UserMPageBuffer->tempuser++;
	}
}

static void ShowDetail(void)
{
	unsigned char i = 0;
	
	ClearText(0x1d50, 10);
	ClearText(0x1d60, 10);
	ClearText(0x1d70, 10);
	ClearText(0x1d80, 30);
	ClearText(0x1d90, 30);
	ClearText(0x1da0, 30);
	BasicPic(0x1d40, 0, 140, 506, 402, 798, 470, 364, 142+(S_UserMPageBuffer->selectindex-1)*72);
	
	if((S_UserMPageBuffer->selectindex > 0) && (S_UserMPageBuffer->selectindex <= MaxPageShowNum))
	{
		i = (S_UserMPageBuffer->pageindex-1)*MaxPageShowNum + S_UserMPageBuffer->selectindex-1;
	
		S_UserMPageBuffer->tempuser = &(S_UserMPageBuffer->user[i]);
		
		if(S_UserMPageBuffer->tempuser->crc == CalModbusCRC16Fun1(S_UserMPageBuffer->tempuser, sizeof(User_Type)-2))
		{
			memcpy(&(S_UserMPageBuffer->tempnewuser), S_UserMPageBuffer->tempuser, sizeof(User_Type));
			DisText(0x1d50, S_UserMPageBuffer->tempuser->user_name, strlen(S_UserMPageBuffer->tempuser->user_name));
			DisText(0x1d60, S_UserMPageBuffer->tempuser->user_age, strlen(S_UserMPageBuffer->tempuser->user_age));
			DisText(0x1d70, S_UserMPageBuffer->tempuser->user_sex, strlen(S_UserMPageBuffer->tempuser->user_sex));
			DisText(0x1d80, S_UserMPageBuffer->tempuser->user_phone, strlen(S_UserMPageBuffer->tempuser->user_phone));
			DisText(0x1d90, S_UserMPageBuffer->tempuser->user_job, strlen(S_UserMPageBuffer->tempuser->user_job));
			DisText(0x1da0, S_UserMPageBuffer->tempuser->user_desc, strlen(S_UserMPageBuffer->tempuser->user_desc));
			BasicPic(0x1d40, 1, 137, 268, 225, 558, 272, 158, 136+(S_UserMPageBuffer->selectindex-1)*72);
		}
	}
	else
		memset(&(S_UserMPageBuffer->tempnewuser), 0, sizeof(User_Type));
}


static void AddANewUser(void)
{
	unsigned char i=0;
	
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
				SendKeyCode(2);
				ReadUserData(S_UserMPageBuffer->user);
			}
			else
			{
				SendKeyCode(1);
			}
			
			memset(&(S_UserMPageBuffer->tempnewuser), 0, sizeof(User_Type));
			S_UserMPageBuffer->selectindex = 0;

			ShowList();
			ShowDetail();
		}
		else
			SendKeyCode(2);
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

