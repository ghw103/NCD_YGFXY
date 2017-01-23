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
#include	"WaittingCardPage.h"
#include	"Motor_Data.h"
#include	"CardStatues_Data.h"
#include	"UI_Data.h"
#include	"MyTest_Data.h"
#include	"MyTools.h"

#include	<string.h>
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
	
	for(index=0; index<PaiDuiWeiNum; index++)
	{
		temp = GetTestItemByIndex(index);
		
		if((temp) && (temp->statues != statuesNull))
		{
			//进入排队模式
			if(temp->statues == startpaidui)
			{
				if(MaxLocation == GetGB_MotorLocation())
				{
					temp->statues = statues1;
				
					UpOneModelData(index, R_ON_G_OFF, 5);
					//20S提示一次将卡插入排队位
					timer_set(&(temp->timer3), 10);
					AddNumOfSongToList(index+22, 0);
				}
			}
			//等待拔出卡槽
			if(temp->statues == statues1)
			{
				//如果从卡槽拔出，则将当前操作卡置为空，可以进行其他操作
				if(GetCardState() == NoCard)
				{
					temp->statues = statues2;
					SetCurrentTestItem(NULL);
				}
				else if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
							
					AddNumOfSongToList(index+22, 0);
				}
			}
			//等待插入排队位
			else if(temp->statues == statues2)
			{
				//如果插入排队位，切换到计时或者超时状态
				if(KEY_Pressed == GetKeyStatues(index))
				{
					//停止播放语音
					stopPlay();
					
					//超时的时候插入，继续超时计时
					if(timerIsStartted(&(temp->timer2)))
						temp->statues = statues6;
					else
						temp->statues = statues5;
						
					UpOneModelData(index, R_ON_G_OFF, 0);
				}
				else if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
							
					AddNumOfSongToList(index+22, 0);
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
							
					AddNumOfSongToList(index+30, 0);
				}
			}
			//等待插入卡槽
			else if(temp->statues == statues4)
			{
				if(GetCardState() == CardIN)
				{
					//停止播放语音
					stopPlay();
					
					UpOneModelData(index, R_OFF_G_ON, 0);
					temp->statues = statues7;
					
					startActivity(createWaittingCardActivity, NULL);
				}
				else if(TimeOut == timer_expired(&(temp->timer3)))
				{
					timer_restart(&(temp->timer3));
							
					AddNumOfSongToList(index+30, 5);
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
					AddNumOfSongToList(index+22, 0);
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
					AddNumOfSongToList(index+38, 0);
					
					if(temp->statues == statues5)
						temp->statues = statues6;
				}
				else if(tempvalue <= 30)
				{
					//排队完成
					if(temp->statues == statues1)
					{
						UpOneModelData(index, R_OFF_G_ON, 0);
						temp->statues = statues7;

						startActivity(createWaittingCardActivity, NULL);
					}
					//等待插入排队位，但是如果时间小于30秒，则转为状态4，提示插入测试位测试，不在插入排队位
					else if(temp->statues == statues2)
					{
						if(NULL == GetCurrentTestItem())
						{
							UpOneModelData(index, R_OFF_G_ON, 0);
							temp->statues = statues4;
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 0);
						}
					}
					//如果正在倒计时，则提示拔出排队位，
					else if(temp->statues == statues5)
					{
						//如果空闲
						if(GetCurrentTestItem() == NULL)
						{
							//如果卡槽有卡，提示清空
							if(GetCardState() == CardIN)
							{
								if(TimeOut == timer_expired(&(temp->timer3)))
								{
									timer_restart(&(temp->timer3));
											
									AddNumOfSongToList(46, 0);					//提示清空卡槽
								}
							}
							else
							{
								SetCurrentTestItem(temp);
								UpOneModelData(index, R_ON_G_OFF, 5);
								temp->statues = statues3;
								timer_restart(&(temp->timer3));
								AddNumOfSongToList(index+30, 0);
							}
							
						}
					}
				}
				else if(tempvalue <= 60)
				{
					//如果正在倒计时，则切换界面到排队界面
					if((temp->statues == statues5) && (GetCurrentTestItem() == NULL))
					{
						if(false == CheckStrIsSame(paiduiActivityName, getCurrentActivityName(), strlen(paiduiActivityName)))
						{
							backToActivity(lunchActivityName);
							startActivity(createPaiDuiActivity, NULL);
						}
					}
				}
			}
			
			//如果正在超时计时
			if(timerIsStartted(&(temp->timer2)))
			{
				//如果有卡倒计时小于60S，则中断此超时卡的测试
				if(GetMinWaitTime() < 60)
				{
					if(temp->statues == statues3)
					{
						SetCurrentTestItem(NULL);
						UpOneModelData(index, R_ON_G_OFF, 0);
						temp->statues = statues6;
					}
				}
				else
				{
					if(temp->statues == statues2)
					{
						if(NULL == GetCurrentTestItem())
						{
							UpOneModelData(index, R_OFF_G_ON, 0);
							temp->statues = statues4;
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 0);
						}
					}
					if(temp->statues == statues6)
					{
						if(NULL == GetCurrentTestItem())
						{
							//如果卡槽有卡，提示清空
							if(GetCardState() == CardIN)
							{
								if(TimeOut == timer_expired(&(temp->timer3)))
								{
									timer_restart(&(temp->timer3));
											
									AddNumOfSongToList(46, 0);					//提示清空卡槽
								}
							}
							else
							{
								SetCurrentTestItem(temp);
								UpOneModelData(index, R_ON_G_OFF, 5);
								temp->statues = statues3;
								timer_restart(&(temp->timer3));
								AddNumOfSongToList(index+30, 0);
							}
						}
					}
				}
			}
		}
		
	}
}

/****************************************end of file************************************************/
