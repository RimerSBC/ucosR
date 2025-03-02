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

static cmd_err_t utl_hex(_cl_param_t *sParam);
static cmd_err_t utl_delay(_cl_param_t *sParam);
static bool iface_util_init(bool verbose);

_iface_t ifaceUtil =
    {
        .name = "utl",
        .prompt = NULL,
        .desc = "Comand line utility",
        .init = iface_util_init,
        .cmdList =
            {
                {.name = "hex", .desc = "Print hexadecimal value", .func = utl_hex},
                {.name = "dly", .desc = "Delay execution in mSec", .func = utl_delay},
                {.name = NULL, .func = NULL},
            }};

static bool iface_util_init(bool verbose)
{
   static bool initialized = false;
   if (initialized)
      return true;
   initialized = true;
   if (verbose)
      tprintf("Interface \"utl\" initialized.\n");
   return true;
}

cmd_err_t utl_hex(_cl_param_t *sParam)
{
   for (uint8_t i = 0; i < sParam->argc; i++)
   {
      tprintf("%4x ", (uint16_t)strtol(sParam->argv[i], NULL, 0));
   }
   tprintf("\n");
   return CMD_NO_ERR;
}

cmd_err_t utl_delay(_cl_param_t *sParam)
{
   uint16_t delay;
   if (!sParam->argc)
      return CMD_MISSING_PARAM;
   delay = (uint16_t)strtol(sParam->argv[0], NULL, 0);
   vTaskDelay(delay);
   return CMD_NO_ERR;
}
