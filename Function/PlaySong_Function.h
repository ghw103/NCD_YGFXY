#ifndef __PLAYSONG_F_H
#define __PLAYSONG_F_H	


#include	"Define.h"
#include 	"FreeRTOS.h"
#include	"semphr.h"

#define	DataBlockSize	4096

/* Audio Parsing Constants */
#define  RIFFChunkID         0x46464952  /* correspond to the letters 'RIFF' */
#define  FileFormat          0x45564157  /* correspond to the letters 'WAVE' */
#define  FormatID            0x20746d66  /* correspond to the letters 'fmt ' */
#define  DataID              0x61746164 /* correspond to the letters 'data' */
#define  FactID              0x74636166 /* correspond to the letters 'fact' */

//RIFF¿é
typedef __packed struct
{
    unsigned int ChunkID;
    unsigned int ChunkSize;
    unsigned int Format;
}ChunkRIFF;
//fmt¿é
typedef __packed struct
{
    unsigned int ChunkID;
    unsigned int ChunkSize ;
    unsigned short AudioFormat;
	unsigned short NumOfChannels;
	unsigned int SampleRate;
	unsigned int ByteRate;
	unsigned short BlockAlign;
	unsigned short BitsPerSample;
}ChunkFMT;	   
//fact¿é 
typedef __packed struct 
{
    unsigned int ChunkID;
    unsigned int ChunkSize ;
    unsigned int NumOfSamples;
}ChunkFACT;
//LIST¿é 
typedef __packed struct 
{
    unsigned int ChunkID;
    unsigned int ChunkSize ;
}ChunkLIST;

//data¿é 
typedef __packed struct 
{
    unsigned int ChunkID;
    unsigned int ChunkSize ;
}ChunkDATA;

//wavÍ·
typedef __packed struct
{ 
	ChunkRIFF riff;
	ChunkFMT fmt;
	ChunkDATA data;		 
}__WaveHeader; 


typedef __packed struct
{ 
    unsigned short audioformat;
	unsigned short nchannels;
	unsigned short blockalign;
	unsigned int datasize;

    unsigned int totsec ;
    unsigned int cursec ;
	
    unsigned int bitrate;
	unsigned int samplerate;
	unsigned short bps;
	
	unsigned int datastart;
}AudioInfo; 

typedef struct
{
	FatfsFileInfo_Def myfile;					//音频文件
	AudioInfo audioinfo;						//音频信息
	unsigned char source1[DataBlockSize];						//缓冲区1
	unsigned char source2[DataBlockSize];						//缓冲区2
	unsigned char sourceindex;					//当前空闲的缓冲区
	unsigned char sourcestatues;				//当前缓冲区的播放情况， 0正在播放，1一个缓冲区播放完毕，切换缓冲区
	unsigned char playstatues;					//播放控制，0 -- 正常播放， 1 -- 暂停， 2 -- 停止
	unsigned char *tempbuf;						//临时缓冲区
}AudioPlayInfo;

void AudioPlay(const char *fname);
void StopMyPlay(void);
#endif 















