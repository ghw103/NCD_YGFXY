/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

#include "SD_SPI_Driver.h"



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	if (pdrv) return STA_NOINIT;		/* Supports only single drive */
	
	if (SD_Init())
		return STA_NOINIT;
	else
	{
		return RES_OK;
	}
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
		if (SD_CardInfo.Type != SD_TYPE_SDHC) // SDHC 以 BLOCK 为单位，其他的以 BYTE 为单位
		{
			sector *= SD_BLOCK_SIZE;
		}
		if (count == 1)
		{
			if (SD_ReadBlock(buff, sector, SD_BLOCK_SIZE)) return RES_ERROR;
		}
		else
		{
			if (SD_ReadMultiBlocks(buff, sector, SD_BLOCK_SIZE, count)) return RES_ERROR;
		}
		return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
		if (SD_CardInfo.Type != SD_TYPE_SDHC) // SDHC 以 BLOCK 为单位，其他的以 BYTE 为单位
		{
			sector *= SD_BLOCK_SIZE;
		}
		if (count == 1)
		{
			if (SD_WriteBlock((uint8_t *)buff, sector, SD_BLOCK_SIZE)) return RES_ERROR;
		}
		else
		{
			if (SD_WriteMultiBlocks((uint8_t *)buff, sector, SD_BLOCK_SIZE, count)) return RES_ERROR;
		}
		return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
		DRESULT res = RES_OK;
		switch (cmd)
		{
			#if !_FS_READONLY
			case CTRL_SYNC:
				if (SD_Assert())
				{
					res = RES_ERROR;
				}
				else
				{
					res = RES_OK;
					SD_DeAssert();
				}
				break;
			#endif
			#if _USE_MKFS && !_FS_READONLY
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = (DWORD)((SD_CardInfo.Capacity) >> 9);
				//SD_TYPE_MMC3 / SD_TYPE_SDV1 算法不一样
				res = RES_OK;
				break;
			case GET_BLOCK_SIZE:
				*(DWORD*)buff = SD_CardInfo.BlockSize;
				res = RES_OK;
				break;
			#endif
			#if _MAX_SS != _MIN_SS
			case GET_SECTOR_SIZE:
				res = RES_OK;
				break;
			#endif
			default:
				res = RES_PARERR;
		}
		return res;
}

DWORD get_fattime (void)
{				 
	return 0;
}	


