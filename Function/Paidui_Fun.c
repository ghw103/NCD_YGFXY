/***************************************************************************************************
*FileName: Paidui_Fun
*Description: 排队功能
*Author: xsx_kair
*Data: 2016年12月13日11:47:20
***************************************************************************************************/

/***************************************************************************************************/
/******************************************Header List********************************************/
/***************************************************************************************************/
#include	"Paidui_Fun.h"
#include	"OutModel_Fun.h"

#include	"CardLimit_Driver.h"

#include	"PlaySong_Task.h"

#include	"PaiDuiPage.h"
#include	"PreReadCardPage.h"

#include	"CardStatues_Data.h"
#include	"UI_Data.h"
#include	"MyTest_Data.h"
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/****************************************File Start*************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName: PaiDuiHandler
*Description: 排队流程控制
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2016年12月13日11:58:58
***************************************************************************************************/
void PaiDuiHandler(void)
{
	unsigned char index = 0;
	ItemData * temp = NULL;
	unsigned short tempvalue = 0;
	PageInfo * currentpage = NULL;
	
	for(index=0; index<PaiDuiWeiNum; index++)
	{
		temp = GetTestItemByIndex(index);
		
		if(temp)
		{
			//进入排队模式
			if(temp->statues == startpaidui)
			{
				temp->statues = statues1;
				
				UpOneModelData(index, R_ON_G_OFF, 5);
				//20S提示一次将卡插入排队位
				timer_set(&(temp->timer3), 10);
				AddNumOfSongToList(index+27, 0);
			}
			//等待拔出卡槽
			if(temp->statues == statues1)
			{
				if(!CardPinIn)
					temp->statues = statues2;

				else if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
							
					AddNumOfSongToList(index+27, 0);
				}
			}
			//等待插入卡槽
			else if(temp->statues == statues4)
			{
				if(CardPinIn)
				{
					UpOneModelData(index, R_OFF_G_ON, 0);
					temp->statues = statues7;
					PageAdvanceTo(DspPreReadCardPage, NULL);
				}
				else if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
							
					AddNumOfSongToList(index+35, 0);
				}
			}
			//等待插入排队位
			else if(temp->statues == statues2)
			{
				//如果插入排队位，切换到计时或者超时状态
				if(KEY_Pressed == GetKeyStatues(index))
				{
					//超时的时候插入，继续超时计时
					if(timerIsStartted(&(temp->timer2)))
						temp->statues = statues6;
					else
						temp->statues = statues5;
						
					UpOneModelData(index, R_ON_G_OFF, 0);
					
					//如果当前操作的不是自己，则说明自己只是被异常拔出，只需插入即可，不更改当前操作对象
					if(temp == GetCurrentTestItem())					
						SetCurrentTestItem(NULL);
				}
				else if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
							
					AddNumOfSongToList(index+27, 0);
				}
			}
			//等待拔出排队位
			else if(temp->statues == statues3)
			{
				//如果拔出排队位
				if(KEY_NoPressed == GetKeyStatues(index))
				{
					UpOneModelData(index, R_OFF_G_ON, 0);
					temp->statues = statues4;
				}
				else if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
							
					AddNumOfSongToList(index+35, 0);
				}
			}
			//倒计时过程中卡拔出
			else if((temp->statues == statues5) || (temp->statues == statues6))
			{
				//如果拔出排队位
				if(KEY_NoPressed == GetKeyStatues(index))
				{
					temp->statues = statues2;
					UpOneModelData(index, R_ON_G_OFF, 5);
					AddNumOfSongToList(index+27, 0);
					timer_restart(&(temp->timer3));
				}
			}

			//如果正在倒计时
			if((timerIsStartted(&(temp->timer))) && (false == timerIsStartted(&(temp->timer2))))
			{
				tempvalue = timer_surplus(&(temp->timer));
				
				if((0 == tempvalue)&&(temp->statues != statues7))
				{
					UpOneModelData(index, R_ON_G_OFF, 0);
					timer_restart(&(temp->timer2));				//启动超时计时器
					AddNumOfSongToList(index+11, 0);
				}
				else if(tempvalue <= 30)
				{
					//排队完成
					if(temp->statues == statues1)
					{
						UpOneModelData(index, R_OFF_G_ON, 0);
						temp->statues = statues7;
						PageAdvanceTo(DspPreReadCardPage, NULL);
					}
					//插入卡槽，结束排队
					else if(temp->statues == statues2)
					{
						UpOneModelData(index, R_OFF_G_ON, 0);
						temp->statues = statues4;
						timer_restart(&(temp->timer3));
						AddNumOfSongToList(index+35, 0);
					}
					//如果正在倒计时，则提示拔出排队位，
					else if(temp->statues == statues5)
					{
						//如果空闲
						if(GetCurrentTestItem() == NULL)
						{
							SetCurrentTestItem(temp);
							UpOneModelData(index, R_ON_G_OFF, 5);
							temp->statues = statues3;
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+35, 0);
						}
					}
				}
				else if(tempvalue <= 60)
				{
					//如果正在倒计时，则切换界面到排队界面
					if((temp->statues == statues5) && (GetCurrentTestItem() == NULL))
					{
						if(My_Pass == GetCurrentPage(&currentpage))
						{
							if(currentpage->CurrentPage != DspPaiDuiPage)
							{
								PageResetToOrigin(NoDisplayPage);
								PageAdvanceTo(DspPaiDuiPage, NULL);
							}
						}
					}
				}
			}
			
			//如果正在超时计时
			if(timerIsStartted(&(temp->timer2)))
			{
				//如果有卡倒计时小于40S，则中断此超时卡的测试
				if(GetMinWaitTime() < 40)
				{
					if(temp->statues == statues3)
					{
						SetCurrentTestItem(NULL);
						UpOneModelData(index, R_ON_G_OFF, 0);
						temp->statues = statues6;
					}
				}
				
				/*如果超时时，操作空闲*/
				if((temp->statues == statues6) && (GetMinWaitTime() > 60) && (GetCurrentTestItem() == NULL))
				{
					SetCurrentTestItem(temp);
					UpOneModelData(index, R_ON_G_OFF, 5);
					temp->statues = statues3;
					timer_restart(&(temp->timer3));
					AddNumOfSongToList(index+35, 0);
				}
				
			}
		}
		
	}
}

/****************************************end of file************************************************/
