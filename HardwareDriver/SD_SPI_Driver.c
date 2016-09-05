/**
  * @file    sd_spi.c
  * @author  Lvehe
  * @version V1.0
  * @date    2015-10-08
  * @brief   SD卡SPI DMA模式，适用于STM32F4。
  *          部分地方将库函数调用优化为寄存器读写。
  */
/* Includes ------------------------------------------------------------------*/
#include "SD_SPI_Driver.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_dma.h"
#include <stdio.h>

#define RESERVED_MASK (uint32_t)0x0F7D0F7D //原定义位于stm32f4xx_dma.c


SD_CARDINFO SD_CardInfo;

static uint8_t Dummy_Send = SD_DUMMY_BYTE;
static uint8_t Dummy_Recv = SD_DUMMY_BYTE;

//SD卡电源配置
void SD_Power_Init(void)
{

}
//SD卡电源控制引脚配置还原
void SD_Power_DeInit(void)
{

}
//SD卡上电(低电平有效)
void SD_PowerOn(void)
{
	//GPIO_ResetBits(SD_POWER_GPIO, SD_POWER_PIN);
	//SD_POWER_GPIO->BSRRH = SD_POWER_PIN;
}
//SD卡断电
void SD_PowerOff(void)
{
	//GPIO_SetBits(SD_POWER_GPIO, SD_POWER_PIN);
	//SD_POWER_GPIO->BSRRL = SD_POWER_PIN;
}
//SD卡检测引脚配置
void SD_Detect_Init(void)
{

	//可配置为触发中断
}
/**
  * @brief  Initializes the SPI and GPIOs resources used to drive the microSD card
  * @param  None
  * @retval None
  */
void SD_LowLevel_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// SD GPIO Periph clock enable
	RCC_AHB1PeriphClockCmd(SD_CS_RCC|SD_MOSI_RCC|SD_MISO_RCC|SD_SCK_RCC, ENABLE);
	// SD_SPI Periph clock enable
	RCC_APB1PeriphClockCmd(SD_SPI_RCC, ENABLE);

	GPIO_PinAFConfig(SD_SCK_GPIO, SD_SCK_SOURCE, SD_SCK_AF);
	GPIO_PinAFConfig(SD_MISO_GPIO, SD_MISO_SOURCE, SD_MISO_AF);
	GPIO_PinAFConfig(SD_MOSI_GPIO, SD_MOSI_SOURCE, SD_MOSI_AF);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	// Configure SD_SPI pins: SCK
	GPIO_InitStructure.GPIO_Pin = SD_SCK_PIN;
	GPIO_Init(SD_SCK_GPIO, &GPIO_InitStructure);

	// Configure SD_SPI pins: MOSI/DI
	GPIO_InitStructure.GPIO_Pin = SD_MOSI_PIN;
	GPIO_Init(SD_MOSI_GPIO, &GPIO_InitStructure);

	// Configure SD_SPI pins: MISO/DO
	GPIO_InitStructure.GPIO_Pin = SD_MISO_PIN;
	GPIO_Init(SD_MISO_GPIO, &GPIO_InitStructure);

	// Configure SD_SPI pins: CS
	GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(SD_CS_GPIO, &GPIO_InitStructure);

	SD_SPI_Config();
	SD_DMA_Config();

	// SD_SPI enable
	SPI_Cmd(SD_SPI, ENABLE);
	SD_CardInfo.Status |= STA_NOINIT;
}
/**
  * @brief  DeInitializes the SPI interface.
  * @param  None
  * @retval None
  */
void SD_LowLevel_DeInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	SD_CardInfo.Status |= STA_NOINIT;
	SPI_Cmd(SD_SPI, DISABLE); // SD_SPI disable
	SPI_I2S_DeInit(SD_SPI); // DeInitializes the SD_SPI
	RCC_APB1PeriphClockCmd(SD_SPI_RCC, DISABLE); // SD_SPI Periph clock disable

	// Configure SD_SPI pins: SCK
	GPIO_InitStructure.GPIO_Pin = SD_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SD_SCK_GPIO, &GPIO_InitStructure);

	// Configure SD_SPI pins: MISO
	GPIO_InitStructure.GPIO_Pin = SD_MISO_PIN;
	GPIO_Init(SD_MISO_GPIO, &GPIO_InitStructure);

	// Configure SD_SPI pins: MOSI
	GPIO_InitStructure.GPIO_Pin = SD_MOSI_PIN;
	GPIO_Init(SD_MOSI_GPIO, &GPIO_InitStructure);

	// Configure SD_SPI pins: CS
	GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
	GPIO_Init(SD_CS_GPIO, &GPIO_InitStructure);
}
/**
  * @brief  Configure SPI of the SD card.
  * @param  None
  * @retval None
  */
