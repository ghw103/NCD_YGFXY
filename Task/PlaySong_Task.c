/******************************************************************************************
*文件名：COM_Task.c
*描述：独立看门狗任务
*说明：优先级1（最低），1S喂狗一次，
*作者：xsx
******************************************************************************************/

#include	"PlaySong_Task.h"
#include	"PlaySong_Function.h"
#include	"QueueUnits.h"
#include	"SystemSet_Data.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

#include	"stdio.h"
/******************************************************************************************/
/*****************************************局部变量声明*************************************/
const char wavfilename[59][20]=
{
	"0:/Audio/n1.wav\0",
	"0:/Audio/n2.wav\0",
	"0:/Audio/n3.wav\0",
	"0:/Audio/n4.wav\0",
	"0:/Audio/n5.wav\0",
	"0:/Audio/n6.wav\0",
	"0:/Audio/n7.wav\0",
	"0:/Audio/n8.wav\0",
	"0:/Audio/n9.wav\0",
	"0:/Audio/n10.wav\0",
	"0:/Audio/n11.wav\0",
	"0:/Audio/n12.wav\0",
	"0:/Audio/n13.wav\0",
	"0:/Audio/n14.wav\0",
	"0:/Audio/n15.wav\0",
	"0:/Audio/n16.wav\0",
	"0:/Audio/n17.wav\0",
	"0:/Audio/n18.wav\0",
	"0:/Audio/n19.wav\0",
	"0:/Audio/n20.wav\0",
	"0:/Audio/n21.wav\0",
	"0:/Audio/n22.wav\0",
	"0:/Audio/n23.wav\0",
	"0:/Audio/n24.wav\0",
	"0:/Audio/n25.wav\0",
	"0:/Audio/n26.wav\0",
	"0:/Audio/n27.wav\0",
	"0:/Audio/n28.wav\0",
	"0:/Audio/n29.wav\0",
	"0:/Audio/n30.wav\0",
	"0:/Audio/n31.wav\0",
	"0:/Audio/n32.wav\0",
	"0:/Audio/n33.wav\0",
	"0:/Audio/n34.wav\0",
	"0:/Audio/n35.wav\0",
	"0:/Audio/n36.wav\0",
	"0:/Audio/n37.wav\0",
	"0:/Audio/n38.wav\0",
	"0:/Audio/n39.wav\0",
	"0:/Audio/n40.wav\0",
	"0:/Audio/n41.wav\0",
	"0:/Audio/n42.wav\0",
	"0:/Audio/n43.wav\0",
	"0:/Audio/n44.wav\0",
	"0:/Audio/n45.wav\0",
	"0:/Audio/n46.wav\0",
	"0:/Audio/n47.wav\0",
	"0:/Audio/n48.wav\0",
	"0:/Audio/n49.wav\0",
	"0:/Audio/n50.wav\0",
	"0:/Audio/n51.wav\0",
	"0:/Audio/n52.wav\0",
	"0:/Audio/n53.wav\0",
	"0:/Audio/n54.wav\0",
	"0:/Audio/n55.wav\0",
	"0:/Audio/n56.wav\0",
	"0:/Audio/n57.wav\0",
	"0:/Audio/n58.wav\0",
	"0:/Audio/n59.wav\0"
};
#define vPlaySongTask_PRIORITY			( ( unsigned portBASE_TYPE ) 2U )

#define	SongListSize	20							//播放列表可保持100个音频
static xQueueHandle SongListQueue = NULL;			//播放音频的队列
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
char StartvPlaySongTask(void)
{
	if(SongListQueue == NULL)
		SongListQueue = xQueueCreate( SongListSize, ( unsigned portBASE_TYPE ) sizeof( unsigned char ) );
	
	return xTaskCreate( vPlaySongTask, ( const char * ) "vPlaySongTask ", configMINIMAL_STACK_SIZE*2, NULL, vPlaySongTask_PRIORITY, NULL );
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
			
			AudioPlay(wavfilename[tempdata]);
			
			//发出停止播放信号
			S_PlayStatus = 0;
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
	
	if(isMute())
		return pdPASS;
	
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
	/*模式3，清空剩余的播放*/
	else if(mode == 2)
	{
		while(pdPASS == xQueueReceive( SongListQueue, &songnum, 0 ));
	}
	//模式4，无论什么状态，都添加新音频，等待播放*/
	else
	{
		
	}
	
	songnum = num;
	if( xQueueSend( SongListQueue, &songnum, 0 ) != pdPASS )
	{
		return pdFAIL;
	}

	return pdPASS;
}

/***************************************************************************************************
*FunctionName: stopPlay
*Description: 停止播放
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年1月20日14:58:57
***************************************************************************************************/
void stopPlay(void)
{
	StopMyPlay();
}

/***************************************************************************************************
*FunctionName: getPlayStatus
*Description: 读取播放状态
*Input: 
*Output: 
*Return: 
*Author: xsx
*Date: 2017年1月20日14:59:13
***************************************************************************************************/
unsigned char getPlayStatus(void)
{
	return S_PlayStatus;
}
