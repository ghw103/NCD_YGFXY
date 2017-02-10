/***************************************************************************************************
*FileName：PT8211_Driver
*Description：音频DA驱动
*Author：xsx
*Data：2015年10月6日10:57:01
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/
#include 	"stm32f4xx.h"
#include	"PT8211_Driver.h"


#include	"MyMem.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/
void (*i2s_tx_callback)(void);
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：PT8211_GPIOInit
*Description：初始化PT8211的I2S接口
*Input：None
*Output：None
*Author：xsx
*Data：2015年8月26日14:34:45
***************************************************************************************************/
void PT8211_GPIOInit(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;

  	RCC_AHB1PeriphClockCmd(I2S_CK_Rcc | I2S_SD_Rcc | I2S_WS_Rcc | Audio_Mute_Rcc, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//使能SPI1时钟

  	GPIO_InitStructure.GPIO_Pin = I2S_CK_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  	GPIO_Init(I2S_CK_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = I2S_SD_Pin;
	GPIO_Init(I2S_SD_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = I2S_WS_Pin;
	GPIO_Init(I2S_WS_Group, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = Audio_Mute_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  	GPIO_Init(Audio_Mute_Group, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3); //PB3复用为 SPI1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3); //PB4复用为 SPI1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3); //PB5复用为 SPI1
	
	SetAudioMode(Mute_Mode);
}

void SetAudioMode(AudioMode mode)
{
	if(Audio_Mode == mode)
		GPIO_SetBits(Audio_Mute_Group, Audio_Mute_Pin);
	else
		GPIO_ResetBits(Audio_Mute_Group, Audio_Mute_Pin);
}

/***************************************************************************************************
*FunctionName：ConfigI2S
*Description：根据读取的音频信息配置I2S
*Input：audiofreq -- 采样频率
*Output：
*Author：
*Data：
***************************************************************************************************/
void ConfigI2S(unsigned short audiofreq)
{
  	I2S_InitTypeDef   I2S_InitStructure;

 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	
	PT8211_GPIOInit();
	
	SPI_I2S_DeInit(SPI3);  

	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
	I2S_InitStructure.I2S_Standard = I2S_Standard_MSB;
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
	I2S_InitStructure.I2S_AudioFreq = audiofreq;
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	I2S_Init(SPI3, &I2S_InitStructure);
	
	I2S_Cmd(SPI3, ENABLE);
}

void I2S_DMA_Init(unsigned char *buf0, unsigned char *buf1, unsigned short num)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); //??DMA??

	DMA_DeInit(DMA1_Stream5);
		while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){}//等待DMA1_Stream1可配置 	 
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //通道0 SPI2_TX通道 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI3->DR;//外设地址为:(u32)&SPI2->DR
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = num;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度：16位 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 使用循环模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//高优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //不使用FIFO模式        
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//外设突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//存储器突发单次传输
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);//初始化DMA Stream
	DMA_Cmd(DMA1_Stream5,DISABLE);//关闭DMA,结束播放	
			
	DMA_DoubleBufferModeConfig(DMA1_Stream5,(u32)buf1,DMA_Memory_0);//双缓冲模式配置
	 
	DMA_DoubleBufferModeCmd(DMA1_Stream5,ENABLE);//双缓冲模式开启
	 
	DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);//开启传输完成中断
	SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);//SPI2 TX DMA????.
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x4;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
}

void DMA1_Stream5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5)==SET)////DMA1_Stream4,传输完成标志
	{
		DMA_ClearITPendingBit(DMA1_Stream5,DMA_IT_TCIF5);
      	i2s_tx_callback();	//执行回调函数,读取数据等操作在这里面处理  
	}
}

void StartPlay(void)
{
	SetAudioMode(Audio_Mode);
	DMA_Cmd(DMA1_Stream5,ENABLE);//开启DMA TX传输,开始播放 
	
}
void StopPlay(void)
{
	SetAudioMode(Mute_Mode);
	DMA_Cmd(DMA1_Stream5,DISABLE);//关闭DMA,结束播放	
	DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,DISABLE);//开启传输完成中断
	SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,DISABLE);//SPI2 TX DMA????.
}