void SD_SPI_Config(void)
{
	SPI_InitTypeDef   SPI_InitStructure;

	// SD_SPI Config
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SD_SPI, &SPI_InitStructure);
}
//SPI关联的DMA配置
void SD_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(SD_DMA_RCC, ENABLE);

	DMA_DeInit(SD_DMA_RX_STREAM);
	DMA_DeInit(SD_DMA_TX_STREAM);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SD_SPI->DR));
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //!!!
	DMA_InitStructure.DMA_BufferSize = 0; //!!!
	
	DMA_InitStructure.DMA_Channel = SD_DMA_RX_CHANNEL;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0; //!!!
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_Init(SD_DMA_RX_STREAM, &DMA_InitStructure);

	DMA_InitStructure.DMA_Channel = SD_DMA_TX_CHANNEL;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0; //!!!
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_Init(SD_DMA_TX_STREAM, &DMA_InitStructure);

	SD_SPI->CR2 |= SPI_I2S_DMAReq_Rx;
	SD_SPI->CR2 |= SPI_I2S_DMAReq_Tx;
	//SPI_I2S_DMACmd(SD_SPI, SPI_I2S_DMAReq_Rx, ENABLE);
	//SPI_I2S_DMACmd(SD_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
}
/*
// DMA 启动、等待、清除标志、停止
void SD_DMAProcess(void)
{
	// 启动
	unsigned short waittime = 0;
	
	SD_DMA_RX_STREAM->CR |= DMA_SxCR_EN; //先启动接收
	//DMA_Cmd(SD_DMA_RX_STREAM, ENABLE);
	while(!(SD_DMA_RX_STREAM->CR & DMA_SxCR_EN));
	//while(DMA_GetCmdStatus(SD_DMA_RX_STREAM) == DISABLE);
	SD_DMA_TX_STREAM->CR |= DMA_SxCR_EN; //再启动发送
	//DMA_Cmd(SD_DMA_TX_STREAM, ENABLE);
	while(!(SD_DMA_TX_STREAM->CR & DMA_SxCR_EN))
	{
		waittime++;
		if(waittime > 1000)
			;
	}
	//while(DMA_GetCmdStatus(SD_DMA_TX_STREAM) == DISABLE);
	
	// 等待完成
	while(!((SD_DMA->HISR & RESERVED_MASK) & SD_DMA_TX_TCIF));
	//while(!DMA_GetFlagStatus(SD_DMA_RX_STREAM, SD_DMA_TX_TCIF));
	while(!((SD_DMA->LISR & RESERVED_MASK) & SD_DMA_RX_TCIF));
	//while(!DMA_GetFlagStatus(SD_DMA_RX_STREAM, SD_DMA_RX_TCIF));

	// 清除完成标志
	SD_DMA->LIFCR = SD_DMA_RX_TCIF & RESERVED_MASK; //清除接收完成标志
	//DMA_ClearFlag(SD_DMA_RX_STREAM, SD_DMA_RX_TCIF);
	SD_DMA->HIFCR = SD_DMA_TX_TCIF & RESERVED_MASK; //清除发送完成标志
	//DMA_ClearFlag(SD_DMA_TX_STREAM, SD_DMA_TX_TCIF);
	
	// 停止
	SD_DMA_TX_STREAM->CR &= ~DMA_SxCR_EN;
	//DMA_Cmd(SD_DMA_TX_STREAM, DISABLE);
	while(SD_DMA_TX_STREAM->CR & DMA_SxCR_EN);
	//while(DMA_GetCmdStatus(SD_DMA_TX_STREAM) == ENABLE);
	SD_DMA_RX_STREAM->CR &= ~DMA_SxCR_EN;
	//DMA_Cmd(SD_DMA_RX_STREAM, DISABLE);
	while(SD_DMA_RX_STREAM->CR & DMA_SxCR_EN);
	//while(DMA_GetCmdStatus(SD_DMA_TX_STREAM) == ENABLE);
}*/

static
void stm32_dma_transfer(
   unsigned char receive,      /* FALSE for buff->SPI, TRUE for SPI->buff               */
   const BYTE *buff,   /* receive TRUE  : 512 byte data block to be transmitted
                     receive FALSE : Data buffer to store received data    */
   UINT btr          /* receive TRUE  : Byte count (must be multiple of 2)
                     receive FALSE : Byte count (must be 512)              */
)
{
   DMA_InitTypeDef DMA_InitStructure;
   uint32_t rw_workbyte[] = { 0xffff };

   /* shared DMA configuration values */
   DMA_InitStructure.DMA_Channel = DMA_Channel_0;
   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SD_SPI->DR));
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_BufferSize = btr;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
   DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
   DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_Full;
   DMA_InitStructure.DMA_MemoryBurst          = DMA_MemoryBurst_Single;
   DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;

   DMA_DeInit(SD_DMA_RX_STREAM);
   DMA_DeInit(SD_DMA_TX_STREAM);

   if ( receive ) {

      /* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
	  DMA_InitStructure.DMA_Channel = SD_DMA_RX_CHANNEL;
      DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buff;
      DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
      DMA_Init(SD_DMA_RX_STREAM, &DMA_InitStructure);

      /* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
	   DMA_InitStructure.DMA_Channel = SD_DMA_TX_CHANNEL;
      DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rw_workbyte;
      DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
      DMA_Init(SD_DMA_TX_STREAM, &DMA_InitStructure);

   } else {

      /* DMA1 channel2 configuration SPI2 RX ---------------------------------------------*/
	   DMA_InitStructure.DMA_Channel = SD_DMA_RX_CHANNEL;
      DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rw_workbyte;
      DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
      DMA_Init(SD_DMA_RX_STREAM, &DMA_InitStructure);

      /* DMA1 channel3 configuration SPI2 TX ---------------------------------------------*/
	   DMA_InitStructure.DMA_Channel = SD_DMA_TX_CHANNEL;
      DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buff;
      DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
      DMA_Init(SD_DMA_TX_STREAM, &DMA_InitStructure);

   }

   /* Enable DMA1 Channel4 */
   DMA_Cmd(SD_DMA_RX_STREAM, ENABLE);
   /* Enable DMA1 Channel5 */
   DMA_Cmd(SD_DMA_TX_STREAM, ENABLE);

   /* Enable SPI2 TX/RX request */
   SPI_I2S_DMACmd(SD_SPI, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

   /* Wait until DMA1_Channel 5 Transfer Complete */
   // not needed: while (DMA_GetFlagStatus(DMA1_FLAG_TC3) == RESET) { ; }
   /* Wait until DMA1_Channel 4 Receive Complete */
   while (DMA_GetFlagStatus(SD_DMA_RX_STREAM,DMA_FLAG_TCIF3) == RESET) { ; }

   /* Disable DMA1 Channel4 */
   DMA_Cmd(SD_DMA_TX_STREAM, DISABLE);
   /* Disable DMA1 Channel5 */
   DMA_Cmd(SD_DMA_RX_STREAM, DISABLE);

   /* Disable SPI1 RX/TX request */
   SPI_I2S_DMACmd(SD_SPI, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);

   DMA_ClearFlag(SD_DMA_RX_STREAM,DMA_FLAG_TCIF3);
   DMA_ClearFlag(SD_DMA_TX_STREAM,DMA_FLAG_TCIF4);
}

