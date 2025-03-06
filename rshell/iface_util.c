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

enum
{
   AUTO,
   BYTE,
   WORD,
   DWORD,
   FLOAT
} numType = AUTO;

static cmd_err_t utl_hex(_cl_param_t *sParam);
static cmd_err_t utl_delay(_cl_param_t *sParam);
static cmd_err_t utl_dec(_cl_param_t *sParam);
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
                {.name = "dec", .desc = "Make a number from a byte list", .func = utl_dec},
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
   numType = AUTO;
   uint8_t paramType;
   uint8_t paramPtr = 0;
   uint32_t result;
   if (!sParam->argc)
      return CMD_MISSING_PARAM;
   if (*sParam->argv[0] == '-' && (sParam->argv[0][1] > 'a')) // get params
   {
      if (sParam->argc < 2)
         return CMD_MISSING_PARAM;
      paramPtr++;
      sParam->argc--;
      while (*++sParam->argv[0])
         switch (*sParam->argv[0])
         {
         case 'b':
            numType = BYTE;
            break;
         case 'w':
            numType = WORD;
            break;
         case 'd':
            numType = DWORD;
            break;
         }
   }

   for (uint8_t i = 0; i < sParam->argc; i++)
   {
      result = (uint32_t)strtoul(sParam->argv[paramPtr], NULL, 0);
      if (numType == AUTO)
         paramType = result < 256 ? BYTE : result > 65535 ? DWORD :
                                                            WORD;
      else
         paramType = numType;
      switch (paramType)
      {
      case BYTE:
         tprintf("%2X ", (uint8_t)result);
         break;
      case WORD:
         tprintf("%4X ", (uint16_t)result);
         break;
      default:
         tprintf("%8X ", (uint32_t)result);
         break;
      }
   }
   tprintf("\n");
   return CMD_NO_ERR;
}

cmd_err_t utl_dec(_cl_param_t *sParam)
{
   numType = AUTO;
   uint8_t paramPtr = 0;
   bool lsbFirst = true;
   bool isSigned = false;
   if (!sParam->argc)
      return CMD_MISSING_PARAM;
   if (*sParam->argv[0] == '-') // get params
   {
      if (sParam->argc < 2)
         return CMD_MISSING_PARAM;
      paramPtr++;
      sParam->argc--;
      while (*++sParam->argv[0])
         switch (*sParam->argv[0])
         {
         case 'b':
            numType = BYTE;
            break;
         case 'w':
            numType = WORD;
            break;
         case 'd':
            numType = DWORD;
            break;
         case 'f':
            numType = FLOAT;
            break;
         case 'm':
            lsbFirst = false;
            break;
         case 's':
            isSigned = true;
            break;
         }
   }
   if (numType == AUTO)
      numType = sParam->argc == 1 ? BYTE :
                sParam->argc == 2 ? WORD :
                                    DWORD;
   if (sParam->argc > 4)
      sParam->argc = 4;
   switch (numType)
   {
   case BYTE:
   {
      uint8_t result = (uint8_t)strtol(sParam->argv[paramPtr], NULL, 0);
      tprintf("%d", isSigned ? (int8_t)result : (uint8_t)result);
   }
   break;
   case WORD:
   {
      uint16_t result;
      if (sParam->argc == 1)
         result = (uint16_t)strtol(sParam->argv[paramPtr], NULL, 0);
      else
      {
         result = (uint8_t)strtol(sParam->argv[paramPtr++], NULL, 0);
         result += (uint16_t)strtol(sParam->argv[paramPtr], NULL, 0) << 8;
         if (!lsbFirst)
            result = (result << 8) + (result >> 8);
      }
      tprintf("%d", isSigned ? (int16_t)result : (uint16_t)result);
   }
   break;
   case DWORD:
   case FLOAT:
   {
      uint32_t result = 0;
      if (sParam->argc == 1)
         result = (uint32_t)strtoul(sParam->argv[paramPtr], NULL, 0);
      else
      {
         for (uint8_t i = 0; i < sParam->argc; i++)
            result = (result << 8) + (uint8_t)strtol(sParam->argv[paramPtr++], NULL, 0);
         if (!lsbFirst)
            result = ((result & 0x000000FF) << 24) | ((result & 0x0000FF00) << 8) |
                     ((result & 0x00FF0000) >> 8) | ((result & 0xFF000000) >> 24);
      }
      if (numType == DWORD)
         tprintf("%d", isSigned ? (int32_t)result : (uint32_t)result);
      else
         tprintf("%f", *(float *)&result);
   }
   break;
   default:
      break;
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
