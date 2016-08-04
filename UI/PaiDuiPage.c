/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"PaiDuiPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"SystemSetPage.h"
#include	"MyMem.h"
#include	"DeviceInfo_Data.h"
#include	"TimeDownNorPage.h"

#include	"TM1623_Driver.h"
#include	"OutModel_Fun.h"
#include	"Motor_Fun.h"
#include	"CardStatues_Data.h"
#include	"PlaySong_Task.h"

#include	"MyTest_Data.h"
#include	"LunchPage.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static PaiDuiPageBuffer * S_PaiDuiPageBuffer = NULL;
/******************************************************************************************/
/*****************************************局部函数声明*************************************/
static void Input(unsigned char *pbuf , unsigned short len);
static void PageUpDate(void);
static void UpData(void);

static MyState_TypeDef PageInit(void *pram);
static MyState_TypeDef PageBufferMalloc(void);
static MyState_TypeDef PageBufferFree(void);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

unsigned char DspPaiDuiPage(void *  parm)
{
	SetGBSysPage(DspPaiDuiPage, DspLunchPage, DspTimeDownNorPage, Input, PageUpDate, PageInit, PageBufferMalloc, PageBufferFree);
	
	GBPageInit(parm);
	
	return 0;
}


static void Input(unsigned char *pbuf , unsigned short len)
{
	unsigned short *pdata = NULL;
	
	pdata = MyMalloc((len/2)*sizeof(unsigned short));
	if(pdata == NULL)
		return;
	
	/*命令*/
	pdata[0] = pbuf[4];
	pdata[0] = (pdata[0]<<8) + pbuf[5];
	
	/*返回*/
	if(pdata[0] == 0x2600)
	{
		GBPageBufferFree();
		GotoGBParentPage(NULL);
	}
	/*等待插卡*/
	else if(pdata[0] == 0x00a1)
	{
		
	}
	
	MyFree(pdata);
}

static void PageUpDate(void)
{
	static unsigned short count = 0;
	
	if(count % 50 == 0)
		UpData();
	
	count++;
}

static MyState_TypeDef PageInit(void *pram)
{
	if(My_Fail == PageBufferMalloc())
		return My_Fail;

	SelectPage(63);
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{	
	S_PaiDuiPageBuffer = (PaiDuiPageBuffer *)MyMalloc(sizeof(PaiDuiPageBuffer));
			
	if(S_PaiDuiPageBuffer)
	{
		memset(S_PaiDuiPageBuffer, 0, sizeof(PaiDuiPageBuffer));
		
		return My_Pass;
	}
	else
		return My_Fail;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_PaiDuiPageBuffer);
	S_PaiDuiPageBuffer = NULL;
	
	return My_Pass;
}


