#include 	"PlaySong_Function.h"
#include	"PT8211_Driver.h"
#include	"user_fatfs.h"
#include	"Led_Driver.h"

#include	"MyMem.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

#include	"stdlib.h"
#include	<string.h>

/******************************************************************************************/
/*****************************************局部变量*************************************/
AudioPlayInfo * myAudioPlayInfo = NULL;

/******************************************************************************************/
/*****************************************局部函数*************************************/
static void ChangeOutputDataSource(void);
static void AudioDataSend(void);
static MyState_TypeDef wav_filldata(void *buf , unsigned int len);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/



static MyState_TypeDef wav_filldata(void *buf , unsigned int len)
{
	unsigned short i;
	MyState_TypeDef statues = My_Fail;

	unsigned char *p = (unsigned char *)buf;

	f_read(&(myAudioPlayInfo->myfile.file), p, len, &(myAudioPlayInfo->myfile.br));
	
	if(myAudioPlayInfo->myfile.br < len)
	{
		for(i=myAudioPlayInfo->myfile.br; i<len-myAudioPlayInfo->myfile.br; i++)
			p[i] = 0;
	}
	else
		statues = My_Pass;
	
	return statues;
}
static void ChangeOutputDataSource(void)
{
	if(DMA1_Stream5->CR&(1<<19)) 								//当前使用Memory1数据
		myAudioPlayInfo->sourceindex = 1;                       //可以将数据读取到缓冲区0

	else                               							//当前使用Memory0数据
		myAudioPlayInfo->sourceindex = 0;                       //可以将数据读取到缓冲区1
	
	myAudioPlayInfo->sourcestatues = 1;
}

static void AudioDataSend(void)
{
	MyState_TypeDef readstatues =  My_Pass;
	
	if(myAudioPlayInfo)
	{
		myAudioPlayInfo->sourceindex = 1;
			
		f_lseek (&(myAudioPlayInfo->myfile.file), 44);				//跳过头文件信息，定位到wav数据区
		
		myAudioPlayInfo->playstatues = 1;
		myAudioPlayInfo->sourcestatues = 0;
			
		if(My_Pass == wav_filldata(myAudioPlayInfo->source1, DataBlockSize))
		{
			if(My_Pass == wav_filldata(myAudioPlayInfo->source2, DataBlockSize))
			{
				myAudioPlayInfo->playstatues = 0;
			}
		}

		I2S_DMA_Init(myAudioPlayInfo->source1, myAudioPlayInfo->source2, DataBlockSize/2);
			
		StartPlay();
		while(1)
		{			
			/*等待当前缓冲区播放完毕*/
			if(myAudioPlayInfo->sourcestatues != 0)
			{
				if((My_Fail == readstatues) || (myAudioPlayInfo->playstatues == 1))
					break;
					
				if(0 == myAudioPlayInfo->playstatues)
				{
					if(myAudioPlayInfo->sourceindex == 0)
						readstatues = wav_filldata(myAudioPlayInfo->source2, DataBlockSize);
					else if(myAudioPlayInfo->sourceindex == 1)
						readstatues = wav_filldata(myAudioPlayInfo->source1, DataBlockSize);
				}
					
				myAudioPlayInfo->sourcestatues = 0;
			}
				
			vTaskDelay(10*portTICK_RATE_MS);
		}
		StopPlay();
	}
}
/***************************************************************************************************
*FunctionName：AudioPlay
*Description：播放音频
*Input：fname -- 文件名
*Output：
*Author：xsx
*Data：2015年10月12日15:13:26
***************************************************************************************************/
void AudioPlay(const char *fname)
{	
	i2s_tx_callback = ChangeOutputDataSource;
	
	myAudioPlayInfo = MyMalloc(sizeof(AudioPlayInfo));
	if(myAudioPlayInfo)
	{
		memset(myAudioPlayInfo, 0, sizeof(AudioPlayInfo));
		
		myAudioPlayInfo->playstatues = 1;
		
		myAudioPlayInfo->myfile.res = f_open(&(myAudioPlayInfo->myfile.file) , (TCHAR*)fname, FA_READ);
		
		if(FR_OK == myAudioPlayInfo->myfile.res)
		{
			AudioDataSend();

			f_close(&(myAudioPlayInfo->myfile.file));
		}
		
		MyFree(myAudioPlayInfo);
	}
}

void StopMyPlay(void)
{
	if(myAudioPlayInfo)
	{
		myAudioPlayInfo->playstatues = 1;
	}
}


