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

#ifndef _CMD_FAT_CTRL_INCLUDED
#define _CMD_FAT_CTRL_INCLUDED

#include "sd_mmc.h"

#define SD_PIN_MCDA0    PORT_PA09
#define SD_PIN_MCDA1    PORT_PA10
#define SD_PIN_MCDA2    PORT_PA11
#define SD_PIN_MCDA3    PORT_PB10
#define SD_PIN_MCCK     PORT_PB11
#define SD_PIN_MCMD     PORT_PA08
#define SD_PIN_CD       PORT_PA06
#define SD_PORT_CTRLA   PORT->Group[0] // porta
#define SD_PORT_CTRLB   PORT->Group[1] // portb

#define SYS_SD_SLOT    0

void fat_task(void* vParam );
bool sd_info_print(uint8_t slot);
extern bool FATReady;
extern uint8_t sd_mmc_block[SD_MMC_BLOCK_SIZE];

#endif //_CMD_FAT_CTRL_INCLUDED