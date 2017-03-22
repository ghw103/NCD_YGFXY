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
					AddNumOfSongToList(index+22, 2);
				}
			}
			
			//如果正在倒计时
			if((timerIsStartted(&(temp->timer))) && (false == timerIsStartted(&(temp->timer2))))
			{
				tempvalue = timer_surplus(&(temp->timer));
				
				if(0 == tempvalue)
				{
					if(statues7 != temp->statues)
					{
						timer_restart(&(temp->timer2));				//启动超时计时器
						AddNumOfSongToList(index+38, 0);
						
						if(temp->statues == statues5)
							temp->statues = statues6;
					}
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
								UpOneModelData(index, R_OFF_G_ON, 0);
								temp->statues = statues4;
								timer_restart(&(temp->timer3));
								AddNumOfSongToList(index+30, 0);
							}
						}
						else
						{
							//如果插入排队位，切换到计时或者超时状态
							if(KEY_Pressed == GetKeyStatues(index))
							{
								temp->statues = statues5;
									
								UpOneModelData(index, R_ON_G_OFF, 0);
							}
							else if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));
										
								AddNumOfSongToList(index+22, 2);
							}
						}
					}
					//等待拔出排队位
					else if(temp->statues == statues3)
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
					}
					//等待插入卡槽
					else if(temp->statues == statues4)
					{
						if(GetCardState() == CardIN)
						{
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
				//时间>30秒
				else
				{
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
									
							AddNumOfSongToList(index+22, 2);
						}
					}
					//等待插入排队位
					else if(temp->statues == statues2)
					{
						//如果插入排队位，切换到计时或者超时状态
						if(KEY_Pressed == GetKeyStatues(index))
						{
							//超时的时候插入，继续超时计时
							temp->statues = statues5;
								
							UpOneModelData(index, R_ON_G_OFF, 0);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							timer_restart(&(temp->timer3));
									
							AddNumOfSongToList(index+22, 2);
						}
					}
					//等待拔出排队位
					else if(temp->statues == statues3)
					{
						;
					}
					//等待插入卡槽
					else if(temp->statues == statues4)
					{
						;
					}
					//倒计时过程中卡拔出
					else if(temp->statues == statues5)
					{
						//如果拔出排队位
						if(KEY_NoPressed == GetKeyStatues(index))
						{
							temp->statues = statues2;
							UpOneModelData(index, R_ON_G_OFF, 5);
							AddNumOfSongToList(index+22, 2);
							timer_restart(&(temp->timer3));
						}
						
						if((tempvalue <= 40) && (GetCurrentTestItem() == NULL))
						{
							if(false == CheckStrIsSame(paiduiActivityName, getCurrentActivityName(), strlen(paiduiActivityName)))
							{
								backToActivity(lunchActivityName);
								startActivity(createPaiDuiActivity, NULL);
							}
						}
					}
				}
			}
			
			//如果正在超时计时
			if(timerIsStartted(&(temp->timer2)))
			{
				tempvalue = GetMinWaitTime();
				
				//等待拔出卡槽
				if(temp->statues == statues1)
				{
					;
				}
				//等待插入排队位
				else if(temp->statues == statues2)
				{
					//有卡即将测试
					if(tempvalue <= 40)
					{
						//如果插入排队位，切换到计时或者超时状态
						if(KEY_Pressed == GetKeyStatues(index))
						{
							temp->statues = statues6;
								
							UpOneModelData(index, R_ON_G_OFF, 0);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							timer_restart(&(temp->timer3));
									
							AddNumOfSongToList(index+22, 2);
						}
					}
					else
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
							UpOneModelData(index, R_OFF_G_ON, 0);
							temp->statues = statues4;
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 0);
						}
					}
				}
				//等待拔出排队位
				else if(temp->statues == statues3)
				{
					//有卡即将测试
					if(tempvalue <= 40)
					{
						SetCurrentTestItem(NULL);
						UpOneModelData(index, R_ON_G_OFF, 0);
						temp->statues = statues6;
					}
					else
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
					}
				}
				//等待插入卡槽
				else if(temp->statues == statues4)
				{
					//有卡即将测试
					if(tempvalue <= 40)
					{
						SetCurrentTestItem(NULL);
						UpOneModelData(index, R_ON_G_OFF, 5);
						temp->statues = statues2;
						
						timer_restart(&(temp->timer3));		
						AddNumOfSongToList(index+22, 2);
					}
					else
					{
						if(GetCardState() == CardIN)
						{
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
				}
				//超时过程中
				else if(temp->statues == statues6)
				{
					//空闲
					if(tempvalue > 40)
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
