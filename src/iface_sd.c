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
 * @file rshell.c
 * @author Sergey Sanders
 * @date September 2022
 * @brief * rshell interface "Setup"
 *   system control interface
 *
 */
#include "FreeRTOS.h"
#include "bsp.h"
#include "fat.h"
#include "ff.h"
#include "keyboard.h"
#include "rshell.h"
#include "sd_mmc.h"
#include "task.h"
#include "tstring.h"
#include "uterm.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "board_api.h"
#include "tusb.h"

typedef enum
{
   TUSB_NOT_MOUNTED,
   TUSB_MOUNTED,
   TUSB_SUSPENDED,
} tusb_stat_e;

tusb_stat_e tusbStat;

static void usb_device_task(void *param);
void cdc_task(void *params);

static bool iface_sd_init(bool verbose);
static cmd_err_t cmd_fat_sdi(_cl_param_t *sParam);
static cmd_err_t cmd_fat_block_read(_cl_param_t *sParam);
static cmd_err_t cmd_fat_ls(_cl_param_t *sParam);
static cmd_err_t cmd_fat_cd(_cl_param_t *sParam);
static cmd_err_t cmd_fat_pwd(_cl_param_t *sParam);
static cmd_err_t cmd_fat_rm(_cl_param_t *sParam);
static cmd_err_t cmd_fat_cat(_cl_param_t *sParam);

const _iface_t ifaceSD = {
    .name = "sd",
    .prompt = NULL,
    .desc = "sd card FAT control",
    .init = iface_sd_init,
    .cmdList = {
        {.name = "sdi", .desc = "sd card info", .func = cmd_fat_sdi},
        {.name = "sdrd", .desc = "sd card read : block count", .func = cmd_fat_block_read},
        {.name = "ls", .desc = "directory list", .func = cmd_fat_ls},
        {.name = "cd", .desc = "change directory", .func = cmd_fat_cd},
        {.name = "pwd", .desc = "get working directory", .func = cmd_fat_pwd},
        {.name = "rm", .desc = "remove file(s)", .func = cmd_fat_rm},
        {.name = "cat", .desc = "displays file contents", .func = cmd_fat_cat},
        {.name = NULL, .func = NULL},
    }};

