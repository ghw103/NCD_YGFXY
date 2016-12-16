/******************************************************************************************
*文件名：COM_Task.c
*描述：独立看门狗任务
*说明：优先级1（最低），1S喂狗一次，
*作者：xsx
******************************************************************************************/

#include	"PlaySong_Task.h"
#include	"PlaySong_Function.h"
#include	"QueueUnits.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

#include	"stdio.h"
/******************************************************************************************/
/*****************************************局部变量声明*************************************/
const char wavfilename[63][20]=
{
	"0:/Audio/ncd_1.wav\0",
	"0:/Audio/ncd_2.wav\0",
	"0:/Audio/ncd_3.wav\0",
	"0:/Audio/ncd_4.wav\0",
	"0:/Audio/ncd_5.wav\0",
	"0:/Audio/ncd_6.wav\0",
	"0:/Audio/ncd_7.wav\0",
	"0:/Audio/ncd_8.wav\0",
	"0:/Audio/ncd_9.wav\0",
	"0:/Audio/ncd_10.wav\0",
	"0:/Audio/ncd_11.wav\0",
	"0:/Audio/ncd_12.wav\0",
	"0:/Audio/ncd_13.wav\0",
	"0:/Audio/ncd_14.wav\0",
	"0:/Audio/ncd_15.wav\0",
	"0:/Audio/ncd_16.wav\0",
	"0:/Audio/ncd_17.wav\0",
	"0:/Audio/ncd_18.wav\0",
	"0:/Audio/ncd_19.wav\0",
	"0:/Audio/ncd_20.wav\0",
	"0:/Audio/ncd_21.wav\0",
	"0:/Audio/ncd_22.wav\0",
	"0:/Audio/ncd_23.wav\0",
	"0:/Audio/ncd_24.wav\0",
	"0:/Audio/ncd_25.wav\0",
	"0:/Audio/ncd_26.wav\0",
	"0:/Audio/ncd_27.wav\0",
	"0:/Audio/ncd_28.wav\0",
	"0:/Audio/ncd_29.wav\0",
	"0:/Audio/ncd_30.wav\0",
	"0:/Audio/ncd_31.wav\0",
	"0:/Audio/ncd_32.wav\0",
	"0:/Audio/ncd_33.wav\0",
	"0:/Audio/ncd_34.wav\0",
	"0:/Audio/ncd_35.wav\0",
	"0:/Audio/ncd_36.wav\0",
	"0:/Audio/ncd_37.wav\0",
	"0:/Audio/ncd_38.wav\0",
	"0:/Audio/ncd_39.wav\0",
	"0:/Audio/ncd_40.wav\0",
	"0:/Audio/ncd_41.wav\0",
	"0:/Audio/ncd_42.wav\0",
	"0:/Audio/ncd_43.wav\0",
	"0:/Audio/ncd_44.wav\0",
	"0:/Audio/ncd_45.wav\0",
	"0:/Audio/ncd_46.wav\0",
	"0:/Audio/ncd_47.wav\0",
	"0:/Audio/ncd_48.wav\0",
	"0:/Audio/ncd_49.wav\0",
	"0:/Audio/ncd_50.wav\0",
	"0:/Audio/ncd_51.wav\0",
	"0:/Audio/ncd_52.wav\0",
	"0:/Audio/ncd_53.wav\0",
	"0:/Audio/ncd_54.wav\0",
	"0:/Audio/ncd_55.wav\0",
	"0:/Audio/ncd_56.wav\0",
	"0:/Audio/ncd_57.wav\0",
	"0:/Audio/ncd_58.wav\0",
	"0:/Audio/ncd_59.wav\0",
	"0:/Audio/ncd_60.wav\0",
	"0:/Audio/ncd_61.wav\0",
	"0:/Audio/ncd_62.wav\0",
	"0:/Audio/ncd_63.wav\0",
};
#define vPlaySongTask_PRIORITY			( ( unsigned portBASE_TYPE ) 2U )

#define	SongListSize	20							//播放列表可保持100个音频
static xQueueHandle SongListQueue = NULL;			//播放音频的队列
static xQueueHandle AudioStatusQueue = NULL;		//播放音频的状态
static unsigned char S_PlayStatus = 0;
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
	
	if(AudioStatusQueue == NULL)
		AudioStatusQueue = xQueueCreate( 10, ( unsigned portBASE_TYPE ) sizeof( unsigned char ) );
	
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
		if(pdPASS == xQueueReceive( SongListQueue, &tempdata, portMAX_DELAY ))
		{
			//发出开始播放信号
			S_PlayStatus = 1;
			SendDataToQueue(AudioStatusQueue, NULL, &S_PlayStatus , 1 , 1, 10 / portTICK_RATE_MS, NULL);
			
			AudioPlay(wavfilename[tempdata]);
			
			//发出停止播放信号
			S_PlayStatus = 0;
			SendDataToQueue(AudioStatusQueue, NULL, &S_PlayStatus , 1 , 1, 10 / portTICK_RATE_MS, NULL);
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
		StopMyPlay();
	}
	/*模式2，如果正在播放，则取消新音频*/
	else if(mode == 1)
	{
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

MyState_TypeDef TakeAudioPlayStatus(unsigned char *status)
{
	if(pdPASS == xQueueReceive( AudioStatusQueue, status,  10/portTICK_RATE_MS))
		return My_Pass;
	else
		return My_Fail;
}
