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
 * @file main.c
 * @author Sergey Sanders
 * @date 24 March 2024
 * @brief ucosR
 * ATSAME(D)5 chip
 * @see ATSAMD51 Datasheet
 */
#include <string.h>
#include "freeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "sys_sercom.h"
#include "lcd.h"
#include "keyboard.h"
#include "tstring.h"
#include "rshell.h"
#include "uterm.h"
#include "llfs.h"
#include "fat.h"
#include "sys_config.h"

void ucosR_task(void *vParam)
{
   xTaskCreate(lcd_task, "LCD", configMINIMAL_STACK_SIZE / 2, NULL, 5, &xLcdTask);
   xTaskCreate(keyboard_task, "KBD", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
   xTaskCreate(sys_sercom3_task, "SCOM3", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
   vTaskDelay(50);
   conf_load();
   lcd_set_bl(sysConf.bright);
   xTaskCreate(fat_task, "fat", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
   xTaskCreate(uterm_task, "terminal", configMINIMAL_STACK_SIZE, NULL, 2, &xuTermTask);
   xTaskCreate(rshell_task, "rshell", configMINIMAL_STACK_SIZE * 8, NULL, 2, NULL);
   vTaskSuspend(NULL);
   while (1) // should never get here.
      asm("nop");
}