static bool iface_sd_init(bool verbose)
{
   static bool initialized = false;
   if (initialized)
      return true;

   for (uint8_t i = 0; i < 20; i++)
   {
      if (FATReady)
         break;
      vTaskDelay(10); // Wait for FAT is ready with 200 ms timeout.
   }
   initialized = FATReady;
   
  void tusb_board_init(void);
  tusb_board_init(); 
  xTaskCreate(usb_device_task, "usbd", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
  xTaskCreate(cdc_task, "cdc", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 2, NULL);
   
   if (verbose)
      tprintf("Interface \"sd\" initialized.\n");
   return initialized;
}

char *cmd_fat_sdi(_cl_param_t *sParam)
{
   uint8_t slot = 0;
   if (sParam->argc)
   {
      slot = (uint8_t)strtol(sParam->argv[0], NULL, 0);
      if (slot > 1)
         slot = 1;
   }
   sd_info_print(slot);
   return CMD_NO_ERR;
}

char *cmd_fat_block_read(_cl_param_t *sParam)
{
   uint32_t block = 0;
   uint16_t count = 1;
   uint8_t line = 0;
   if (sParam->argc)
   {
      block = (uint32_t)strtol(sParam->argv[0], NULL, 0);
      if (sParam->argc > 1)
         count = (uint16_t)strtol(sParam->argv[1], NULL, 0);
   }
   while (sd_mmc_check(0) != SD_MMC_OK)
      taskYIELD(); // Wait for card is ready.

   if (sd_mmc_get_type(0) & (CARD_TYPE_SD | CARD_TYPE_MMC))
      for (uint16_t i = block; i < block + count; i++)
      {
         sd_mmc_init_read_blocks(0, i, 1);
         sd_mmc_start_read_blocks(sd_mmc_block, 1);
         sd_mmc_wait_end_of_read_blocks(false);
         for (uint16_t j = 0; j < sizeof(sd_mmc_block); j += 8)
            dump8(j, &sd_mmc_block[j], 8, &line);
      }
   else
      return "No SD card!";
   return CMD_NO_ERR;
}

char *cmd_fat_ls(_cl_param_t *sParam)
{
   FRESULT fr;  /* Return value */
   DIR dj;      /* Directory object */
   FILINFO fno; /* File information */
   uint8_t lineCnt = 0;
   /// Start to search for files
   if ((fr = f_findfirst(&dj, &fno, "", sParam->argc ? sParam->argv[0] : "*")) != FR_OK)
      return "can't read volume!";
   while (fr == FR_OK && fno.fname[0]) /* Repeat while an item is found */
   {
      if (lineCnt > uTerm.lines - 2)
      {
         tprintf("\e[a\e[33mscroll(y/n)?\e[r");
         if (keyboard_wait("yY"))
            lineCnt = 0;
         tprintf("\e[1K\r");
         if (lineCnt)
            break;
      }
      if (fno.fattrib & AM_DIR)
         tprintf("[ %s ]\n", fno.fname); /* Print directory name */
      else
         tprintf("%d %s\n", fno.fsize, fno.fname); /* Print the object name */
      fr = f_findnext(&dj, &fno);                  /* Search for next item */
      lineCnt++;
   }
   f_closedir(&dj);
   return CMD_NO_ERR;
}

char *cmd_fat_cd(_cl_param_t *sParam)
{
   FRESULT fr; /* Return value */
   if (!sParam->argc)
      return "Missing directory name!";
   if ((fr = f_chdir(sParam->argv[0])) != FR_OK)
      return "Change directory failed!";
   return CMD_NO_ERR;
}

char *cmd_fat_pwd(_cl_param_t *sParam)
{
   FRESULT fr; /* Return value */
   char str[64];
   if (sParam->argc)
      return CMD_MISSING_PARAM;
   if ((fr = f_getcwd(str, sizeof(str))) != FR_OK) /* Get current directory */
   {
      return "Failed to read directory!";
   }
   tprintf("%s\n", str);
   return CMD_NO_ERR;
}

char *cmd_fat_rm(_cl_param_t *sParam)
{
   FRESULT fr;  // Return value
   DIR dj;      // Directory object
   FILINFO fno; // File information
   if (!sParam->argc)
      return CMD_MISSING_PARAM;
   // Start to search for files
   if ((fr = f_findfirst(&dj, &fno, "", sParam->argv[0])) != FR_OK)
      return "Filed to read directory!";
   if (strchr(sParam->argv[0], '*'))      // massive erase
      while (fr == FR_OK && fno.fname[0]) // Repeat while an item is found
      {
         f_unlink(fno.fname);        // delete file
         fr = f_findnext(&dj, &fno); // Search for next item
      }
   else if (!strcmp(fno.fname, sParam->argv[0]))
   {
      tprintf("\e[a\e[33mdelete %s(y/n)?\e[r", sParam->argv[0]);
      if (keyboard_wait("Yy"))
         f_unlink(sParam->argv[0]);
   }
   stdio->putch('\n');
   f_closedir(&dj);
   return CMD_NO_ERR;
}

static cmd_err_t cmd_fat_cat(_cl_param_t *sParam)
{
   FIL catFile;
   UINT br;
   char ch[16];
   bool verbose = false;
   uint8_t n = 0, lCount = 0;
   char *fileName;
   if (!sParam->argc)
      return CMD_MISSING_PARAM;
   if (*sParam->argv[0] == '-') // the first param is an option list
   {
      while (*++sParam->argv[0] && !n)
         switch (*sParam->argv[0])
         {
         case 'v':
            verbose = true;
            break;
         case 'n':
            n = (uint8_t)strtol(++sParam->argv[0], NULL, 0);
            break;
         }
      if (sParam->argc < 2)
         return CMD_MISSING_PARAM;
      fileName = sParam->argv[1];
   }
   else
      fileName = sParam->argv[0];
   if (f_open(&catFile, fileName, FA_READ) != FR_OK)
      return "Unable to open file!";

   while (f_read(&catFile, ch, sizeof(ch), &br) == FR_OK)
   {
      for (uint8_t i = 0; i < br; i++)
         switch (ch[i])
         {
         case '\n':
            lCount++;
            if (lCount == n)
            {
               stdio->putch(ch[i]);
               return CMD_NO_ERR;
            }
            if (lCount > uTerm.lines - 3)
            {
               tprintf("\n\e[a\e[33mscroll(y/n)?\e[r");
               if (!keyboard_wait("yY"))
                  i = br;
               tprintf("\e[1K\r");
               lCount = 0;
               if (i == br)
               {
                  f_close(&catFile);
                  return CMD_NO_ERR;
               }
            }
         case ' ' ... '~':
            stdio->putch(ch[i]);
            break;
         default:
            if (verbose)
               tprintf("#%d ", ch[i]);
         }
      if (br < sizeof(ch))
         break;
   }
   f_close(&catFile);
   return CMD_NO_ERR;
}

/// Tiny USB section

// USB Device Driver task
// This top level thread process all usb events and invoke callbacks
static void usb_device_task(void *param)
{
   (void)param;

   // init device stack on configured roothub port
   // This should be called after scheduler/kernel is started.
   // Otherwise it could cause kernel issue since USB IRQ handler does use RTOS queue API.
   tusb_rhport_init_t dev_init = {
       .role = TUSB_ROLE_DEVICE,
       .speed = TUSB_SPEED_AUTO};
   tusb_init(BOARD_TUD_RHPORT, &dev_init);
   
   if (board_init_after_tusb)
   {
      board_init_after_tusb();
   }

   // RTOS forever loop
   while (1)
   {
      // put this thread to waiting state until there is new events
      tud_task();

      // following code only run if tud_task() process at least 1 event
      tud_cdc_write_flush();
      
      taskYIELD();
   }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
   tusbStat = TUSB_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
   tusbStat = TUSB_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
   (void)remote_wakeup_en;
   tusbStat = TUSB_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
   tusbStat = tud_mounted() ? TUSB_MOUNTED : TUSB_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void cdc_task(void *params)
{
   while (1)
   {
      // connected() check for DTR bit
      // Most but not all terminal client set this when making connection
      // if ( tud_cdc_connected() )
      {
         // There are data available
         while (tud_cdc_available())
         {
            uint8_t buf[64];

            // read and echo back
            uint32_t count = tud_cdc_read(buf, sizeof(buf));
            (void)count;

            // Echo back
            // Note: Skip echo by commenting out write() and write_flush()
            // for throughput test e.g
            //    $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
            tud_cdc_write(buf, count);
         }

         tud_cdc_write_flush();
      }
      vTaskDelay(1);
   }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
   (void)itf;
   (void)rts;

   // TODO set some indicator
   if (dtr)
   {
      // Terminal connected
   }
   else
   {
      // Terminal disconnected
   }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
   (void)itf;
}