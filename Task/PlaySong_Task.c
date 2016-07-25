/******************************************************************************************
*文件名：COM_Task.c
*描述：独立看门狗任务
*说明：优先级1（最低），1S喂狗一次，
*作者：xsx
******************************************************************************************/

#include	"PlaySong_Task.h"
#include	"PlaySong_Function.h"
#include	"Define.h"


#include 	"FreeRTOS.h"
#include 	"task.h"

#include	"stdio.h"
/******************************************************************************************/
/*****************************************局部变量声明*************************************/
const char wavfilename[62][40]=
{
	"0:/Sound/1TimeSone.wav",
	"0:/Sound/2TimeSone.wav",
	"0:/Sound/3TimeSone.wav",
	"0:/Sound/4TimeSone.wav",
	"0:/Sound/5TimeSone.wav",
	"0:/Sound/6TimeSone.wav",										//5
	"0:/Sound/7TimeSone.wav",
	"0:/Sound/8TimeSone.wav",
	"0:/Sound/ActionTimeOutCancel.wav",
	"0:/Sound/AddSample.wav",
	"0:/Sound/AddSample2.wav",										//10
	"0:/Sound/Busy1IntoTimeOut.wav",
	"0:/Sound/Busy2IntoTimeOut.wav",
	"0:/Sound/Busy3IntoTimeOut.wav",
	"0:/Sound/Busy4IntoTimeOut.wav",
	"0:/Sound/Busy5IntoTimeOut.wav",										//15
	"0:/Sound/Busy6IntoTimeOut.wav",
	"0:/Sound/Busy7IntoTimeOut.wav",
	"0:/Sound/Busy8IntoTimeOut.wav",
	"0:/Sound/CannotTestNormal.wav",
	"0:/Sound/Error_CardTemp.wav",									//20
	"0:/Sound/Error_CardTimeOut.wav",
	"0:/Sound/Error_CardUsed.wav",
	"0:/Sound/Error_ErWeiMa.wav",
	"0:/Sound/Error_ETemp.wav",
	"0:/Sound/Error_Memery.wav",								//25
	"0:/Sound/InputSampleID.wav",
	"0:/Sound/Put1ToQueue.wav",
	"0:/Sound/Put2ToQueue.wav",
	"0:/Sound/Put3ToQueue.wav",
	"0:/Sound/Put4ToQueue.wav",										//30
	"0:/Sound/Put5ToQueue.wav",
	"0:/Sound/Put6ToQueue.wav",
	"0:/Sound/Put7ToQueue.wav",
	"0:/Sound/Put8ToQueue.wav",
	"0:/Sound/Put1ToTestPlace.wav",								//35
	"0:/Sound/Put2ToTestPlace.wav",
	"0:/Sound/Put3ToTestPlace.wav",
	"0:/Sound/Put4ToTestPlace.wav",
	"0:/Sound/Put5ToTestPlace.wav",
	"0:/Sound/Put6ToTestPlace.wav",									//40
	"0:/Sound/Put7ToTestPlace.wav",
	"0:/Sound/Put8ToTestPlace.wav",
	"0:/Sound/PutNewCardToTestPlace.wav",
	"0:/Sound/Scanning.wav",
	"0:/Sound/SelectTester.wav",								//45
	"0:/Sound/StartSystemInit.wav",
	"0:/Sound/StartTimeDown.wav",
	"0:/Sound/SystemAlarm.wav",
	"0:/Sound/SystemError.wav",								//49
	"0:/Sound/SystemInitDone.wav",
	"0:/Sound/TestQueueFullWait.wav",
	"0:/Sound/Welcome.wav",											//52
	"0:/Sound/1Ready.wav",
	"0:/Sound/2Ready.wav",
	"0:/Sound/3Ready.wav",						//55
	"0:/Sound/4Ready.wav",
	"0:/Sound/5Ready.wav",
	"0:/Sound/6Ready.wav",
	"0:/Sound/7Ready.wav",
	"0:/Sound/8Ready.wav",						//60
	"0:/Sound/TestBusyWait.wav",
};
#define vPlaySongTask_PRIORITY			( ( unsigned portBASE_TYPE ) 2U )

#define	SongListSize	20							//播放列表可保持100个音频
static xQueueHandle SongListQueue = NULL;			//播放音频的队列
static unsigned char GB_PlayStatues = 0;			//0空闲，1播放
/******************************************************************************************/
/*****************************************局部函数声明*************************************/

static void vPlaySongTask( void *pvParameters );

/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

/************************************************************************
** 函数名:StartvIwdgTask
** 功  能:创建并启动喂狗任务
** 输  入:无
** 输  出:无
** 返  回：无
** 备  注：无
** 时  间: 2015年5月15日 17:04:43 
** 作  者：xsx                                                 
************************************************************************/
void StartvPlaySongTask(void)
{
	if(SongListQueue == NULL)
		SongListQueue = xQueueCreate( SongListSize, ( unsigned portBASE_TYPE ) sizeof( unsigned char ) );
	
	xTaskCreate( vPlaySongTask, ( const char * ) "vPlaySongTask ", configMINIMAL_STACK_SIZE*2, NULL, vPlaySongTask_PRIORITY, NULL );
}

/************************************************************************
** 函数名:vIwdgTask
** 功  能:喂狗任务
** 输  入:无
** 输  出:无
** 返  回：无
** 备  注：无
** 时  间: 2015年5月15日 17:04:12  
** 作  者：xsx                                                 
************************************************************************/
static void vPlaySongTask( void *pvParameters )
{
	unsigned char tempdata;
	while(1)
	{
		if(pdPASS == xQueueReceive( SongListQueue, &tempdata, 200*portTICK_RATE_MS ))
		{
			GB_PlayStatues = 1;
			AudioPlay(wavfilename[tempdata]);
			GB_PlayStatues = 0;
		}
	}
}

/***************************************************************************************************
*FunctionName：AddNumOfSongToList
*Description：添加歌曲到播放列表，如果音频模块空闲，会立即播放
*Input：num -- 歌曲编号，需与歌曲文件名数组对应
*Output：返回添加成功或失败
*Author：xsx
*Data：2015年10月13日09:06:23
***************************************************************************************************/
unsigned char AddNumOfSongToList(unsigned char num, unsigned char mode)
{
	unsigned char songnum = num;
	
	if(SongListQueue == NULL)
		return pdFAIL;

	/*模式1， 终止正在播放，播放新音频*/
	if(mode == 0)
	{
		if(GB_PlayStatues == 1)
			StopMyPlay();
	}
	/*模式2，如果正在播放，则取消新音频*/
	else if(mode == 1)
	{
		if(GB_PlayStatues == 1)
			return pdPASS;
	}
	/*模式3，无论什么状态，都添加新音频，等待播放*/
	else if(mode == 2)
	{

	}
	
	if( xQueueSend( SongListQueue, &songnum, 0 ) != pdPASS )
	{
		return pdFAIL;
	}

	return pdPASS;
}

unsigned char GetPlayStatues(void)
{
	return GB_PlayStatues;
}
