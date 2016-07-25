/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "MMC_SD.h"



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
	u8 res=0;	    
	switch(pdrv)
	{
		case 0://SD?
			res = SD_Initialize();//SD_Initialize() 
		 	if(res)//STM32 SPI?bug,?sd??????????????????,????SPI????
			{
				SD_SPI_ReadWriteByte(0xff);
			}
  			break;
		case 1:
			res=1;
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //?????
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
	u8 res=0; 
    if (!count)return RES_PARERR;//count????0,????????		 	 
	switch(pdrv)
	{
		case 0://SD?
			res=SD_ReadDisk(buff,sector,count);	 
		 	if(res)//STM32 SPI?bug,?sd??????????????????,????SPI????
			{
				SD_SPI_ReadWriteByte(0xff);
			}
			break;
		case 1:
			res=1;
			break;
		default:
			res=1; 
	}
   //?????,?SPI_SD_driver.c??????ff.c????
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;
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
	u8 res=0;  
    if (!count)return RES_PARERR;//count????0,????????		 	 
	switch(pdrv)
	{
		case 0://SD?
			res=SD_WriteDisk((u8*)buff,sector,count);
			break;
		case 1:
			res=1;
			break;
		default:
			res=1; 
	}
    //?????,?SPI_SD_driver.c??????ff.c????
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
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
	return RES_OK;
}

DWORD get_fattime (void)
{				 
	return 0;
}	