//设置SPI速度 SPI_BaudRatePrescaler_2 ~ SPI_BaudRatePrescaler_256 => (0~7)*8
void SD_SpeedSet(uint16_t speed)
{
	SD_SPI->CR1 = ((uint16_t)(SD_SPI->CR1 & 0xFFC7)) | (speed & 0x0038);
	SD_SPI->CR1 |= SPI_CR1_SPE;
	//SPI_Cmd(SD_SPI, ENABLE);
}
/**
* @brief  Detect if SD card is correctly plugged in the memory slot.
* @param  None
* @retval Return if SD is detected or not
*/
uint8_t SD_Detect(void)
{
	__IO uint8_t status = SD_PRESENT;

	return status;
}

/**
* @brief  DeInitializes the SD/SD communication.
* @param  None
* @retval None
*/
void SD_DeInit(void)
{
	SD_LowLevel_DeInit();
}

/**
* @brief  Initializes the SD/SD communication.
* @param  None
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Sequence failed
*         - SD_RESPONSE_NO_ERROR: Sequence succeed
*/
SD_Error SD_Init(void)
{
	uint32_t i;
	SD_Error res;

/*	if (SD_CardInfo.Status & STA_NODISK) return SD_RESPONSE_FAILURE;
	//接触不良可能导致 SD 卡检测失效, 故此处不检测, 可在调用此函数前自由决定是否检测
	if (SD_Detect() == SD_NOT_PRESENT) return SD_RESPONSE_FAILURE;
	SD_LowLevel_Init();
*/
	//已经初始化则不再重复初始化过程
	if ((SD_CardInfo.Status & STA_NOINIT) == 0x00) return SD_RESPONSE_NO_ERROR;
	
	//开始初始化
	SD_CardInfo.Type = SD_TYPE_UNKNOW;
	SD_SpeedSet(SPI_BaudRatePrescaler_256); //Low speed

	SD_CS_HIGH(); // SD chip select high
	// Rise CS and MOSI for 80(>74) clocks cycles
	for (i = 0; i < 10; i++)
	{
		SD_ReadByte();
	}

	// Send CMD0 to put SD in SPI mode
	res = SD_CmdRsp(SD_CMD_GO_IDLE_STATE, 0, 0x95);
	if (res != SD_IN_IDLE_STATE) return SD_RESPONSE_FAILURE;

	// CMD0 switching period is within 8 clocks after the end bit
	// of the CMD0 command R1 response
	SD_ReadByte(); //提高可靠性
	SD_ReadByte();
	// SDv2 及以后版本支持 CMD8, IF=Interface
	res = SD_CmdRsp(SD_CMD_IF_COND, 0x01AA, 0x87);
	if (res == SD_IN_IDLE_STATE)
	{	// SDv2 Card or later
		uint8_t ocr[4]; //OCR (Operation Conditions Register)
		ocr[0] = SD_ReadByte();
		ocr[1] = SD_ReadByte();
		ocr[2] = SD_ReadByte();
		ocr[3] = SD_ReadByte();
		if (ocr[2] == 0x01 && ocr[3] == 0xAA) //电压范围2.7V~3.6V
		{
			i = INIT_RETRY_MAX; // wait up to 2s
			do // 初始化
			{
				res = SD_CmdRsp(SD_CMD_APP_CMD, 0, 0xFF); // CMD55
				if (res == SD_IN_IDLE_STATE)
				{
					res = SD_CmdRsp(41, 0x40000000, 0xFF); // ACMD41 注意参数
				}
			}
			while ((res != SD_RESPONSE_NO_ERROR) && --i);
			if(i)
			{
				// CMD58 读取 OCR
				res = SD_CmdRsp(SD_CMD_READ_OCR, 0, 0xFF);
				if (res == SD_RESPONSE_NO_ERROR)
				{
					ocr[0] = SD_ReadByte();
					ocr[1] = SD_ReadByte();
					ocr[2] = SD_ReadByte();
					ocr[3] = SD_ReadByte();
					if (ocr[0] & 0x40) // CCS bit (Card Capacity Status)
					{
						SD_CardInfo.Type = SD_TYPE_SDHC; // or SDXC
					}
					else
					{
						SD_CardInfo.Type = SD_TYPE_SDV2;
					}
				}
			}
		}
	}
	else // CMD8 其他响应
	{
		i = INIT_RETRY_MAX;
		do // 初始化
		{
			res = SD_CmdRsp(SD_CMD_APP_CMD, 0, 0xFF); // CMD55
			if (res == SD_IN_IDLE_STATE)
			{
				res = SD_CmdRsp(41, 0x00, 0xFF); // ACMD41 参数与 SDv2 不同
			}
		}
		while ((res != SD_RESPONSE_NO_ERROR) && --i);
		if (i) // 有回应是 SDv1, 且初始化完成
		{
			SD_CardInfo.Type = SD_TYPE_SDV1;
		}
		else // 没有回应是 MMC
		{
			i = INIT_RETRY_MAX;
			do // 初始化
			{
				res = SD_CmdRsp(SD_CMD_SEND_OP_COND, 0x00, 0xFF); //CMD1 参数与 SDv2 不同
			}
			while ((res != SD_RESPONSE_NO_ERROR) && --i);
			if (i) SD_CardInfo.Type = SD_TYPE_MMC3;
		}
	}
	if (SD_CardInfo.Type != SD_TYPE_UNKNOW)
	{
		SD_CardInfo.Status &= ~STA_NOINIT; //初始化成功
		// CMD59 关闭 CRC 校验
		SD_CmdRsp(SD_CMD_CRC_ON_OFF, 0x00, 0xFF);

		// CMD16 设置 BLOCK 为 512 Bytes (对 SDHC/SDHX 无效)
		SD_CmdRsp(SD_CMD_SET_BLOCKLEN, SD_BLOCK_SIZE, 0xFF);
		
		res = SD_RESPONSE_NO_ERROR;
	}
	else
	{
		res = SD_RESPONSE_FAILURE;
	}
	SD_DeAssert();
	//根据SD卡和硬件设计情况确定 SPI_BaudRatePrescaler_4
	SD_SpeedSet(SPI_BaudRatePrescaler_2);

	return res;
}