static void UpData(void)
{
	unsigned char index = 1;
	static unsigned char count = 0;
	
	/*更新倒计时数据*/
	for(index=1; index<PaiDuiWeiNum; index++)
	{
		S_PaiDuiPageBuffer->tempd2 = GetTestItemByIndex(index);
		
		if(S_PaiDuiPageBuffer->tempd2)
		{
			if((S_PaiDuiPageBuffer->tempd2->statues == statues1)||(S_PaiDuiPageBuffer->tempd2->statues == statues2)||
				(S_PaiDuiPageBuffer->tempd2->statues == statues4)||(S_PaiDuiPageBuffer->tempd2->statues == statues5)||
				(S_PaiDuiPageBuffer->tempd2->statues == statues6))
			{
				//显示倒计时
				ClearText(0x2710+(index-1)*3, 6);
				sprintf(S_PaiDuiPageBuffer->buf, "%dS", timer_surplus(&(S_PaiDuiPageBuffer->tempd2->timer)));
				DisText(0x2710+(index-1)*3, S_PaiDuiPageBuffer->buf, strlen(S_PaiDuiPageBuffer->buf));
				
				//检测卡图标闪烁
				if((count % 2) == 0)
					S_PaiDuiPageBuffer->myico.ICO_ID = 23;
				else
					S_PaiDuiPageBuffer->myico.ICO_ID = 24;
				
				S_PaiDuiPageBuffer->myico.X = 65+((index-1)*119);
				S_PaiDuiPageBuffer->myico.Y = 100;
				
				BasicUI(0x2690+(index-1)*16 ,0x1907 , 1, &(S_PaiDuiPageBuffer->myico) , sizeof(Basic_ICO));
				
				//显示倒计时槽
				S_PaiDuiPageBuffer->tempvalue = timer_surplus(&(S_PaiDuiPageBuffer->tempd2->timer));
				S_PaiDuiPageBuffer->tempvalue /= S_PaiDuiPageBuffer->tempd2->testdata.temperweima.CardWaitTime*60;
				S_PaiDuiPageBuffer->tempvalue *= 250;
				S_PaiDuiPageBuffer->tempvalue = 570-S_PaiDuiPageBuffer->tempvalue;
				BasicPic(0x2610+(index-1)*16, 1, 100, 15, (unsigned short)(S_PaiDuiPageBuffer->tempvalue), 22, 570, 55 + (index-1)*119, (unsigned short)(S_PaiDuiPageBuffer->tempvalue)-220);
			}
			/*倒计时*/
			else if(S_PaiDuiPageBuffer->tempd2->statues == statues3)
			{
				ClearText(0x2710+(index-1)*3, 6);
				sprintf(S_PaiDuiPageBuffer->buf, "%dS", timer_surplus(&(S_PaiDuiPageBuffer->tempd2->timer)));
				DisText(0x2710+(index-1)*3, S_PaiDuiPageBuffer->buf, strlen(S_PaiDuiPageBuffer->buf));
				
				S_PaiDuiPageBuffer->myico.ICO_ID = 24;
				
				S_PaiDuiPageBuffer->myico.X = 65+((index-1)*119);
				S_PaiDuiPageBuffer->myico.Y = 100;
				BasicUI(0x2690+(index-1)*16 ,0x1907 , 1, &(S_PaiDuiPageBuffer->myico) , sizeof(Basic_ICO));
				
				S_PaiDuiPageBuffer->tempvalue = timer_surplus(&(S_PaiDuiPageBuffer->tempd2->timer));
				S_PaiDuiPageBuffer->tempvalue /= S_PaiDuiPageBuffer->tempd2->testdata.temperweima.CardWaitTime*60;
				S_PaiDuiPageBuffer->tempvalue *= 250;
				S_PaiDuiPageBuffer->tempvalue = 570-S_PaiDuiPageBuffer->tempvalue;
				BasicPic(0x2610+(index-1)*16, 1, 100, 15, (unsigned short)(S_PaiDuiPageBuffer->tempvalue), 22, 570, 55 + (index-1)*119, (unsigned short)(S_PaiDuiPageBuffer->tempvalue)-220);
			}
			/*进入超时处理阶段*/
			else if(S_PaiDuiPageBuffer->tempd2->statues == statues8)
			{			
				ClearText(0x2710+(index-1)*3, 6);
				sprintf(S_PaiDuiPageBuffer->buf, "%dS", timer_Count(&(S_PaiDuiPageBuffer->tempd2->timer2)));
				DisText(0x2710+(index-1)*3, S_PaiDuiPageBuffer->buf, strlen(S_PaiDuiPageBuffer->buf));
				
				S_PaiDuiPageBuffer->myico.ICO_ID = 24;
				S_PaiDuiPageBuffer->myico.X = 65+((index-1)*119);
				S_PaiDuiPageBuffer->myico.Y = 100;
				
				BasicUI(0x2690+(index-1)*16 ,0x1907 , 1, &(S_PaiDuiPageBuffer->myico) , sizeof(Basic_ICO));
			}
			else if((S_PaiDuiPageBuffer->tempd2->statues == statues9) || (S_PaiDuiPageBuffer->tempd2->statues == statues13) || 
				(S_PaiDuiPageBuffer->tempd2->statues == statues12))
			{
				ClearText(0x2710+(index-1)*3, 6);
				sprintf(S_PaiDuiPageBuffer->buf, "%dS", timer_Count(&(S_PaiDuiPageBuffer->tempd2->timer2)));
				DisText(0x2710+(index-1)*3, S_PaiDuiPageBuffer->buf, strlen(S_PaiDuiPageBuffer->buf));
				
				if(count % 2 == 0)
					S_PaiDuiPageBuffer->myico.ICO_ID = 23;
				else
					S_PaiDuiPageBuffer->myico.ICO_ID = 24;
				
				S_PaiDuiPageBuffer->myico.X = 65+((index-1)*119);
				S_PaiDuiPageBuffer->myico.Y = 100;
				
				BasicUI(0x2690+(index-1)*16 ,0x1907 , 1, &(S_PaiDuiPageBuffer->myico) , sizeof(Basic_ICO));
			}
			/*将卡插入卡槽*/
			else if((S_PaiDuiPageBuffer->tempd2->statues == statues10)||(S_PaiDuiPageBuffer->tempd2->statues == statues11))
			{
				ClearText(0x2710+(index-1)*3, 6);
				sprintf(S_PaiDuiPageBuffer->buf, "%dS", timer_Count(&(S_PaiDuiPageBuffer->tempd2->timer2)));
				DisText(0x2710+(index-1)*3, S_PaiDuiPageBuffer->buf, strlen(S_PaiDuiPageBuffer->buf));
				
				if(count % 2 == 0)
					S_PaiDuiPageBuffer->myico.ICO_ID = 23;
				else
					S_PaiDuiPageBuffer->myico.ICO_ID = 24;
				
				S_PaiDuiPageBuffer->myico.X = 65+((index-1)*119);
				S_PaiDuiPageBuffer->myico.Y = 100;
				
				BasicUI(0x2690+(index-1)*16 ,0x1907 , 1, &(S_PaiDuiPageBuffer->myico) , sizeof(Basic_ICO));
			}
		}
		else
		{
			//清除倒计时时间
			ClearText(0x2710+(index-1)*3, 6);
			//显示蓝色卡
			S_PaiDuiPageBuffer->myico.ICO_ID = 23;
			S_PaiDuiPageBuffer->myico.X = 65+((index-1)*119);
			S_PaiDuiPageBuffer->myico.Y = 100;
			BasicUI(0x2690+(index-1)*16 ,0x1907 , 1, &(S_PaiDuiPageBuffer->myico) , sizeof(Basic_ICO));
		}
	}

	count++;
}


