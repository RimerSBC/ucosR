/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sd_mmc.h"
#include "fat.h"

/* Definitions of physical drive number for each drive */
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 1 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    sd_mmc_err_t result;
    if (pdrv != DEV_MMC) return RES_PARERR;
    while((result = sd_mmc_check(SYS_SD_SLOT)) == SD_MMC_INIT_ONGOING)
        taskYIELD();
    return (result == SD_MMC_OK) ? RES_OK : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    sd_mmc_err_t result;
    if (pdrv != DEV_MMC) return RES_PARERR;
    while((result = sd_mmc_check(SYS_SD_SLOT)) == SD_MMC_INIT_ONGOING)
        taskYIELD();
    return (result == SD_MMC_OK) ? RES_OK : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,		/* Physical drive nmuber to identify the drive */
    BYTE *buff,		/* Data buffer to store read data */
    LBA_t sector,	/* Start sector in LBA */
    UINT count		/* Number of sectors to read */
)
{
    if (pdrv != DEV_MMC) return RES_PARERR;
    if(sd_mmc_get_type(SYS_SD_SLOT) & (CARD_TYPE_SD | CARD_TYPE_MMC))
    {
        sd_mmc_init_read_blocks(SYS_SD_SLOT, sector, count);
        sd_mmc_start_read_blocks(buff, count);
        sd_mmc_wait_end_of_read_blocks(false);
        return RES_OK;
    }
    else return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,			/* Physical drive nmuber to identify the drive */
    const BYTE *buff,	/* Data to be written */
    LBA_t sector,		/* Start sector in LBA */
    UINT count			/* Number of sectors to write */
)
{
    if (pdrv != DEV_MMC) return RES_PARERR;

    if(sd_mmc_get_type(SYS_SD_SLOT) & (CARD_TYPE_SD | CARD_TYPE_MMC))
    {
        if (sd_mmc_init_write_blocks(SYS_SD_SLOT, sector, count) != SD_MMC_OK) 
            return RES_ERROR;
        if (sd_mmc_start_write_blocks(buff, count) != SD_MMC_OK) 
            return RES_ERROR;
        if (sd_mmc_wait_end_of_write_blocks(false) != SD_MMC_OK) 
            return RES_ERROR;
        return RES_OK;
    }
    else return RES_ERROR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
/*
CTRL_SYNC			0	// Complete pending write process (needed at FF_FS_READONLY == 0)
GET_SECTOR_COUNT	1	// Get media size (needed at FF_USE_MKFS == 1)
GET_SECTOR_SIZE		2	// Get sector size (needed at FF_MAX_SS != FF_MIN_SS)
GET_BLOCK_SIZE		3	// Get erase block size (needed at FF_USE_MKFS == 1)
CTRL_TRIM			4	// Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1)
*/
DRESULT disk_ioctl (
    BYTE pdrv,		/* Physical drive nmuber (0..) */
    BYTE cmd,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    if (pdrv != DEV_MMC) return RES_PARERR;
    switch (cmd)
    {
    case CTRL_SYNC:
        while (sd_mmc_busy())
            taskYIELD();
        return RES_OK;
    case GET_SECTOR_COUNT:
        *(LBA_t *)buff = sd_mmc_get_capacity(SYS_SD_SLOT) * 1024 / SD_MMC_SECTOR_SIZE;
        return RES_OK;
    case GET_SECTOR_SIZE:
        *(WORD *)buff = SD_MMC_SECTOR_SIZE;
        return RES_OK;
    case GET_BLOCK_SIZE:
        *(DWORD *)buff = SD_MMC_BLOCK_SIZE;
        return RES_OK;
    case CTRL_TRIM:
        return RES_OK;
    default:
        return RES_PARERR;
    }
}
