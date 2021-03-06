/******************************************************************************************/
/*****************************************头文件*******************************************/

#include	"PreReadCardPage.h"
#include	"Define.h"
#include	"LCD_Driver.h"
#include	"UI_Data.h"
#include	"MyMem.h"
#include	"MyTest_Data.h"
#include 	"MLX90614_Driver.h"
#include	"LunchPage.h"
#include	"CodeScan_Task.h"
#include	"WaittingCardPage.h"
#include	"TimeDownNorPage.h"
#include	"CodeScanFunction.h"
#include	"PaiDuiPage.h"
#include	"CardStatues_Data.h"
#include	"Motor_Fun.h"
#include	"PlaySong_Task.h"
#include	"Test_Task.h"
#include	"SDFunction.h"
#include	"MyTools.h"
#include	"CRC16.h"
#include	"System_Data.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	"queue.h"

#include	<string.h>
#include	"stdio.h"

/******************************************************************************************/
/*****************************************局部变量声明*************************************/
static PreReadPageBuffer * S_PreReadPageBuffer = NULL;
const unsigned int TestLineHigh1 = 77010;	//此数据与曲线显示区域高度有关，如果界面不改，此数不改
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

static void clearPageText(void);
static void CheckQRCode(void);
static void ShowCardInfo(void);
static void CheckPreTestCard(void);
static void showTemperature(void);
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
MyState_TypeDef createPreReadCardActivity(Activity * thizActivity, Intent * pram)
{
	if(NULL == thizActivity)
		return My_Fail;
	
	if(My_Pass == activityBufferMalloc())
	{
		InitActivity(thizActivity, "PreReadCardActivity\0", activityStart, activityInput, activityFresh, activityHide, activityResume, activityDestroy);
		
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
	if(S_PreReadPageBuffer)
	{
		S_PreReadPageBuffer->currenttestdata = GetCurrentTestItem();
		
		clearPageText();
		
		clearScanQRCodeResult();
		clearTestResult();
		
		vTaskDelay(500 / portTICK_RATE_MS);
		StartScanQRCode(&(S_PreReadPageBuffer->temperweima));
	}
	
	SelectPage(92);
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
	if(S_PreReadPageBuffer)
	{
		/*命令*/
		S_PreReadPageBuffer->lcdinput[0] = pbuf[4];
		S_PreReadPageBuffer->lcdinput[0] = (S_PreReadPageBuffer->lcdinput[0]<<8) + pbuf[5];
		
		/*二维码读取失败，过期，已使用*/
		if((S_PreReadPageBuffer->lcdinput[0] >= 0x1400) && (S_PreReadPageBuffer->lcdinput[0] <= 0x1405))
		{
			/*数据*/
			S_PreReadPageBuffer->lcdinput[1] = pbuf[7];
			S_PreReadPageBuffer->lcdinput[1] = (S_PreReadPageBuffer->lcdinput[1]<<8) + pbuf[8];
			
			/*更换检测卡*/
			if(S_PreReadPageBuffer->lcdinput[1] == 0x0001)
			{
				//如果是排队中的再次预读，则返回排队界面，状态切换回之前的状态
				if(S_PreReadPageBuffer->currenttestdata->statues == status_prereadagain_n)
					S_PreReadPageBuffer->currenttestdata->statues = status_incard_n;
				else if(S_PreReadPageBuffer->currenttestdata->statues == status_prereadagain_o)
					S_PreReadPageBuffer->currenttestdata->statues = status_incard_o;
				//如果是第一次预读
				else if(S_PreReadPageBuffer->currenttestdata->statues == status_wait1)
					S_PreReadPageBuffer->currenttestdata->statues = status_wait1;
				
				backToFatherActivity();
			}
			//取消测试
			else if(S_PreReadPageBuffer->lcdinput[1] == 0x0000)
			{
				DeleteCurrentTest();
				
				//如果还有卡在排队，说明这个界面是从排队界面过来的，只需返回到排队界面
				if(IsPaiDuiTestting())
					backToActivity(paiduiActivityName);	
				else
					backToActivity(lunchActivityName);
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
	CheckQRCode();
		
	CheckPreTestCard();
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
	if(NULL == S_PreReadPageBuffer)
	{
		S_PreReadPageBuffer = MyMalloc(sizeof(PreReadPageBuffer));
		
		if(S_PreReadPageBuffer)
		{
			memset(S_PreReadPageBuffer, 0, sizeof(PreReadPageBuffer));
	
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
	MyFree(S_PreReadPageBuffer);
	S_PreReadPageBuffer = NULL;
}



static void CheckQRCode(void)
{
	if(My_Pass == TakeScanQRCodeResult(&(S_PreReadPageBuffer->scancode)))
	{	
		//不支持的品种
		if(S_PreReadPageBuffer->scancode == CardUnsupported)
		{
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(56, 0);
			SendKeyCode(6);
		}
		//过期
		else if(S_PreReadPageBuffer->scancode == CardCodeTimeOut)
		{
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(15, 0);
			SendKeyCode(4);
		}
		//读取成功
		else if(S_PreReadPageBuffer->scancode == CardCodeScanOK)
		{
			ShowCardInfo();
			
			//读取完二维码后，试剂卡在卡槽内部，此时读取温度比较合适
			showTemperature();
			
			//如果是第一次读取二维码
			if(S_PreReadPageBuffer->currenttestdata->statues == status_preread)
			{
				//将读取的二维码数据拷贝到测试数据包中
				memcpy(&(S_PreReadPageBuffer->currenttestdata->testdata.temperweima), &(S_PreReadPageBuffer->temperweima), sizeof(QRCode));
				
				//设置倒计时时间
				timer_set(&(S_PreReadPageBuffer->currenttestdata->timer), S_PreReadPageBuffer->currenttestdata->testdata.temperweima.CardWaitTime*60);
			
				//读取校准参数
				memcpy(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust.ItemName, S_PreReadPageBuffer->currenttestdata->testdata.temperweima.ItemName, AdjItemNameLen);
				getAdjPram(getGBSystemSetData(), &(S_PreReadPageBuffer->currenttestdata->testdata.tempadjust));
				
				S_PreReadPageBuffer->preTestErrorCount = 0;
				StartTest(S_PreReadPageBuffer->currenttestdata);
			}
			else
			{
				//校验试机卡编号
				if((pdPASS == CheckStrIsSame(S_PreReadPageBuffer->currenttestdata->testdata.temperweima.PiHao, S_PreReadPageBuffer->temperweima.PiHao, 15)) &&
					(pdPASS == CheckStrIsSame(S_PreReadPageBuffer->currenttestdata->testdata.temperweima.piNum, S_PreReadPageBuffer->temperweima.piNum, 5)))
				{
					startActivity(createTimeDownActivity, NULL);
				}
				//试剂卡变更
				else
				{
					vTaskDelay(100 / portTICK_RATE_MS);
					MotorMoveTo(MaxLocation, 1);
					AddNumOfSongToList(13, 0);
					SendKeyCode(2);
				}
			}
		}
		/*其他错误：CardCodeScanFail, CardCodeCardOut, CardCodeScanTimeOut, CardCodeCRCError*/
		else
		{
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(12, 0);
			SendKeyCode(1);
		}
	}
}

static void CheckPreTestCard(void)
{
	if(My_Pass == TakeTestResult(&(S_PreReadPageBuffer->cardpretestresult)))
	{
		timer_restart(&(S_PreReadPageBuffer->currenttestdata->timer));
		
		//未加样
		if(S_PreReadPageBuffer->cardpretestresult == NoSample)
		{
			//未加样重测3次，第三次未加样则表明真的未加样
			S_PreReadPageBuffer->preTestErrorCount++;
			if(S_PreReadPageBuffer->preTestErrorCount < 8)
			{	
				StartTest(S_PreReadPageBuffer->currenttestdata);
			}
			else
			{
				MotorMoveTo(MaxLocation, 1);
				AddNumOfSongToList(16, 0);
				SendKeyCode(5);
			}
		}
		else if(S_PreReadPageBuffer->cardpretestresult == ResultIsOK)
		{
			MotorMoveTo(MaxLocation, 1);
			AddNumOfSongToList(14, 0);
			SendKeyCode(3);
		}
		else if(S_PreReadPageBuffer->cardpretestresult == PeakError)
		{
			//如果是排队模式，则进入排队界面
			if(S_PreReadPageBuffer->currenttestdata->testlocation > 0)
			{
				MotorMoveTo(MaxLocation, 1);
				
				S_PreReadPageBuffer->currenttestdata->statues = status_start;

				startActivity(createPaiDuiActivity, NULL);
			}
			else
			{		
				startActivity(createTimeDownActivity, NULL);
			}
		}
		
	}
}

static void clearPageText(void)
{
	ClearText(0x1420);
	ClearText(0x1430);
	ClearText(0x1440);
	ClearText(0x1450);
	ClearText(0x1460);
	ClearText(0x1470);
}

static void ShowCardInfo(void)
{
	sprintf(S_PreReadPageBuffer->buf, "%s\0", S_PreReadPageBuffer->temperweima.ItemName);
	DisText(0x1420, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf)+1);
		
	sprintf(S_PreReadPageBuffer->buf, "%s-%s\0", S_PreReadPageBuffer->temperweima.PiHao, S_PreReadPageBuffer->temperweima.piNum);
	DisText(0x1430, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf)+1);
	
	sprintf(S_PreReadPageBuffer->buf, "%d S\0", S_PreReadPageBuffer->temperweima.CardWaitTime*60);
	DisText(0x1440, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf)+1);
		
	sprintf(S_PreReadPageBuffer->buf, "20%02d年%02d月%02d日\0", S_PreReadPageBuffer->temperweima.CardBaoZhiQi.year, 
		S_PreReadPageBuffer->temperweima.CardBaoZhiQi.month, S_PreReadPageBuffer->temperweima.CardBaoZhiQi.day);
	DisText(0x1450, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf)+1);
}

static void showTemperature(void)
{
	//获取检测卡温度
	S_PreReadPageBuffer->currenttestdata->testdata.TestTemp.O_Temperature = GetCardTemperature();
	S_PreReadPageBuffer->currenttestdata->testdata.TestTemp.E_Temperature = GetGB_EnTemperature();
	
	sprintf(S_PreReadPageBuffer->buf, "%2.1f ℃\0", S_PreReadPageBuffer->currenttestdata->testdata.TestTemp.O_Temperature);
	DisText(0x1460, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf)+1);
	
	sprintf(S_PreReadPageBuffer->buf, "%2.1f ℃\0",S_PreReadPageBuffer->currenttestdata->testdata.TestTemp.E_Temperature);
	DisText(0x1470, S_PreReadPageBuffer->buf, strlen(S_PreReadPageBuffer->buf)+1);
}

