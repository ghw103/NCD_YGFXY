/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"PaiDuiPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"

#include	"SystemSetPage.h"
#include	"MyMem.h"
#include	"TimeDownNorPage.h"
#include	"PreReadCardPage.h"
#include	"TM1623_Driver.h"
#include	"Motor_Data.h"
#include	"Motor_Fun.h"
#include	"CardStatues_Data.h"
#include	"PlaySong_Task.h"
#include	"CardLimit_Driver.h"
#include	"SampleIDPage.h"
#include	"MyTools.h"
#include	"MyTest_Data.h"
#include	"LunchPage.h"
#include	"System_Data.h"

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
static void activityStart(void);
static void activityInput(unsigned char *pbuf , unsigned short len);
static void activityFresh(void);
static void activityHide(void);
static void activityResume(void);
static void activityDestroy(void);
static MyState_TypeDef activityBufferMalloc(void);
static void activityBufferFree(void);
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
MyState_TypeDef createPaiDuiActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, paiduiActivityName, activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_PaiDuiPageBuffer)
	{
		//进入界面先禁止插卡自动新建测试功能
		timer_set(&(S_PaiDuiPageBuffer->timer0), 65535);
		
		//如果排队模块失联，则提示
		timer_set(&(S_PaiDuiPageBuffer->timer1), 1);
	}
	
	SelectPage(93);
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
	if(S_PaiDuiPageBuffer)
	{
		/*命令*/
		S_PaiDuiPageBuffer->lcdinput[0] = pbuf[4];
		S_PaiDuiPageBuffer->lcdinput[0] = (S_PaiDuiPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_PaiDuiPageBuffer->lcdinput[0] == 0x1500)
		{
			//测试中，不允许返回
			if(NULL != GetCurrentTestItem())
			{
				SendKeyCode(4);
				AddNumOfSongToList(21, 0);
			}
			//即将测试，不允许返回
			else if(GetMinWaitTime() < 40)
			{
				SendKeyCode(3);
				AddNumOfSongToList(20, 0);
			}
			else if(true == isSomePaiduiInOutTimeStatus())
			{
				SendKeyCode(3);
				AddNumOfSongToList(20, 0);
			}
			else
			{
				//页面正在刷新数据，忙
				while(S_PaiDuiPageBuffer->pageisbusy)
				{
					vTaskDelay(100 / portTICK_RATE_MS);
				}
				
				backToActivity(lunchActivityName);
			}
		}
		//继续测试
		else if(S_PaiDuiPageBuffer->lcdinput[0] == 0x1501)
		{
			S_PaiDuiPageBuffer->error = CreateANewTest(PaiDuiTestType);
			//创建成功
			if(Error_OK == S_PaiDuiPageBuffer->error)
			{
				startActivity(createSampleActivity, NULL);
			}
			//排队位置满，不允许
			else if(Error_PaiduiFull == S_PaiDuiPageBuffer->error)
			{
				SendKeyCode(2);
				AddNumOfSongToList(19, 0);
			}
			//创建失败
			else if(Error_Mem == S_PaiDuiPageBuffer->error)
			{
				SendKeyCode(1);
				AddNumOfSongToList(7, 0);
			}
			//有卡即将测试
			else if(Error_PaiDuiBusy == S_PaiDuiPageBuffer->error)
			{
				SendKeyCode(3);
				AddNumOfSongToList(20, 0);
			}
			//测试中禁止添加
			else if(Error_PaiduiTesting == S_PaiDuiPageBuffer->error)
			{
				SendKeyCode(4);
				AddNumOfSongToList(21, 0);
			}
			//排队模块失联
			else if(Error_PaiduiDisconnect == S_PaiDuiPageBuffer->error)
			{
				SendKeyCode(5);
				AddNumOfSongToList(58, 0);
			}
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
	unsigned char index = 0;
	
	if(S_PaiDuiPageBuffer)
	{
		S_PaiDuiPageBuffer->count2++;
		
		//界面忙
		S_PaiDuiPageBuffer->pageisbusy = true;

		//检测一次排队模块是否正常
		if(Connect_Error == getPaiduiModuleStatus())
		{
			if(TimeOut == timer_expired(&(S_PaiDuiPageBuffer->timer1)))
			{
				timer_set(&(S_PaiDuiPageBuffer->timer1), 5);
				SendKeyCode(6);
				AddNumOfSongToList(57, 2);
			}
		}
			
		
		//500ms刷新一次界面
		if(S_PaiDuiPageBuffer->count2 % 5 == 0)
		{
			//如果当前功能处于禁止状态，且电机位置处于最大行程，且卡槽无卡，且最近的卡大于 则启用插卡自动创建功能
			if((S_PaiDuiPageBuffer->timer0.interval == 65535) && (MaxLocation == GetGB_MotorLocation()) && (!CardPinIn))
			{
				timer_set(&(S_PaiDuiPageBuffer->timer0), 1);
			}

			//如果当前空闲，且扫描时间到，则检测是否插卡了
			if(TimeOut == timer_expired(&(S_PaiDuiPageBuffer->timer0)))
			{
				//如果当前空闲,且已经插卡
				if((CardPinIn) && (NULL == GetCurrentTestItem()))
				{
					S_PaiDuiPageBuffer->error = CreateANewTest(PaiDuiTestType);
					//创建成功
					if(Error_OK == S_PaiDuiPageBuffer->error)
					{
						vTaskDelay(1000 / portTICK_RATE_MS);
						//创建成功，则使电机远离，防止用户拔卡
						MotorMoveTo(1000, 1);			
						startActivity(createSampleActivity, NULL);		
						return;
					}
					//排队位置满，不允许
					else if(Error_PaiduiFull == S_PaiDuiPageBuffer->error)
					{
						SendKeyCode(2);
						AddNumOfSongToList(19, 2);
						timer_set(&(S_PaiDuiPageBuffer->timer0), 65535);
					}
					//创建失败
					else if(Error_Mem == S_PaiDuiPageBuffer->error)
					{
						SendKeyCode(1);
						AddNumOfSongToList(7, 0);
						timer_set(&(S_PaiDuiPageBuffer->timer0), 65535);
					}
					//有卡即将测试
					else if(Error_PaiDuiBusy == S_PaiDuiPageBuffer->error)
					{
						SendKeyCode(3);
						AddNumOfSongToList(20, 0);
						timer_set(&(S_PaiDuiPageBuffer->timer0), 65535);
					}
					//测试中禁止添加
					else if(Error_PaiduiTesting == S_PaiDuiPageBuffer->error)
					{
						SendKeyCode(4);
						AddNumOfSongToList(21, 0);
						timer_set(&(S_PaiDuiPageBuffer->timer0), 65535);
					}
					//排队模块失联
					else if(Error_PaiduiDisconnect == S_PaiDuiPageBuffer->error)
					{
						SendKeyCode(5);
						AddNumOfSongToList(58, 0);
					}
				}
			}
			//更新倒计时数据
			for(index=0; index<PaiDuiWeiNum; index++)
			{
				S_PaiDuiPageBuffer->tempd2 = GetTestItemByIndex(index);
				
				if(S_PaiDuiPageBuffer->tempd2)
				{
					DspNum(0x1506+index, S_PaiDuiPageBuffer->tempd2->statues, 2);
					//超时
					if(isInTimeOutStatus(S_PaiDuiPageBuffer->tempd2))
					{
						S_PaiDuiPageBuffer->tempvalue1 = timer_Count(&(S_PaiDuiPageBuffer->tempd2->timer2));
						if(S_PaiDuiPageBuffer->tempvalue1 > 60)
							sprintf(S_PaiDuiPageBuffer->buf, "%02dM", S_PaiDuiPageBuffer->tempvalue1/60);
						else
							sprintf(S_PaiDuiPageBuffer->buf, "%02dS", S_PaiDuiPageBuffer->tempvalue1);
		
						WriteVarIcoNum(0x1510+index*16, 50);
					}
					else
					{
						S_PaiDuiPageBuffer->tempvalue1 = timer_surplus(&(S_PaiDuiPageBuffer->tempd2->timer));
						if(S_PaiDuiPageBuffer->tempvalue1 > 60)
							sprintf(S_PaiDuiPageBuffer->buf, "%02dM", S_PaiDuiPageBuffer->tempvalue1/60);
						else
							sprintf(S_PaiDuiPageBuffer->buf, "%02dS", S_PaiDuiPageBuffer->tempvalue1);							
						
						S_PaiDuiPageBuffer->tempvalue = S_PaiDuiPageBuffer->tempd2->testdata.temperweima.CardWaitTime*60 - S_PaiDuiPageBuffer->tempvalue1;
						S_PaiDuiPageBuffer->tempvalue /= S_PaiDuiPageBuffer->tempd2->testdata.temperweima.CardWaitTime*60;
						S_PaiDuiPageBuffer->tempvalue *= 50;

						WriteVarIcoNum(0x1510+index*16, (unsigned short)(S_PaiDuiPageBuffer->tempvalue));
					}
					
					DisText(0x1610+index*0x08, S_PaiDuiPageBuffer->buf, 10);
					
					if((S_PaiDuiPageBuffer->tempd2->statues == status_timedown) || (S_PaiDuiPageBuffer->tempd2->statues == status_timeup)){
						BasicPic(0x1590+index*0x10, 1, 138, 10+85*S_PaiDuiPageBuffer->tempd2->testdata.temperweima.itemConstData.icoIndex, 10, 10+85*S_PaiDuiPageBuffer->tempd2->testdata.temperweima.itemConstData.icoIndex+75, 10+285, 91+index*110, 190);
					}
					else
					{
						//检测卡图标闪烁
						if((S_PaiDuiPageBuffer->count % 2) == 0)
							BasicPic(0x1590+index*0x10, 1, 138, 10+85*S_PaiDuiPageBuffer->tempd2->testdata.temperweima.itemConstData.icoIndex, 10, 10+85*S_PaiDuiPageBuffer->tempd2->testdata.temperweima.itemConstData.icoIndex+75, 10+285, 91+index*110, 190);
						else
							BasicPic(0x1590+index*0x10, 0, 138, 10+85*S_PaiDuiPageBuffer->tempd2->testdata.temperweima.itemConstData.icoIndex, 10, 10+85*S_PaiDuiPageBuffer->tempd2->testdata.temperweima.itemConstData.icoIndex+75, 10+285, 91+index*110, 190);
					}
				}
				else
				{
					//清除倒计时时间
					ClearText(0x1610+index*0x08);
					ClearText(0x1650+index*0x08);
					
					//显示卡凹槽
					S_PaiDuiPageBuffer->myico.ICO_ID = 37;
					S_PaiDuiPageBuffer->myico.X = 69+index*119;
					S_PaiDuiPageBuffer->myico.Y = 135;
					BasicUI(0x1590+index*0x10 ,0x1907 , 0, &(S_PaiDuiPageBuffer->myico) , sizeof(Basic_ICO));
					
					//时间进度条显示0
					WriteVarIcoNum(0x1510+index*16, 0);
				}
			}
			
			S_PaiDuiPageBuffer->count++;
			if(S_PaiDuiPageBuffer->count > 65535)
				S_PaiDuiPageBuffer->count = 0;
		}
		
		//界面空闲
		S_PaiDuiPageBuffer->pageisbusy = false;
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
	//清除当前页面的告警弹出框
	SendKeyCode(16);
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
	if(S_PaiDuiPageBuffer)
	{
		//进入界面先禁止插卡自动新建测试功能，通过设置超大检测时间
		timer_set(&(S_PaiDuiPageBuffer->timer0), 65535);
	}
	
	SelectPage(93);
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
	//清除当前页面的告警弹出框
	SendKeyCode(16);
	
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
	if(NULL == S_PaiDuiPageBuffer)
	{
		S_PaiDuiPageBuffer = MyMalloc(sizeof(PaiDuiPageBuffer));
		
		if(S_PaiDuiPageBuffer)
		{
			memset(S_PaiDuiPageBuffer, 0, sizeof(PaiDuiPageBuffer));
	
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
	MyFree(S_PaiDuiPageBuffer);
	S_PaiDuiPageBuffer = NULL;
}


