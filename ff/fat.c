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
 * @file fat_ctrl.c
 * @author Sergey Sanders
 * @date Jan 2021
 * @brief * FAT FS on SD interface
 *
 */
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "tstring.h"
#include "conf_sd_mmc.h"
#include "sd_mmc.h"
#include "hal_mci_sync.h"
#include "fat.h"
#include "ff.h"

bool FATReady = false;

struct mci_sync_desc sdcard_io_bus;
/* Card Detect (CD) pin settings */
static sd_mmc_detect_t SDMMC_ACCESS_0_cd[CONF_SD_MMC_MEM_CNT] =
{
    {CONF_SD_MMC_0_CD_DETECT_PORT, CONF_SD_MMC_0_CD_DETECT_VALUE},
};

uint8_t sd_mmc_block[SD_MMC_BLOCK_SIZE];

void sd_port_init(void)
{
    // set clocks
    REG_GCLK_PCHCTRL45 = CLK_60MHZ | GCLK_PCHCTRL_CHEN;/* GCLK_SDHC0 core clock @ 60MHz  */\
    REG_MCLK_AHBMASK |= MCLK_AHBMASK_SDHC0;
    /// set in bsp.h REG_GCLK_PCHCTRL3 = GCLK_PCHCTRL_GEN_GCLK2 | GCLK_PCHCTRL_CHEN; // All SERCOMs slow clock @ 12MHz
    // set pins
    SD_PORT_CTRLA.WRCONFIG.reg = CONF_PIN_VAL(SD_PIN_CD) | PORT_WRCONFIG_PULLEN | PORT_WRCONFIG_INEN | PORT_WRCONFIG_WRPINCFG;
    SD_PORT_CTRLA.WRCONFIG.reg = CONF_PIN_VAL(SD_PIN_MCDA0 | SD_PIN_MCDA1 | SD_PIN_MCDA2 | SD_PIN_MCMD) | PORT_WRCONFIG_PMUX(0x08) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPINCFG;
    SD_PORT_CTRLB.WRCONFIG.reg = CONF_PIN_VAL(SD_PIN_MCCK | SD_PIN_MCDA3) | PORT_WRCONFIG_PMUX(0x08) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPINCFG;
   // sd_mmc_init(&sdcard_io_bus, SDMMC_ACCESS_0_cd, SDMMC_ACCESS_0_wp);
   // mci_sync_init(&sdcard_io_bus, SDHC0);
}
bool sd_info_print(uint8_t slot)
{
    sd_mmc_err_t result;
    while((result = sd_mmc_check(SYS_SD_SLOT)) == SD_MMC_INIT_ONGOING)
        taskYIELD();
    if (result != SD_MMC_OK)
    {
        tprintf("Card read error: %d\n",result);
        return false;
    }
    tprintf("Card information:\n");
    switch (sd_mmc_get_type(slot))
    {
    case CARD_TYPE_SD | CARD_TYPE_HC:
        tprintf("SDHC");
        break;
    case CARD_TYPE_SD:
        tprintf("SD");
        break;
    case CARD_TYPE_MMC | CARD_TYPE_HC:
        tprintf("MMC High Density");
        break;
    case CARD_TYPE_MMC:
        tprintf("MMC");
        break;
    case CARD_TYPE_SDIO:
        tprintf("SDIO");
        return true;
    case CARD_TYPE_SD_COMBO:
        tprintf("SD COMBO");
        break;
    case CARD_TYPE_UNKNOWN:
    default:
        tprintf("Unknow card!\n");
        return false;
    }
    tprintf("\n    %d MB\n", (uint16_t)(sd_mmc_get_capacity(slot) / 1024));
    return true;
}

void fat_task(void* vParam )
{
    bool sdCsPin,lastSdCsPin;
    sd_mmc_err_t result;
    FATFS *fs;     /* Ponter to the filesystem object */
    fs = pvPortMalloc(sizeof (FATFS));           /* Get work area for the volume */
    sd_port_init();
    sdCsPin = (SD_PORT_CTRLA.IN.reg & SD_PIN_CD) ? true : false;
    lastSdCsPin = !sdCsPin; // force first card status check
    while(1)
    {
        sdCsPin = (SD_PORT_CTRLA.IN.reg & SD_PIN_CD) ? true : false;
        if (lastSdCsPin != sdCsPin)
        {
            lastSdCsPin = sdCsPin;
            sd_mmc_init(&sdcard_io_bus, SDMMC_ACCESS_0_cd, NULL);
            mci_sync_init(&sdcard_io_bus, SDHC0);
            if(sdCsPin == CONF_SD_MMC_0_CD_DETECT_VALUE)
            {
                //if (sd_info_print(SYS_SD_SLOT))
                while((result = sd_mmc_check(SYS_SD_SLOT)) == SD_MMC_INIT_ONGOING)
                    taskYIELD();
                if (result == SD_MMC_OK) 
                    FATReady = (f_mount(fs, "", 1) == FR_OK) ? true : false;
            }
            else
            {
                f_unmount("");
            }
        }
        vTaskDelay(100);
    }
}
