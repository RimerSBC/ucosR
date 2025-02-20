/**-----------------------------------------------------------------------------
 * Copyright (c) 2025 Sergey Sanders
 * sergey@sesadesign.com
 * -----------------------------------------------------------------------------
 * Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0
 * International (CC BY-NC-SA 4.0). 
 * 
 * You are free to:
 *  - Share: Copy and redistribute the material.
 *  - Adapt: Remix, transform, and build upon the material.
 * 
 * Under the following terms:
 *  - Attribution: Give appropriate credit and indicate changes.
 *  - NonCommercial: Do not use for commercial purposes.
 *  - ShareAlike: Distribute under the same license.
 * 
 * DISCLAIMER: This work is provided "as is" without any guarantees. The authors
 * aren’t responsible for any issues, damages, or claims that come up from using
 * it. Use at your own risk!
 * 
 * Full license: http://creativecommons.org/licenses/by-nc-sa/4.0/
 * ---------------------------------------------------------------------------*/

#include "FreeRTOS.h"
#include "task.h"
#include "sys_sercom.h"
#include "llfs_vol_eeprom.h"

uint16_t eeprom_write_sector(uint8_t *data,uint16_t sector);
uint16_t eeprom_read_sector(uint8_t *data,uint16_t sector);
bool eeprom_get_sector_next(uint16_t *next,uint16_t sector);
bool eeprom_set_sector_next(uint16_t *next,uint16_t sector);

static const lf_phy_t phyEEPROM =
{
    .fsType 		= 	LLFS_VOLUME_VALID,
    .fsTypeN		= 	(uint8_t)~LLFS_VOLUME_VALID,
    .devID			=   LLFS_DEVID_EEPROM,
    .secCount 		=	LLFS_SECTOR_COUNT_EEPROM,
    .compress		=	LLFS_COMPRESS_NONE,
    .ecc 			= 	LLFS_ECC_NONE,
};

static volume_t eepromIC =
{
    .phy = &phyEEPROM,
    .write_sector = eeprom_write_sector,
    .read_sector = eeprom_read_sector,
    .get_sector_next = eeprom_get_sector_next,
    .set_sector_next = eeprom_set_sector_next,
};

volume_t *volumeEEPROM = &eepromIC;

uint16_t eeprom_write_sector(uint8_t *data,uint16_t sector)
{
    if (data==NULL)
        lf_error = LF_ERR_MEM;
    else
        lf_error = (sys_eeprom_write(LLFS_SECTOR_SIZE*sector,data,LLFS_SECTOR_SIZE)) ? LF_ERR_NONE : LF_ERR_WRITE;
    return lf_error == LF_ERR_NONE ? LLFS_SECTOR_SIZE : 0;
}
uint16_t eeprom_read_sector(uint8_t *data,uint16_t sector)
{
    if (data==NULL)
        lf_error = LF_ERR_MEM;
    else
        lf_error = (sys_eeprom_read(LLFS_SECTOR_SIZE*sector,data,LLFS_SECTOR_SIZE)) ? LF_ERR_NONE : LF_ERR_READ;
    return lf_error == LF_ERR_NONE ? LLFS_SECTOR_SIZE : 0;
}

bool eeprom_set_sector_next(uint16_t *next,uint16_t sector)
{
    lf_error = (sys_eeprom_write(LLFS_SECTOR_SIZE*(sector+1)-2,(uint8_t *)next,2)) ? LF_ERR_NONE : LF_ERR_WRITE;
    return (lf_error == LF_ERR_NONE) ? true : false;
}

bool eeprom_get_sector_next(uint16_t *next,uint16_t sector)
{
    lf_error = (sys_eeprom_read(LLFS_SECTOR_SIZE*(sector+1)-2,(uint8_t *)next,2)) ? LF_ERR_NONE : LF_ERR_READ;
    return (lf_error == LF_ERR_NONE) ? true : false;
}