/**
* @brief  Returns information about specific card.
* @param  CardInfo: pointer to a SD_CARDINFO structure that contains all SD
*         card information.
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Sequence failed
*         - SD_RESPONSE_NO_ERROR: Sequence succeed
*/
SD_Error SD_GetCardInfo(SD_CARDINFO *CardInfo)
{
	SD_Error res;
	SD_Type sd_type = CardInfo->Type;

	res = SD_GetCSDRegister(&(CardInfo->CSD));
	res = SD_GetCIDRegister(&(CardInfo->CID));

	if (sd_type == SD_TYPE_SDHC)
	{
		CardInfo->Capacity = ((uint64_t)CardInfo->CSD.DeviceSize + 1) * 512 * 1024;
        CardInfo->BlockSize = 512;
	}
	else if (sd_type == SD_TYPE_SDV2 || sd_type == SD_TYPE_SDV1)
	{
		CardInfo->Capacity = (CardInfo->CSD.DeviceSize + 1);
		CardInfo->Capacity *= (1 << (CardInfo->CSD.DeviceSizeMul + 2));
		CardInfo->BlockSize = 1 << (CardInfo->CSD.RdBlockLen);
		CardInfo->Capacity *= CardInfo->BlockSize;
	}

	return res;
}

/**
* @brief  Read the CSD card register.
*         Reading the contents of the CSD register in SPI mode is a simple
*         read-block transaction.
* @param  sd_csd: pointer on an SCD register structure
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Sequence failed
*         - SD_RESPONSE_NO_ERROR: Sequence succeed
*/
SD_Error SD_GetCSDRegister(SD_CSD* sd_csd)
{
	uint32_t i = 0;
	SD_Error res;
	uint8_t CSD_Tab[16]; // Card Specific Data
	SD_Type sd_type = SD_CardInfo.Type;

	// Send CMD9 (CSD register)
	res = SD_CmdRsp(SD_CMD_SEND_CSD, 0, 0xFF);
	if (!res)
	{	// Wait for response in the R1 format (0x00 is no errors)
		res = SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ);
		if (!res)
		{
			for (i = 0; i < 16; i++) // Store CSD register value on CSD_Tab
			{
				CSD_Tab[i] = SD_ReadByte();
			}
			// Get CRC bytes (not really needed by us, but required by SD)
			SD_ReadByte();
			SD_ReadByte();
		}
		SD_DeAssert();
	}
	
	/*!< Byte 0 */
	sd_csd->CSDStruct = (CSD_Tab[0] & 0xC0) >> 6;
	sd_csd->SysSpecVersion = (CSD_Tab[0] & 0x3C) >> 2;
	sd_csd->Reserved1 = CSD_Tab[0] & 0x03;

	/*!< Byte 1 */
	sd_csd->TAAC = CSD_Tab[1];

	/*!< Byte 2 */
	sd_csd->NSAC = CSD_Tab[2];

	/*!< Byte 3 */
	sd_csd->MaxBusClkFrec = CSD_Tab[3];

	/*!< Byte 4 */
	sd_csd->CardComdClasses = CSD_Tab[4] << 4;

	/*!< Byte 5 */
	sd_csd->CardComdClasses |= (CSD_Tab[5] & 0xF0) >> 4;
	sd_csd->RdBlockLen = CSD_Tab[5] & 0x0F;

	/*!< Byte 6 */
	sd_csd->PartBlockRead = (CSD_Tab[6] & 0x80) >> 7;
	sd_csd->WrBlockMisalign = (CSD_Tab[6] & 0x40) >> 6;
	sd_csd->RdBlockMisalign = (CSD_Tab[6] & 0x20) >> 5;
	sd_csd->DSRImpl = (CSD_Tab[6] & 0x10) >> 4;
	sd_csd->Reserved2 = 0; /*!< Reserved */

	if (sd_type == SD_TYPE_SDHC) // or SDXC CSD v2.0
	{
		/*!< Byte 7 */
        sd_csd->DeviceSize = (CSD_Tab[7] & 0x3F) << 16;

        /*!< Byte 8 */
        sd_csd->DeviceSize |= (CSD_Tab[8] << 8);

        /*!< Byte 9 */
        sd_csd->DeviceSize |= (CSD_Tab[9]);
	}
	else if (sd_type == SD_TYPE_SDV1 || sd_type == SD_TYPE_SDV2) //CSD v1.0
	{
		sd_csd->DeviceSize = (CSD_Tab[6] & 0x03) << 10;
		/*!< Byte 7 */
		sd_csd->DeviceSize |= (CSD_Tab[7]) << 2;

		/*!< Byte 8 */
		sd_csd->DeviceSize |= (CSD_Tab[8] & 0xC0) >> 6;

		sd_csd->MaxRdCurrentVDDMin = (CSD_Tab[8] & 0x38) >> 3;
		sd_csd->MaxRdCurrentVDDMax = (CSD_Tab[8] & 0x07);

		/*!< Byte 9 */
		sd_csd->MaxWrCurrentVDDMin = (CSD_Tab[9] & 0xE0) >> 5;
		sd_csd->MaxWrCurrentVDDMax = (CSD_Tab[9] & 0x1C) >> 2;
		sd_csd->DeviceSizeMul = (CSD_Tab[9] & 0x03) << 1;

		/*!< Byte 10 */
		sd_csd->DeviceSizeMul |= (CSD_Tab[10] & 0x80) >> 7;
	}

	sd_csd->EraseGrSize = (CSD_Tab[10] & 0x40) >> 6;
	sd_csd->EraseGrMul = (CSD_Tab[10] & 0x3F) << 1;

	/*!< Byte 11 */
	sd_csd->EraseGrMul |= (CSD_Tab[11] & 0x80) >> 7;
	sd_csd->WrProtectGrSize = (CSD_Tab[11] & 0x7F);

	/*!< Byte 12 */
	sd_csd->WrProtectGrEnable = (CSD_Tab[12] & 0x80) >> 7;
	sd_csd->ManDeflECC = (CSD_Tab[12] & 0x60) >> 5;
	sd_csd->WrSpeedFact = (CSD_Tab[12] & 0x1C) >> 2;
	sd_csd->MaxWrBlockLen = (CSD_Tab[12] & 0x03) << 2;

	/*!< Byte 13 */
	sd_csd->MaxWrBlockLen |= (CSD_Tab[13] & 0xC0) >> 6;
	sd_csd->WriteBlockPaPartial = (CSD_Tab[13] & 0x20) >> 5;
	sd_csd->Reserved3 = 0;
	sd_csd->ContentProtectAppli = (CSD_Tab[13] & 0x01);

	/*!< Byte 14 */
	sd_csd->FileFormatGroup = (CSD_Tab[14] & 0x80) >> 7;
	sd_csd->CopyFlag = (CSD_Tab[14] & 0x40) >> 6;
	sd_csd->PermWrProtect = (CSD_Tab[14] & 0x20) >> 5;
	sd_csd->TempWrProtect = (CSD_Tab[14] & 0x10) >> 4;
	sd_csd->FileFormat = (CSD_Tab[14] & 0x0C) >> 2;
	sd_csd->ECC = (CSD_Tab[14] & 0x03);

	/*!< Byte 15 */
	sd_csd->CSD_CRC = (CSD_Tab[15] & 0xFE) >> 1;
	sd_csd->Reserved4 = 1;

	/*!< Return the reponse */
	return res;
}

