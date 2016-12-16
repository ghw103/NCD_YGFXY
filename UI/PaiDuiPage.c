/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"PaiDuiPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"SystemSetPage.h"
#include	"MyMem.h"
#include	"TimeDownNorPage.h"
#include	"PreReadCardPage.h"
#include	"TM1623_Driver.h"

#include	"Motor_Fun.h"
#include	"CardStatues_Data.h"
#include	"PlaySong_Task.h"
#include	"CardLimit_Driver.h"
#include	"SampleIDPage.h"

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


static void Input(unsigned char *pbuf , unsigned short len)
{
	if(S_PaiDuiPageBuffer)
	{
		/*命令*/
		S_PaiDuiPageBuffer->lcdinput[0] = pbuf[4];
		S_PaiDuiPageBuffer->lcdinput[0] = (S_PaiDuiPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*返回*/
		if(S_PaiDuiPageBuffer->lcdinput[0] == 0x1600)
		{
			//测试中，不允许返回
			if(NULL != GetCurrentTestItem())
			{
				SendKeyCode(4);
			}
			//即将测试，不允许返回
			else if(GetMinWaitTime() < 60)
			{
				SendKeyCode(3);
				AddNumOfSongToList(61, 0);
			}
			else
			{
				//页面正在刷新数据，忙
				while(S_PaiDuiPageBuffer->pageisbusy)
				{
					vTaskDelay(100 / portTICK_RATE_MS);
				}
				
				PageBufferFree();
				PageResetToOrigin(DisplayPage);
			}
		}
		//继续测试
		else if(S_PaiDuiPageBuffer->lcdinput[0] == 0x1601)
		{
			S_PaiDuiPageBuffer->error = CreateANewTest(PaiDuiTestType);
			//创建成功
			if(Error_OK == S_PaiDuiPageBuffer->error)
			{
				PageBufferFree();
				PageAdvanceTo(DspSampleIDPage, NULL);
			}
			//排队位置满，不允许
			else if(Error_PaiduiFull == S_PaiDuiPageBuffer->error)
			{
				SendKeyCode(2);
				AddNumOfSongToList(61, 0);
			}
			//创建失败
			else if(Error_Mem == S_PaiDuiPageBuffer->error)
			{
				SendKeyCode(1);
				AddNumOfSongToList(41, 0);
			}
			//有卡即将测试
			else if(Error_PaiDuiBusy == S_PaiDuiPageBuffer->error)
			{
				SendKeyCode(3);
				AddNumOfSongToList(61, 0);
			}
			//测试中禁止添加
			else if(Error_PaiduiTesting == S_PaiDuiPageBuffer->error)
			{
				SendKeyCode(4);
				AddNumOfSongToList(61, 0);
			}
		}
	}
}

static void PageUpDate(void)
{
	unsigned char index = 0;
	
	if(S_PaiDuiPageBuffer)
	{
		S_PaiDuiPageBuffer->count2++;
		
		//界面忙
		S_PaiDuiPageBuffer->pageisbusy = true;

		
		//500ms刷新一次界面
		if(S_PaiDuiPageBuffer->count2 % 50 == 0)
		{
			//插卡
			if(CardPinIn)
			{
				//如果空闲，说明插入的卡不是排队中的卡，则创建新排队
				if(NULL == GetCurrentTestItem())
				{
					if(TimeOut == timer_expired(&(S_PaiDuiPageBuffer->timer0)))
					{
						S_PaiDuiPageBuffer->error = CreateANewTest(PaiDuiTestType);
						//创建成功
						if(Error_OK == S_PaiDuiPageBuffer->error)
						{
							PageAdvanceTo(DspSampleIDPage, NULL);
								
							return;
						}
						//排队位置满，不允许
						else if(Error_PaiduiFull == S_PaiDuiPageBuffer->error)
						{
							SendKeyCode(2);
							AddNumOfSongToList(61, 0);
						}
						//创建失败
						else if(Error_Mem == S_PaiDuiPageBuffer->error)
						{
							SendKeyCode(1);
							AddNumOfSongToList(41, 0);
						}
						//有卡即将测试
						else if(Error_PaiDuiBusy == S_PaiDuiPageBuffer->error)
						{
							SendKeyCode(3);
							AddNumOfSongToList(61, 0);
						}
						//测试中禁止添加
						else if(Error_PaiduiTesting == S_PaiDuiPageBuffer->error)
						{
							SendKeyCode(4);
							AddNumOfSongToList(61, 0);
						}
						
						//定时器设置长时间，表明不在对此卡创建，需要重新拔插才行
						timer_set(&(S_PaiDuiPageBuffer->timer0), 10000);
					}
				}
				//如果有卡，但是排队忙，则说明插入的卡是排队的卡，
				else
					timer_set(&(S_PaiDuiPageBuffer->timer0), 1);
			}
			//如果无卡，则重置定时器
			else
				timer_set(&(S_PaiDuiPageBuffer->timer0), 1);
		
			//更新倒计时数据
			for(index=0; index<PaiDuiWeiNum; index++)
			{
				S_PaiDuiPageBuffer->tempd2 = GetTestItemByIndex(index);
				
				if(S_PaiDuiPageBuffer->tempd2)
				{
					//超时
					if(timerIsStartted(&(S_PaiDuiPageBuffer->tempd2->timer2)))
					{
						S_PaiDuiPageBuffer->tempvalue1 = timer_Count(&(S_PaiDuiPageBuffer->tempd2->timer2));
						sprintf(S_PaiDuiPageBuffer->buf, "%d:%d", S_PaiDuiPageBuffer->tempvalue1/60, S_PaiDuiPageBuffer->tempvalue1%60);
						DisText(0x1650+index*0x08, S_PaiDuiPageBuffer->buf, 10);
							
						memset(S_PaiDuiPageBuffer->buf, 0, 10);
						DisText(0x1610+index*0x08, S_PaiDuiPageBuffer->buf, 10);
								
						BasicPic(0x1510+index*16, 0, 100, 15, 0, 22, 570, 55 , 0);
					}
					else
					{
						S_PaiDuiPageBuffer->tempvalue1 = timer_surplus(&(S_PaiDuiPageBuffer->tempd2->timer));
						sprintf(S_PaiDuiPageBuffer->buf, "%d:%d", S_PaiDuiPageBuffer->tempvalue1/60, S_PaiDuiPageBuffer->tempvalue1%60);
						DisText(0x1610+index*0x08, S_PaiDuiPageBuffer->buf, 10);
							
						memset(S_PaiDuiPageBuffer->buf, 0, 10);
						DisText(0x1650+index*0x08, S_PaiDuiPageBuffer->buf, 10);
							
						S_PaiDuiPageBuffer->tempvalue = S_PaiDuiPageBuffer->tempvalue1;
						S_PaiDuiPageBuffer->tempvalue /= S_PaiDuiPageBuffer->tempd2->testdata.temperweima.CardWaitTime*60;
						S_PaiDuiPageBuffer->tempvalue *= 254;
						S_PaiDuiPageBuffer->tempvalue = 360-S_PaiDuiPageBuffer->tempvalue;
						BasicPic(0x1510+index*16, 1, 137, 142, (unsigned short)(S_PaiDuiPageBuffer->tempvalue), 147, 360, 59 + index*119, (unsigned short)(S_PaiDuiPageBuffer->tempvalue)+29);
					}
						
					if((S_PaiDuiPageBuffer->tempd2->statues == statues5) || (S_PaiDuiPageBuffer->tempd2->statues == statues6))
						S_PaiDuiPageBuffer->myico.ICO_ID = 24;

					else
					{
						//检测卡图标闪烁
						if((S_PaiDuiPageBuffer->count % 2) == 0)
							S_PaiDuiPageBuffer->myico.ICO_ID = 23;
						else
							S_PaiDuiPageBuffer->myico.ICO_ID = 24;
					}
						
					S_PaiDuiPageBuffer->myico.X = 69+(index*119);
					S_PaiDuiPageBuffer->myico.Y = 135;
						
					BasicUI(0x1590+index*0x10 ,0x1907 , 1, &(S_PaiDuiPageBuffer->myico) , sizeof(Basic_ICO));
				}
				else
				{
					//清除倒计时时间
					ClearText(0x1610+index*0x08, 10);
					ClearText(0x1650+index*0x08, 10);
					//显示蓝色卡
					S_PaiDuiPageBuffer->myico.ICO_ID = 23;
					S_PaiDuiPageBuffer->myico.X = 69+index*119;
					S_PaiDuiPageBuffer->myico.Y = 135;
					BasicUI(0x1590+index*0x10 ,0x1907 , 1, &(S_PaiDuiPageBuffer->myico) , sizeof(Basic_ICO));
					BasicPic(0x1510+index*0x10, 0, 137, 15, 0, 22, 570, 55 , 0);
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

static MyState_TypeDef PageInit(void *pram)
{
	if(My_Pass == PageBufferMalloc())
		;
		
		SelectPage(93);
		
		//一秒读取一次是否插卡
		timer_set(&(S_PaiDuiPageBuffer->timer0), 1);
	
	return My_Pass;
}

static MyState_TypeDef PageBufferMalloc(void)
{		
	if(NULL == S_PaiDuiPageBuffer)
	{
		S_PaiDuiPageBuffer = MyMalloc(sizeof(PaiDuiPageBuffer));
		if(S_PaiDuiPageBuffer)	
		{
			memset(S_PaiDuiPageBuffer, 0, sizeof(PaiDuiPageBuffer));
	
			return My_Pass;
		}
	}

	return My_Fail;
}

static MyState_TypeDef PageBufferFree(void)
{
	MyFree(S_PaiDuiPageBuffer);
	S_PaiDuiPageBuffer = NULL;
	
	return My_Pass;
}



