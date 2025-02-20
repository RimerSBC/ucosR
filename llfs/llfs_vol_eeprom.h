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
#ifndef _LLFS_VOL_EEPROM_H_INCLUDED
#define _LLFS_VOL_EEPROM_H_INCLUDED
#include "stdint.h"
#include "llfs.h"

#define LLFS_EMPTY_BYTE             0xff
#define LLFS_DEVID_EEPROM           0x0400
#define LLFS_EEPROM_SIZE            (64 * 1024)
#define LLFS_SECTOR_COUNT_EEPROM    256
extern volume_t *volumeEEPROM;


#endif // _LLFS_VOL_EEPROM_H_INCLUDED