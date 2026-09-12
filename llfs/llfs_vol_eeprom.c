/**-----------------------------------------------------------------------------
 * Copyright (c) 2025 Sergey Sanders
 * sergey@sesadesign.com
 * -----------------------------------------------------------------------------
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