/**
* @brief  Read the CID card register.
*         Reading the contents of the CID register in SPI mode is a simple
*         read-block transaction.
* @param  sd_cid: pointer on an CID register structure
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Sequence failed
*         - SD_RESPONSE_NO_ERROR: Sequence succeed
*/
SD_Error SD_GetCIDRegister(SD_CID* sd_cid)
{
	uint32_t i;
	SD_Error res;
	uint8_t CID_Tab[16];

	// Send CMD10 (CID register)
	res = SD_CmdRsp(SD_CMD_SEND_CID, 0, 0xFF);
	if (!res)
	{
		res = SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ);
		if (!res)
		{
			for (i = 0; i < 16; i++) // Store CID register value on CID_Tab
			{
				CID_Tab[i] = SD_ReadByte();
			}
			// Get CRC bytes (not really needed by us, but required by SD)
			SD_ReadByte();
			SD_ReadByte();
		}
		SD_DeAssert();
	}
	
	/*!< Byte 0 */
	sd_cid->ManufacturerID = CID_Tab[0];

	/*!< Byte 1 */
	sd_cid->OEM_AppliID = CID_Tab[1] << 8;

	/*!< Byte 2 */
	sd_cid->OEM_AppliID |= CID_Tab[2];

	/*!< Byte 3 */
	sd_cid->ProdName1 = CID_Tab[3] << 24;

	/*!< Byte 4 */
	sd_cid->ProdName1 |= CID_Tab[4] << 16;

	/*!< Byte 5 */
	sd_cid->ProdName1 |= CID_Tab[5] << 8;

	/*!< Byte 6 */
	sd_cid->ProdName1 |= CID_Tab[6];

	/*!< Byte 7 */
	sd_cid->ProdName2 = CID_Tab[7];

	/*!< Byte 8 */
	sd_cid->ProdRev = CID_Tab[8];

	/*!< Byte 9 */
	sd_cid->ProdSN = CID_Tab[9] << 24;

	/*!< Byte 10 */
	sd_cid->ProdSN |= CID_Tab[10] << 16;

	/*!< Byte 11 */
	sd_cid->ProdSN |= CID_Tab[11] << 8;

	/*!< Byte 12 */
	sd_cid->ProdSN |= CID_Tab[12];

	/*!< Byte 13 */
	sd_cid->Reserved1 |= (CID_Tab[13] & 0xF0) >> 4;
	sd_cid->ManufactDate = (CID_Tab[13] & 0x0F) << 8;

	/*!< Byte 14 */
	sd_cid->ManufactDate |= CID_Tab[14];

	/*!< Byte 15 */
	sd_cid->CID_CRC = (CID_Tab[15] & 0xFE) >> 1;
	sd_cid->Reserved2 = 1;

	/*!< Return the reponse */
	return res;
}

