/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include"diskio.h"		/* FatFs lower layer API */
#include"sd.h"	    	/* Header file of existing MMC/SDC contorl module */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/* pdrv--Physical drive nmuber to identify the drive                           */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (	BYTE pdrv	)
{
	switch (pdrv) 
	{
		case 0:				
		case 1: 			
		case 2:				
		case 3:				return RES_OK;
		default:			return STA_NOINIT;
	}
}



/*-----------------------------------------------------------------------
@name:			 Inidialize a Drive                                                    
@param:			 pdrv--Physical drive nmuber to identify the drive 
@return:		 SDTATUS
-----------------------------------------------------------------------*/
DSTATUS disk_initialize (BYTE pdrv)
{
	int Status;
	switch (pdrv) 
	{
		case 0:
					Status = SD_Init();
					if(0 == Status){
						return RES_OK;
					}else{
						return STA_NOINIT;
					}
		case 1:	
		case 2:
		case 3:	return RES_OK;
		default:	return STA_NOINIT;
	}
}



/*-----------------------------------------------------------------------
@name:			 Inidialize a Drive                                                    
@param:			 pdrv--Physical drive nmuber to identify the drive 
						 buff--Data buffer to store read data 
						 secter--Sector address in LBA 
						 count--Number of sectors to read
@return:		 none
-----------------------------------------------------------------------*/
DRESULT disk_read (BYTE pdrv,BYTE *buff,DWORD sector,UINT count)
{
	int Status;
	if(!count)	return RES_PARERR;//count不能为0，否则返回参数错误
	switch (pdrv) 
	{
		case 0:
			   Status = SD_ReadDisk(buff, sector, count);
				 if(1 == Status)	return RES_OK;
				 else	return RES_ERROR;
		default:	return RES_ERROR;
	}
}



/*-----------------------------------------------------------------------
@name:				disk_write;
@param:				pdrv--Physical drive nmuber to identify the drive 
							buff--Data to be written
							sector--Sector address in LBA 
							count--Number of sectors to write
-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write(BYTE pdrv,const BYTE *buff,DWORD sector,UINT count)
{
	int Status;
	if(!count)	return RES_PARERR;
	switch(pdrv) 
	{
		case 0:
				 Status = SD_WriteDisk(buff, sector, count);
				 if(1 == Status)	return RES_OK;
				 else return RES_ERROR;
		default:	return RES_PARERR;
	}
}
#endif


/*-----------------------------------------------------------------------
@description:	Miscellaneous Functions
@name:			disk_ioctl();
@param:			BYTE pdrv--Physical drive nmuber(0..)
						cmd--Control code 
						buff--Buffer to send/receive control data
@return:		DRESULT
-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl(BYTE pdrv,BYTE cmd,void *buff)
{
	switch(pdrv){
		case 0:	 return RES_OK;
		default: return RES_PARERR;
	}
}
#endif


/**
 * @brief    Get current time
 */
DWORD get_fattime(void)
{
     return RES_OK;
} 