void CheckTime(void)
{
	unsigned char index = 0;
	CardState_Def tempCardState;
	KeyChange_Def tempkey;
	ItemData * temp = NULL;
	unsigned short tempvalue = 0;
	
	//检测测试位置的卡拔插动作
	if(pdPASS == GetCardStatuesFromQueue(&tempCardState, 10/portTICK_RATE_MS))
	{
		temp = GetCurrentTestItem();
		if(temp)
		{
			if(tempCardState == NoCard)
			{
				if(temp->statues == statues1)
					temp->statues = statues2;
			}
			else if(tempCardState == CardIN)
			{
				if(temp->statues == statues6)
				{
					temp->statues = statues7;
					UpOneModelData(temp->testlocation-1, R_OFF_G_ON, 0);
					DspTimeDownNorPage(NULL);
				}
				else if(temp->statues == statues11)
				{		
					temp->statues = statues7;
					UpOneModelData(temp->testlocation-1, R_OFF_G_ON, 0);
					DspTimeDownNorPage(NULL);
				}
			}
		}
	}
	
	//检测排队位的卡拔插动作
	if(pdPASS == GetKeyStatuesFromQueue(&tempkey, 10/portTICK_RATE_MS))
	{
		temp = GetTestItemByIndex(tempkey.index);
		if(temp)
		{
			/*检测到有卡拔出排队位*/
			if(tempkey.keystatues == KEY_NoPressed)
			{
				if(temp->statues == statues3)
				{
					UpOneModelData(tempkey.index-1, R_ON_G_OFF, 0);
					temp->statues = statues4;
					timer_restart(&(temp->timer3));
					AddNumOfSongToList(tempkey.index+26, 0);
				}
				else if(temp->statues == statues5)
				{
					temp->statues = statues6;
				}
				else if(temp->statues == statues10)
					temp->statues = statues11;
			}
			else if(tempkey.keystatues == KEY_Pressed)
			{
				/*等待插入，则转入3状态*/
				if(temp->statues == statues2)
				{
					UpOneModelData(tempkey.index-1, R_ON_G_OFF, 0);
					temp->statues = statues3;
					SetCurrentTestItem(NULL);
				}
				else if(temp->statues == statues4)
				{
					UpOneModelData(tempkey.index-1, R_ON_G_OFF, 0);
					temp->statues = statues3;
					timer_restart(&(temp->timer3));
					AddNumOfSongToList(tempkey.index+26, 0);
				}
				else if(temp->statues == statues9)
				{
					UpOneModelData(tempkey.index-1, R_ON_G_OFF, 0);
					temp->statues = statues8;
				}
				else if(temp->statues == statues12)
				{
					UpOneModelData(tempkey.index-1, R_ON_G_OFF, 0);
					temp->statues = statues8;
				}
				else if(temp->statues == statues13)
				{
					UpOneModelData(tempkey.index-1, R_ON_G_OFF, 0);
					temp->statues = statues8;
				}
			}
		}
	}
	
	for(index=1; index<PaiDuiWeiNum; index++)
	{
		temp = GetTestItemByIndex(index);
		
		if(temp)
		{
			/*重置操作倒计时*/
			if(temp->statues == statuesNull)
			{
				temp->statues = statues1;
				
				UpOneModelData(index-1, R_ON_G_OFF, 5);
				//20S提示一次将卡插入排队位
				timer_set(&(temp->timer3), 10);
				AddNumOfSongToList(index+26, 0);
			}
			else if((temp->statues == statues1)||(temp->statues == statues2))
			{
				tempvalue = timer_surplus(&(temp->timer));
				//如果检测卡反应时间到
				if(tempvalue <= 20)
				{
					if(temp->statues == statues1)
					{
						UpOneModelData(index-1, R_OFF_G_ON, 0);
						temp->statues = statues7;
						DspTimeDownNorPage(NULL);
					}
					else if(temp->statues == statues2)
					{
						UpOneModelData(index-1, R_ON_G_OFF, 5);
						temp->statues = statues6;
						timer_restart(&(temp->timer3));
						AddNumOfSongToList(index+34, 0);
					}
				}
				else if(TimeOut == timer_expired(&(temp->timer3)))
				{
					//20S提示一次将卡插入排队位
					timer_restart(&(temp->timer3));
					
					AddNumOfSongToList(index+26, 0);
				}
			}
			else if(temp->statues == statues3)
			{
				tempvalue = timer_surplus(&(temp->timer));
				if(0 == tempvalue)
				{
					UpOneModelData(index-1, R_ON_G_OFF, 0);
					timer_restart(&(temp->timer2));
					temp->statues = statues8;
					AddNumOfSongToList(index+10, 0);
				}
				/*倒计时小于20S，提示插入测试位*/
				else if(tempvalue < 20)
				{
					//如果空闲
					if(GetCurrentTestItem() == NULL)
					{
						SetCurrentTestItem(temp);
						temp->statues = statues5;
						
						UpOneModelData(index-1, R_ON_G_OFF, 5);
						
						timer_restart(&(temp->timer3));
						AddNumOfSongToList(index+34, 0);
						
						MotorMoveTo(MaxLocation, 1);
					}
				}
				/*提醒时间快到了*/
				else if( (tempvalue <= 60) && (tempvalue > 20) )
				{
					if(tempvalue < 30)
					{
						if((DspPaiDuiPage != GetGBCurrentPage()) && (GetCurrentTestItem() == NULL))
							DspPaiDuiPage(NULL);
					}
					
					if(TimeOut == timer_expired(&(temp->timer3)))
					{
						timer_restart(&(temp->timer3));
						AddNumOfSongToList(index-1, 0);
					}
				}
			}
			else if(temp->statues == statues4)
			{
				tempvalue = timer_surplus(&(temp->timer));
				if(0 == tempvalue)
				{
					timer_restart(&(temp->timer2));
					UpOneModelData(index-1, R_ON_G_OFF, 5);
					temp->statues = statues13;
					AddNumOfSongToList(index+10, 0);
				}
				/*倒计时小于20S，提示插入测试位*/
				else if(tempvalue < 20)
				{
					//空闲
					if(GetCurrentTestItem() == NULL)
					{
						SetCurrentTestItem(temp);
						temp->statues = statues6;
						
						UpOneModelData(index-1, R_ON_G_OFF, 5);
						
						timer_restart(&(temp->timer3));
						AddNumOfSongToList(index+34, 0);
						
						MotorMoveTo(MaxLocation, 1);
					}
				}
				if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
					AddNumOfSongToList(index+26, 0);
				}
			}
			else if((temp->statues == statues5) || (temp->statues == statues6))
			{
				/*进入超时阶段*/
				if(TimeOut == timer_expired(&(temp->timer)))
				{
					timer_restart(&(temp->timer2));
					
					if(statues5 == temp->statues)
					{
						SetCurrentTestItem(NULL);
						UpOneModelData(index-1, R_ON_G_OFF, 0);
						temp->statues = statues8;
						AddNumOfSongToList(index+10, 0);
					}
					else if(statues6 == temp->statues)
					{
						temp->statues = statues9;
						UpOneModelData(index-1, R_ON_G_OFF, 5);
						SetCurrentTestItem(NULL);
						AddNumOfSongToList(index+10, 0);
						AddNumOfSongToList(index+26, 2);
					}
				}
				/*提示放入卡槽准备测试*/
				else if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
					AddNumOfSongToList(index+34, 0);
				}
			}
			/*正在超时计时*/
			else if(temp->statues == statues8)
			{
				/*如果超时时，操作空闲*/
				if((GetMinWaitTime() > 80)&&(GetCurrentTestItem() == NULL))
				{
					UpOneModelData(index-1, R_ON_G_OFF, 5);
					temp->statues = statues10;
					SetCurrentTestItem(temp);
					timer_restart(&(temp->timer3));
					AddNumOfSongToList(index+34, 0);
				}
			}
			/*插入排队位，进入超时*/
			else if(temp->statues == statues9)
			{
				if(TimeOut == timer_expired(&(temp->timer3)))
				{
					//20S提示一次将卡插入排队位
					timer_restart(&(temp->timer3));
					
					AddNumOfSongToList(index+26, 0);
				}
			}
			/*正在超时时，有其他排队卡准备要测试了*/
			else if((temp->statues == statues10)||(temp->statues == statues11))
			{
				/*如果超时时，操作空闲*/
				if(GetMinWaitTime() < 60)
				{
					/*如果卡还没拔出来，则直接放弃此次操作*/
					if(temp->statues == statues10)
					{
						UpOneModelData(index-1, R_ON_G_OFF, 0);
						temp->statues = statues8;
						SetCurrentTestItem(NULL);
					}
					/*如果卡已拔，但是没插入卡槽，则提醒然后放弃此次操作*/
					else if(temp->statues == statues11)
					{
						UpOneModelData(index-1, R_ON_G_OFF, 5);
						temp->statues = statues12;
						timer_restart(&(temp->timer3));
						SetCurrentTestItem(NULL);
						AddNumOfSongToList(index+26, 0);
					}
				}
				else
				{
					if(TimeOut == timer_expired(&(temp->timer3)))
					{
						timer_restart(&(temp->timer3));
						AddNumOfSongToList(index+34, 0);
					}
				}
			}
			else if(temp->statues == statues12)
			{
				if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
					AddNumOfSongToList(index+26, 0);
				}
			}
			else if(temp->statues == statues13)
			{
				if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
					AddNumOfSongToList(index+26, 0);
				}
			}
		}
	}
}