/**
* @brief  Reads a block of data from the SD.
* @param  pBuffer: pointer to the buffer that receives the data read from the
*                  SD.
* @param  ReadAddr: SD's internal address to read from.
* @param  BlockSize: the SD card Data block size.
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Sequence failed
*         - SD_RESPONSE_NO_ERROR: Sequence succeed
*/
SD_Error SD_ReadBlock(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize)
{
	uint32_t i;
	SD_Error res;

	// Send CMD17 to read single block
	res = SD_CmdRsp(SD_CMD_READ_SINGLE_BLOCK, ReadAddr, 0xFF);
	if (!res)
	{
		// Now look for the data token(0xFE) to signify the start of the data
		i = READ_BLOCK_MAX; //200ms
		while ((SD_ReadByte() != SD_START_DATA_SINGLE_BLOCK_READ) && --i);
		if (i)
		{
/*			// 设置接收通道
			SD_DMA_RX_STREAM->CR |= DMA_MemoryInc_Enable; //自增
			SD_DMA_RX_STREAM->M0AR = (uint32_t)pBuffer;
			SD_DMA_RX_STREAM->NDTR = BlockSize;
			//DMA_SetCurrDataCounter(SD_DMA_RX_STREAM, BlockSize);
			
			// 设置发送通道
			SD_DMA_TX_STREAM->CR &= ~DMA_MemoryInc_Enable; //不自增
			SD_DMA_TX_STREAM->M0AR = (uint32_t)&Dummy_Send;
			SD_DMA_TX_STREAM->NDTR = BlockSize;
			//DMA_SetCurrDataCounter(SD_DMA_TX_STREAM, BlockSize);
			
			SD_DMAProcess();
*/			
			stm32_dma_transfer(1, pBuffer, BlockSize);

			// Get CRC bytes (not really needed by us, but required by SD)
			SD_ReadByte();
			SD_ReadByte();
			// Set response value to success
			res = SD_RESPONSE_NO_ERROR;
		}
		else
		{
			res = SD_RESPONSE_FAILURE;
		}
		SD_DeAssert();
	}
	
	return res;
}

/**
* @brief  Reads multiple block of data from the SD.
* @param  pBuffer: pointer to the buffer that receives the data read from the
*                  SD.
* @param  ReadAddr: SD's internal address to read from.
* @param  BlockSize: the SD card Data block size.
* @param  NumberOfBlocks: number of blocks to be read.
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Sequence failed
*         - SD_RESPONSE_NO_ERROR: Sequence succeed
*/
SD_Error SD_ReadMultiBlocks(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
	uint32_t i, j;
	SD_Error res;

	// Send CMD18 to read multiple blocks
	res = SD_CmdRsp(SD_CMD_READ_MULTIPLE_BLOCK, ReadAddr, 0xFF);
	if (!res)
	{
		// 设置接收通道
		SD_DMA_RX_STREAM->CR |= DMA_MemoryInc_Enable; //自增
		// 设置发送通道
		SD_DMA_TX_STREAM->M0AR = (uint32_t)&Dummy_Send;
		SD_DMA_TX_STREAM->CR &= ~DMA_MemoryInc_Enable; //不自增
		
		for (i = 0; i < NumberOfBlocks; i++)
		{
			// Now look for the data token(0xFE) to signify the start of the data
			j = READ_BLOCK_MAX; //200ms
			while ((SD_ReadByte() != SD_START_DATA_MULTIPLE_BLOCK_READ) && --j);
			if (j > 0)
			{
/*				// 设置接收通道
				SD_DMA_RX_STREAM->M0AR = (uint32_t)(pBuffer) + i*BlockSize;
				SD_DMA_RX_STREAM->NDTR = BlockSize;
				// 设置发送通道
				SD_DMA_TX_STREAM->NDTR = BlockSize;

				SD_DMAProcess();
*/
				stm32_dma_transfer(1, pBuffer + i*BlockSize, BlockSize);
				// Get CRC bytes (not really needed by us, but required by SD)
				SD_ReadByte();
				SD_ReadByte();
				// Set response value to success
				res = SD_RESPONSE_NO_ERROR;
			}
			else
			{
				res = SD_RESPONSE_FAILURE; // Set response value to failure
			}
		}
		// CMD12 停止传输
		if (!SD_SendCmd(SD_CMD_STOP_TRANSMISSION, 0x00, 0xFF))
		{
			SD_ReadByte(); //R1b
			SD_GetResponse(SD_RESPONSE_NO_ERROR);
		}
		SD_DeAssert();
	}

	return res;
}

/**
* @brief  Writes a block on the SD
* @param  pBuffer: pointer to the buffer containing the data to be written on
*                  the SD.
* @param  WriteAddr: address to write on.
* @param  BlockSize: the SD card Data block size.
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Sequence failed
*         - SD_RESPONSE_NO_ERROR: Sequence succeed
*/
SD_Error SD_WriteBlock(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t BlockSize)
{
	SD_Error res;
	// Send CMD24 to write single block
	res = SD_CmdRsp(SD_CMD_WRITE_SINGLE_BLOCK, WriteAddr, 0xFF);
	if (!res)
	{
		res = SD_WaitReady();
		if (!res)
		{
			// Send the data token(0xFE) to signify the start of the data
			SD_WriteByte(SD_START_DATA_SINGLE_BLOCK_WRITE);
			
/*			// Write the block data to SD
			// 设置接收通道
			SD_DMA_RX_STREAM->CR &= ~DMA_MemoryInc_Enable; //不自增
			SD_DMA_RX_STREAM->M0AR = (uint32_t)&Dummy_Recv;
			SD_DMA_RX_STREAM->NDTR = BlockSize;
			// 设置发送通道
			SD_DMA_TX_STREAM->CR |= DMA_MemoryInc_Enable; //自增
			SD_DMA_TX_STREAM->M0AR = (uint32_t)pBuffer;
			SD_DMA_TX_STREAM->NDTR = BlockSize;
			
			SD_DMAProcess();
*/			
			stm32_dma_transfer(0, pBuffer, BlockSize);
			// Put CRC bytes (not really needed by us, but required by SD)
			SD_ReadByte();
			SD_ReadByte();
			// 读取数据响应
			if ((SD_ReadByte() & 0x15) == SD_DATA_OK)
			{
				res = SD_RESPONSE_NO_ERROR;
			}
			else
			{
				res = SD_RESPONSE_FAILURE;
			}
		}
	}
	else
	{	// 有时 SD 卡已发送响应，但单片机的 SPI 无法正确解析。
		// 此时 SD 卡处于接收数据状态，很可能将未知数据写入 SD 卡。
		// 为保险起见，强制发送 CMD12 停止传输，使 SD 卡重新进入传输状态。
		if (!SD_SendCmd(SD_CMD_STOP_TRANSMISSION, 0x00, 0xFF))
		{
			SD_ReadByte();
			SD_GetResponse(SD_RESPONSE_NO_ERROR);
		}
	}
	SD_DeAssert();
	return res;
}

