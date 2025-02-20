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
 * @file iface_sys.c
 * @brief * Rimer SBC system interface
 */
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "rshell.h"
#include "keyboard.h"
#include "tstring.h"

static char* sys_reboot(_cl_param_t *sParam);
static char* sys_poweroff(_cl_param_t *sParam);
static bool iface_sys_init(bool verbose);

_iface_t ifaceSystem =
    {
        .name = "sys",
        .prompt = NULL,
        .desc = "System hardware control",
        .init = iface_sys_init,
        .cmdList =
            {
                {.name = "reboot", .desc = "System reboot", .func = sys_reboot},
                {.name = "off", .desc = "System power off", .func = sys_poweroff},
                {.name = NULL, .func = NULL},
            }
    };

static bool iface_sys_init(bool verbose)
{
   static bool initialized = false;
   if (initialized)
      return true;
   initialized = true;
   if (verbose)
      tprintf("Interface \"sys\" initialized.\n");
   return true;
}
    
char* sys_reboot(_cl_param_t *sParam)
{
   for (uint8_t i = 0; i < 4; i++)
   {
      tprintf(".");
      vTaskDelay(125);
   }
   NVIC_SystemReset();
   return CMD_NO_ERR;
}

char* sys_poweroff(_cl_param_t *sParam)
{
   PM->CTRLA.bit.IORET = 1;
   PM->SLEEPCFG.bit.SLEEPMODE = PM_SLEEPCFG_SLEEPMODE_OFF_Val;
   vTaskDelay(10);
   TIMER_LCD_BL_DECONFIG();
   PWR_PORT.OUTSET.reg = PWR_PIN_VDDn; // Disable VDD
   vTaskDelay(100);
   __asm("wfi");
   return CMD_NO_ERR;
}
