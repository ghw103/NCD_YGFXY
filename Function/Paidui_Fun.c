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
#include	"System_Data.h"

#include	"CardLimit_Driver.h"

#include	"PlaySong_Task.h"

#include	"PaiDuiPage.h"
#include	"PreReadCardPage.h"
#include	"Motor_Data.h"
#include	"CardStatues_Data.h"
#include	"UI_Data.h"
#include	"MyTest_Data.h"
#include	"MyTools.h"
#include	"Timer_Data.h"

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
		
		if((temp) && (temp->statues >= status_start) && (temp->statues <= status_timeup) && (Connect_Ok == getPaiduiModuleStatus()))
		{
			//进入排队模式
			if(temp->statues == status_start)
			{
				if(MaxLocation == GetGB_MotorLocation())
				{
					temp->statues = status_outcard;
				
					UpOneModelData(index, R_OFF_G_ON, R_OFF_G_OFF, 5);
					//20S提示一次将卡插入排队位
					timer_set(&(temp->timer3), 10);
					AddNumOfSongToList(index+22, 0);
				}
			}
			
			//如果正在倒计时
			if(isInTimeOutStatus(temp) == false)
			{
				tempvalue = timer_surplus(&(temp->timer));
				
				if(0 == tempvalue)
				{
					timer_restart(&(temp->timer2));				//启动超时计时器
					
					//等待插入排队位
					if(temp->statues == status_in_n)
					{
						temp->statues = status_in_o;
						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
						AddNumOfSongToList(index+38, 0);
						
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);
					}
					//等待拔出排队位
					else if(temp->statues == status_out_n)
					{
						temp->statues = status_timeup;
						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
						AddNumOfSongToList(index+38, 0);
						
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);
					}
					//等待插入卡槽
					else if(temp->statues == status_incard_n)
					{
						temp->statues = status_in_o;
						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
						AddNumOfSongToList(index+38, 0);
						
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);
					}
					//倒计时中
					else if(temp->statues == status_timedown)
					{
						temp->statues = status_timeup;
						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
						AddNumOfSongToList(index+38, 0);
						
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);
					}
					else if(temp->statues == status_prereadagain_n)
					{
						temp->statues = status_prereadagain_o;
						timer_restart(&(temp->timer2));				//启动超时计时器
						AddNumOfSongToList(index+38, 0);
					}
				}
				else if(tempvalue <= 30)
				{
					//等待从卡槽拔出
					if(temp->statues == status_outcard)
					{
						UpOneModelData(index, R_OFF_G_ON, R_OFF_G_OFF, 0);
						temp->statues = status_prereadagain_n;

						startActivity(createPreReadCardActivity, NULL);
					}
					//等待插入排队位
					else if(temp->statues == status_in_n)
					{
						if(GetCurrentTestItem() == NULL)									//如果空闲
						{
							if(GetCardState() == CardIN)									//如果卡槽有卡，提示清空
							{
								if(TimeOut == timer_expired(&(temp->timer3)))
								{
									timer_restart(&(temp->timer3));
									AddNumOfSongToList(46, 2);								//提示清空卡槽
								}
							}
							else
							{
								SetCurrentTestItem(temp);
								UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
								temp->statues = status_incard_n;
								timer_restart(&(temp->timer3));
								AddNumOfSongToList(index+30, 0);
							}
						}
						else
						{
							//如果插入排队位，切换到计时或者超时状态
							if(KEY_Pressed == GetKeyStatues(index))
							{
								temp->statues = status_timedown;
								UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
							}
							else if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));
								AddNumOfSongToList(index+22, 0);
							}
						}
					}
					//等待拔出排队位
					else if(temp->statues == status_out_n)
					{
						//如果卡槽有卡，提示清空
						if(GetCardState() == CardIN)
						{
							if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));		
								AddNumOfSongToList(46, 2);					//提示清空卡槽
							}
						}
						else
						{
							//如果拔出排队位
							if(KEY_NoPressed == GetKeyStatues(index))
							{
								UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
								temp->statues = status_incard_n;
							}
							else if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));			
								AddNumOfSongToList(index+30, 0);
							}
						}
					}
					//等待插入卡槽
					else if(temp->statues == status_incard_n)
					{
						if(GetCardState() == CardIN)
						{
							UpOneModelData(index, R_OFF_G_ON, R_OFF_G_OFF, 0);
							temp->statues = status_prereadagain_n;
							startActivity(createPreReadCardActivity, NULL);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 5);
						}
					}
					//倒计时中
					else if(temp->statues == status_timedown)
					{
						if(GetCurrentTestItem() == NULL)										//如果空闲
						{
							if(GetCardState() == CardIN)										//如果卡槽有卡，提示清空
							{
								if(TimeOut == timer_expired(&(temp->timer3)))
								{
									timer_restart(&(temp->timer3));		
									AddNumOfSongToList(46, 2);					//提示清空卡槽
								}
							}
							else
							{
								SetCurrentTestItem(temp);
								UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
								temp->statues = status_out_n;
								timer_restart(&(temp->timer3));
								AddNumOfSongToList(index+30, 0);
							}
						}
					}
				}
				//时间>30秒
				else
				{
					//等待从卡槽拔出
					if(temp->statues == status_outcard)
					{
						//如果从卡槽拔出，则将当前操作卡置为空，可以进行其他操作
						if(GetCardState() == NoCard)
						{
							temp->statues = status_in_n;
							SetCurrentTestItem(NULL);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							timer_restart(&(temp->timer3));
									
							AddNumOfSongToList(index+22, 0);
						}
					}
					//等待插入排队位
					else if(temp->statues == status_in_n)
					{
						//如果插入排队位，切换到计时或者超时状态
						if(KEY_Pressed == GetKeyStatues(index))
						{
							//进入倒计时状态
							temp->statues = status_timedown;
								
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							timer_restart(&(temp->timer3));
									
							AddNumOfSongToList(index+22, 0);
						}
					}
					//等待拔出排队位
					else if(temp->statues == status_out_n)
					{
						;
					}
					//等待插入卡槽
					else if(temp->statues == status_incard_n)
					{
						;
					}
					//倒计时中
					else if(temp->statues == status_timedown)
					{
						//如果拔出排队位
						if(KEY_NoPressed == GetKeyStatues(index))
						{
							temp->statues = status_in_n;
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
							AddNumOfSongToList(index+22, 0);
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
			//超时状态
			else
			{
				tempvalue = GetMinWaitTime();
				
				//等待插入排队位
				if(temp->statues == status_in_o)
				{
					//有卡即将测试
					if((tempvalue > 40) && (NULL == GetCurrentTestItem()))
					{
						//如果卡槽有卡，提示清空
						if(GetCardState() == CardIN)
						{
							if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));		
								AddNumOfSongToList(46, 2);					//提示清空卡槽
							}
						}
						else
						{
							SetCurrentTestItem(temp);
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
							temp->statues = status_incard_o;
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 0);
						}
					}
					else
					{
						//如果插入排队位，切换到计时或者超时状态
						if(KEY_Pressed == GetKeyStatues(index))
						{
							temp->statues = status_timeup;
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							timer_restart(&(temp->timer3));		
							AddNumOfSongToList(index+22, 0);
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
						}
					}
				}
				//等待拔出排队位
				else if(temp->statues == status_out_o)
				{
					//有卡即将测试
					if(tempvalue <= 40)
					{
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);
						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_OFF, 0);
						temp->statues = status_timeup;
					}
					else
					{
						//如果卡槽有卡，提示清空
						if(GetCardState() == CardIN)
						{
							if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));		
								AddNumOfSongToList(46, 2);					//提示清空卡槽
							}
						}
						else
						{
							//如果拔出排队位
							if(KEY_NoPressed == GetKeyStatues(index))
							{
								UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
								temp->statues = status_incard_o;
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
				else if(temp->statues == status_incard_o)
				{
					//有卡即将测试
					if(tempvalue <= 40)
					{
						if(temp == GetCurrentTestItem())
							SetCurrentTestItem(NULL);

						UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
						temp->statues = status_in_o;
						
						timer_restart(&(temp->timer3));		
						AddNumOfSongToList(index+22, 0);
					}
					else
					{
						if(GetCardState() == CardIN)
						{
							UpOneModelData(index, R_OFF_G_ON, R_OFF_G_OFF, 0);
							temp->statues = status_prereadagain_o;
							
							startActivity(createPreReadCardActivity, NULL);
						}
						else if(TimeOut == timer_expired(&(temp->timer3)))
						{
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 5);
						}
					}
				}
				//超时中
				else if(temp->statues == status_timeup)
				{
					if((tempvalue > 40) && (NULL == GetCurrentTestItem()))
					{
						//如果卡槽有卡，提示清空
						if(GetCardState() == CardIN)
						{
							if(TimeOut == timer_expired(&(temp->timer3)))
							{
								timer_restart(&(temp->timer3));	
								AddNumOfSongToList(46, 2);					//提示清空卡槽
							}
						}
						else
						{
							SetCurrentTestItem(temp);
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 1);
							temp->statues = status_out_o;
							timer_restart(&(temp->timer3));
							AddNumOfSongToList(index+30, 0);
						}
					}
					else
					{
						//如果拔出排队位
						if(KEY_NoPressed == GetKeyStatues(index))
						{
							UpOneModelData(index, R_ON_G_OFF, R_OFF_G_ON, 5);
							temp->statues = status_in_o;
							timer_restart(&(temp->timer3));		
							AddNumOfSongToList(index+22, 0);
						}
					}
				}
			}
		}
	}
	
	if((IsPaiDuiTestting() == true) && (Connect_Error == getPaiduiModuleStatus()))
	{
		//如果当前不是排队界面
		if(false == CheckStrIsSame(paiduiActivityName, getCurrentActivityName(), strlen(paiduiActivityName)))
		{
			//如果当前没在测试
			if(GetCurrentTestItem() == NULL)
			{
				backToActivity(lunchActivityName);
				startActivity(createPaiDuiActivity, NULL);
			}
		}
	}
}

/****************************************end of file************************************************/