/**
* @brief  Writes many blocks on the SD
* @param  pBuffer: pointer to the buffer containing the data to be written on
*                  the SD.
* @param  WriteAddr: address to write on.
* @param  BlockSize: the SD card Data block size.
* @param  NumberOfBlocks: number of blocks to be written.
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Sequence failed
*         - SD_RESPONSE_NO_ERROR: Sequence succeed
*/
SD_Error SD_WriteMultiBlocks(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
	uint32_t i;
	SD_Error res;

	if (SD_CardInfo.Type > SD_TYPE_MMC3) // 写入预擦除命令
	{	// CMD55 Application-Specific Command
		if (!SD_CmdRsp(SD_CMD_APP_CMD, 0x00, 0xFF))
		{	// ACMD23 设置预擦除块的个数
			SD_CmdRsp(SD_CMD_SET_BLOCK_COUNT, NumberOfBlocks, 0xFF);
		}
	}

	// Send CMD25 to write multiple blocks
	res = SD_CmdRsp(SD_CMD_WRITE_MULTIPLE_BLOCK, WriteAddr, 0xFF);
	if (!res)
	{
/*		// 设置接收通道
		SD_DMA_RX_STREAM->M0AR = (uint32_t)&Dummy_Recv;
		SD_DMA_RX_STREAM->CR &= ~DMA_MemoryInc_Enable; //不自增
		// 设置发送通道
		SD_DMA_TX_STREAM->CR |= DMA_MemoryInc_Enable; //自增
	*/	
		for (i = 0; i < NumberOfBlocks; i++)
		{
			if (SD_WaitReady()) break;
			// Send the data token(0xFC) to signify the start of the data
			SD_WriteByte(SD_START_DATA_MULTIPLE_BLOCK_WRITE);
			
/*			// 设置接收通道
			SD_DMA_RX_STREAM->NDTR = BlockSize;
			// 设置发送通道
			SD_DMA_TX_STREAM->M0AR = (uint32_t)(pBuffer) + i*BlockSize;
			SD_DMA_TX_STREAM->NDTR = BlockSize;
			
			SD_DMAProcess();
	*/
			stm32_dma_transfer(0, pBuffer + i*BlockSize, BlockSize);			
			// Put CRC bytes (not really needed by us, but required by SD)
			SD_ReadByte();
			SD_ReadByte();
			// 读取数据响应 if (SD_GetDataResponse() != SD_DATA_OK)
			if ((SD_ReadByte() & 0x1F) != SD_DATA_OK) break;
		}
		if (i == NumberOfBlocks)
		{
			res = SD_RESPONSE_NO_ERROR;
		}
		else
		{
			res = SD_RESPONSE_FAILURE;
		}
		if (SD_WaitReady()) res = SD_RESPONSE_FAILURE;
		// Send the data token(0xFD) to signify the end of the data
		SD_WriteByte(SD_STOP_DATA_MULTIPLE_BLOCK_WRITE);
	}
	else
	{	// 有时 SD 卡已发送响应，但单片机的 SPI 无法正确解析。
		// 此时 SD 卡处于接收数据状态，很可能将未知数据写入 SD 卡。
		// 为保险起见，强制发送 CMD12 停止传输，使 SD 卡重新进入传输状态???
		SD_WaitReady();
		// Send the data token(0xFD) to signify the end of the data
		SD_WriteByte(SD_STOP_DATA_MULTIPLE_BLOCK_WRITE);
		if (!SD_SendCmd(SD_CMD_STOP_TRANSMISSION, 0x00, 0xFF))
		{
			SD_ReadByte();
			SD_GetResponse(SD_RESPONSE_NO_ERROR);
		}
	}
	SD_DeAssert();
	return res;
}
/*
//代替方案：连续调用SD_WriteBlock，效率较低
SD_Error SD_WriteMultiBlocks(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
	uint32_t i;

	if (SD_CardInfo.Type != SD_TYPE_SDHC) // SDHC 以 BLOCK 为单位，其他的以 BYTE 为单位
	{
		for (i = 0; i < NumberOfBlocks; i++)
		{
			if (SD_WriteBlock(pBuffer + BlockSize * i, WriteAddr, BlockSize))
			{
				return SD_RESPONSE_FAILURE;
			}
			WriteAddr += BlockSize;
		}
	}
	else
	{
		for (i = 0; i < NumberOfBlocks; i++)
		{
			if (SD_WriteBlock(pBuffer + BlockSize * i, WriteAddr, BlockSize))
			{
				return SD_RESPONSE_FAILURE;
			}
			WriteAddr++;
		}
	}
	return SD_RESPONSE_NO_ERROR;
}
*/
/**
* @brief  Send 5 bytes command to the SD card.
* @param  Cmd: The user expected command to send to SD card.
* @param  Arg: The command argument.
* @param  Crc: The CRC.
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Failed for timeout
*         - SD_RESPONSE_NO_ERROR: Succeed
*/
SD_Error SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc)
{
	uint32_t i;
	uint8_t Frame[6];

	SD_DeAssert();
	if (SD_Assert()) return SD_RESPONSE_FAILURE;

	Frame[0] = (Cmd | 0x40); // Construct byte 1
	Frame[1] = (uint8_t)(Arg >> 24); // Construct byte 2
	Frame[2] = (uint8_t)(Arg >> 16); // Construct byte 3
	Frame[3] = (uint8_t)(Arg >> 8); // Construct byte 4
	Frame[4] = (uint8_t)(Arg); // Construct byte 5
	Frame[5] = (Crc); // Construct CRC: byte 6

	for (i = 0; i < 6; i++)
	{
		SD_WriteByte(Frame[i]); // Send the Cmd bytes
	}

	return SD_RESPONSE_NO_ERROR;
}
/**
* @brief  Send 5 bytes command to the SD card.
* @param  Cmd: The user expected command to send to SD card.
* @param  Arg: The command argument.
* @param  Crc: The CRC.
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Failed for timeout
*         - other: Response from SD card
*/
SD_Error SD_CmdRsp(uint8_t Cmd, uint32_t Arg, uint8_t Crc)
{
	uint32_t i;
	SD_Error res = SD_RESPONSE_FAILURE;
	uint8_t Frame[6];

	SD_DeAssert();
	if (SD_Assert()) return SD_RESPONSE_FAILURE;

	Frame[0] = (Cmd | 0x40); // Construct byte 1
	Frame[1] = (uint8_t)(Arg >> 24); // Construct byte 2
	Frame[2] = (uint8_t)(Arg >> 16); // Construct byte 3
	Frame[3] = (uint8_t)(Arg >> 8); // Construct byte 4
	Frame[4] = (uint8_t)(Arg); // Construct byte 5
	Frame[5] = (Crc); // Construct CRC: byte 6

	for (i = 0; i < 6; i++)
	{
		SD_WriteByte(Frame[i]); // Send the Cmd bytes
	}

	i = 10; // 尝试 10 次，规定发送命令后等待响应时间Ncr为 1~8 次
	do
	{
		res = (SD_Error)SD_ReadByte();
	}while ((res & 0x80) && --i);
	
	return res;
}

