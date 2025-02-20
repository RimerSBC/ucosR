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
/**
* @file config.c
* @author Sergey Sanders
* @date September 2022
* @brief * ucosR configuration support
*   system settings interface
*
*/
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "tstring.h"
#include "llfs.h"
#include "llfs_vol_eeprom.h"

void conf_load(void)
{
    _sysconf_t tmpConf;
    lfile_t *confFile;
    if ((confFile=lf_open(DEF_CONF_FILE_NAME,MODE_READ)) != NULL)
    {
        lf_read(confFile,&tmpConf,sizeof(tmpConf));
        if (conf_checksum(&tmpConf) != tmpConf.checkSum)
            lf_write(confFile,&sysConf,sizeof(tmpConf));
        else
            memcpy((uint8_t *)&sysConf,(uint8_t *)&tmpConf,sizeof(_sysconf_t));
        lf_close(confFile);
    }
    else
    {
        if ((confFile = lf_open(DEF_CONF_FILE_NAME,MODE_CREATE | MODE_WRITE)) == NULL)
        {
            lf_format(LLFS_EEPROM_SIZE,LLFS_DEVID_EEPROM,"EEPROM");
            confFile = lf_open(DEF_CONF_FILE_NAME,MODE_CREATE | MODE_WRITE);
        }
        lf_write(confFile,&sysConf,sizeof(tmpConf));
        lf_close(confFile);
    }
}

void conf_save(void)
{
    lfile_t *confFile;
    sysConf.checkSum = conf_checksum(&sysConf);
    if ((confFile = lf_open(DEF_CONF_FILE_NAME,MODE_WRITE)))
    {
        if (lf_write(confFile,(uint8_t *)&sysConf,sizeof(sysConf)))
            {
                lf_close(confFile);
                return;
            }
          lf_close(confFile);  
    }
    tprintf("Failed to save config file\n");
}
