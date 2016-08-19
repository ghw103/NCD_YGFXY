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
static MyState_TypeDef AudioFileInit(void);
static void AudioDataSend(void);
static MyState_TypeDef wav_filldata(void *buf , unsigned int len);
static MyState_TypeDef I2S_CODEC_WaveParm(void * sourceaddr);
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/


/***************************************************************************************************
*FunctionName:I2S_CODEC_WaveParm
*Description:解码文件 ， 只保存riff，fam ，data三个数据块
*Input:sourceaddr -- 传入头文件信息
*Output:
*Author:xsx
*Data:2015年10月12日15:08:39
***************************************************************************************************/
static MyState_TypeDef I2S_CODEC_WaveParm(void * sourceaddr)
{
	__WaveHeader *my__WaveHeader = NULL;
	unsigned int temp;
	MyState_TypeDef mystautes = My_Fail;
	unsigned char *p = (unsigned char *)sourceaddr;
	
	if(myAudioPlayInfo)
	{
		my__WaveHeader = MyMalloc(sizeof(__WaveHeader));
		
		if(my__WaveHeader)
		{
			memset(my__WaveHeader, 0, sizeof(__WaveHeader));
			
			//读取riff头信息
			memcpy(&my__WaveHeader->riff, p, sizeof(ChunkRIFF));
			if(my__WaveHeader->riff.ChunkID == RIFFChunkID)
			{
				if(my__WaveHeader->riff.Format == FileFormat)
				{
					myAudioPlayInfo->audioinfo.datastart += sizeof(ChunkRIFF);
					/*读取FMT 头信息*/
					memcpy(&my__WaveHeader->fmt, (p+12), sizeof(ChunkFMT));
					if(my__WaveHeader->fmt.ChunkID == FormatID)
					{
						myAudioPlayInfo->audioinfo.datastart += 8+my__WaveHeader->fmt.ChunkSize;
						/*读取4个字节，判断是fact区还是data区*/
						temp = *((unsigned int *)(p+12+8+my__WaveHeader->fmt.ChunkSize));
						if((temp == FactID)||(temp == DataID))
						{
							if(temp == FactID)
							{
								myAudioPlayInfo->audioinfo.datastart += 12;
								/*读取data区头信息*/
								memcpy(&my__WaveHeader->data, (p+12+8+my__WaveHeader->fmt.ChunkSize+12), sizeof(ChunkDATA));
							}
							else if(temp == DataID)
							{
								/*读取data区头信息*/
								memcpy(&my__WaveHeader->data, (p+12+8+my__WaveHeader->fmt.ChunkSize), sizeof(ChunkDATA));
							}
							myAudioPlayInfo->audioinfo.datastart += 8;
								/*wav数据可用*/
							mystautes = My_Pass;
									
							myAudioPlayInfo->audioinfo.audioformat = my__WaveHeader->fmt.AudioFormat;	//音频格式
							myAudioPlayInfo->audioinfo.nchannels = my__WaveHeader->fmt.NumOfChannels;	//通道数目
							myAudioPlayInfo->audioinfo.samplerate = my__WaveHeader->fmt.SampleRate;	//采样率
							myAudioPlayInfo->audioinfo.bitrate = my__WaveHeader->fmt.ByteRate*8;		//kbps
							myAudioPlayInfo->audioinfo.blockalign = my__WaveHeader->fmt.BlockAlign;	//块对齐
							myAudioPlayInfo->audioinfo.bps = my__WaveHeader->fmt.BitsPerSample;		//量化位数16/24/32
									
							myAudioPlayInfo->audioinfo.datasize = my__WaveHeader->data.ChunkSize;		//数据块大小
								
/*							sprintf(buf, "wavx->audioformat:%d\r\n",myAudioPlayInfo->audioinfo.audioformat);
							SendDataToNCDServer(buf, strlen(buf));
								
							sprintf(buf, "wavx->nchannels:%d\r\n",myAudioPlayInfo->audioinfo.nchannels);
							SendDataToNCDServer(buf, strlen(buf));
								
							sprintf(buf, "wavx->samplerate:%d\r\n",myAudioPlayInfo->audioinfo.samplerate);
							SendDataToNCDServer(buf, strlen(buf));
								
							sprintf(buf, "wavx->bitrate:%d\r\n",myAudioPlayInfo->audioinfo.bitrate);
							SendDataToNCDServer(buf, strlen(buf));
								
							sprintf(buf, "wavx->bps:%d\r\n",myAudioPlayInfo->audioinfo.bps);
							SendDataToNCDServer(buf, strlen(buf));

							sprintf(buf, "wavx->datastart:%d\r\n",myAudioPlayInfo->audioinfo.datastart); 
							SendDataToNCDServer(buf, strlen(buf));*/
						}
					}
				}
			}
		}
		MyFree(my__WaveHeader);	
	}
	return mystautes;

}
/***************************************************************************************************
*FunctionName：AudioFileInit
*Description：音频文件头信息解码
*Input：
*Output：
*Author：xsx
*Data：2015年10月12日15:11:24
***************************************************************************************************/
static MyState_TypeDef AudioFileInit(void)
{
	MyState_TypeDef mystatues = My_Fail;
	
	if(myAudioPlayInfo)
	{
		myAudioPlayInfo->tempbuf = MyMalloc(512);
		if(myAudioPlayInfo->tempbuf)
		{
			memset(myAudioPlayInfo->tempbuf, 0, 512);
			
			myAudioPlayInfo->myfile.res = f_read(&(myAudioPlayInfo->myfile.file), myAudioPlayInfo->tempbuf, 512, &(myAudioPlayInfo->myfile.br) );
			if(FR_OK == myAudioPlayInfo->myfile.res)
			{
				if(My_Pass == I2S_CODEC_WaveParm(myAudioPlayInfo->tempbuf))
				{
					mystatues = My_Pass;
					ConfigI2S(myAudioPlayInfo->audioinfo.samplerate);
				}
			}	
			MyFree(myAudioPlayInfo->tempbuf);
		}
	}
	
	return mystatues;
}

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
		myAudioPlayInfo->source1 = MyMalloc(DataBlockSize);
		myAudioPlayInfo->source2 = MyMalloc(DataBlockSize);
		
		if(myAudioPlayInfo->source1 && myAudioPlayInfo->source1)
		{
			myAudioPlayInfo->sourceindex = 1;
			
			f_lseek (&(myAudioPlayInfo->myfile.file), myAudioPlayInfo->audioinfo.datastart);				//跳过头文件信息，定位到wav数据区
		
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
			
			MyFree(myAudioPlayInfo->source2);
			MyFree(myAudioPlayInfo->source1);
		}
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
			if(My_Pass == AudioFileInit())
				AudioDataSend();

			f_close(&(myAudioPlayInfo->myfile.file));
		}
		
		MyFree(myAudioPlayInfo);
		myAudioPlayInfo = NULL;
	}
}

void StopMyPlay(void)
{
	if(myAudioPlayInfo)
	{
		myAudioPlayInfo->playstatues = 1;
	}
}