/**
* @brief  Returns the SD response.
* @param  Response: response want to receive
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Sequence failed
*         - SD_RESPONSE_NO_ERROR: Sequence succeed
*/
SD_Error SD_GetResponse(uint8_t Response)
{
	uint32_t i = 10; // 尝试 10 次，规定发送命令后等待响应时间Ncr为 1~8 次
	// Check if response is got or a timeout is happen
	while ((SD_ReadByte() != Response) && --i);

	if (i)
	{
		return SD_RESPONSE_NO_ERROR; // Right response got
	}
	else
	{
		return SD_RESPONSE_FAILURE; // After time out
	}
}

/**
* @brief  Get SD card data response.
* @param  None
* @retval The SD status: Read data response xxx0<status>1
*         - status 010: Data accepted
*         - status 101: Data rejected due to a crc error
*         - status 110: Data rejected due to a Write error.
*         - status 111: Data rejected due to other error.
*/
uint8_t SD_GetDataResponse(void)
{
	uint8_t res;

	res = SD_ReadByte(); // Read response
	switch (res & 0x1F) // Mask unused bits
	{
		case SD_DATA_OK:
			return SD_DATA_OK;
		case SD_DATA_CRC_ERROR:
			return SD_DATA_CRC_ERROR;
		case SD_DATA_WRITE_ERROR:
			return SD_DATA_WRITE_ERROR;
		default:
			return res;
	}
}

/**
* @brief  Returns the SD status.
* @param  None
* @retval The SD status.
*/
uint16_t SD_GetStatus(void)
{
	uint16_t Status = 0xFFFF;

	// Send CMD13 to get SD status
	if (!SD_CmdRsp(SD_CMD_SEND_STATUS, 0x00, 0xFF))
	{
		Status = SD_RESPONSE_NO_ERROR;
		Status |= (uint16_t)(SD_ReadByte() << 8);
		SD_DeAssert();
	}

	return Status;
}


/**
* @brief  Write a byte on the SD.
* @param  Data: byte to send.
* @retval None
*/
uint8_t SD_WriteByte(uint8_t Data)
{
	while ((SD_SPI->SR & SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SD_SPI, Data);
	
	while ((SD_SPI->SR & SPI_I2S_FLAG_RXNE) == RESET);
	return (uint8_t)SPI_I2S_ReceiveData(SD_SPI);
}

/**
* @brief  Read a byte from the SD.
* @param  None
* @retval The received byte.
*/
uint8_t SD_ReadByte(void)
{
	while ((SD_SPI->SR & SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SD_SPI, SD_DUMMY_BYTE);
	
	while ((SD_SPI->SR & SPI_I2S_FLAG_RXNE) == RESET);
	return (uint8_t)SPI_I2S_ReceiveData(SD_SPI);
}

/**
* @brief  De-assert SD card
* @param  None
* @retval None
*/
void SD_DeAssert(void)
{
	SD_CS_HIGH(); // SD chip select high
	SD_ReadByte(); // Send dummy byte 0xFF
}

/**
* @brief  Wait for SD card to be asserted
* @param  None
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Failed for timeout
*         - SD_RESPONSE_NO_ERROR: Succeed
*/
SD_Error SD_Assert(void)
{
	SD_CS_LOW();
	if (!SD_WaitReady()) return SD_RESPONSE_NO_ERROR;
	
	SD_DeAssert();
	return SD_RESPONSE_FAILURE;
}

/**
* @brief  Wait for SD card to not busy status
* @param  None
* @retval The SD Response:
*         - SD_RESPONSE_FAILURE: Failed for timeout
*         - SD_RESPONSE_NO_ERROR: Succeed
*/
SD_Error SD_WaitReady(void)
{
	uint32_t i = WAIT_READY_MAX;
	//500ms, equal to 250 000 if SD_ReadByte() costs 2us
	while ((SD_ReadByte() != 0xFF) && --i);

	if (i)
	{
		return SD_RESPONSE_NO_ERROR;
	}
	else
	{
		return SD_RESPONSE_FAILURE;
	}
}
