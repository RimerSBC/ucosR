/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

/* metadata:
   manufacturer: Microchip
*/

#include "sam.h"
#include "bsp.h"
#include "board_api.h"


//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+

/* Referenced GCLKs, should be initialized firstly */
#define _GCLK_INIT_1ST 0xFFFFFFFF

/* Not referenced GCLKs, initialized last */
#define _GCLK_INIT_LAST (~_GCLK_INIT_1ST)

//--------------------------------------------------------------------+
// Forward USB interrupt events to TinyUSB IRQ Handler
//--------------------------------------------------------------------+
void USB_0_Handler(void)
{
   tud_int_handler(0);
}

void USB_1_Handler(void)
{
   tud_int_handler(0);
}

void USB_2_Handler(void)
{
   tud_int_handler(0);
}

void USB_3_Handler(void)
{
   tud_int_handler(0);
}

//--------------------------------------------------------------------+
// Implementation
//--------------------------------------------------------------------+

void tusb_board_init(void)
{
   // Clock init ( follow hpl_init.c )
/* USB Clock init
    * The USB module requires a GCLK_USB of 48 MHz ~ 0.25% clock
    * for low speed and full speed operation. */

   REG_GCLK_PCHCTRL10 = 3 | GCLK_PCHCTRL_CHEN; /* USB clock set to 48MHz  */
   REG_MCLK_APBBMASK |= MCLK_APBBMASK_USB;
   REG_MCLK_AHBMASK |= MCLK_AHBMASK_USB;

   // USB Pin Init

   USB_PORT.WRCONFIG.reg = CONF_PIN_VAL(USB_PIN_DM | USB_PIN_DP) | PORT_WRCONFIG_PMUX(0x07) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPINCFG;

   // Configure the DFLL for USB clock recovery.
   OSCCTRL->DFLLCTRLA.reg = 0;

   OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_CSTEP(0x1) | OSCCTRL_DFLLMUL_FSTEP(0x1) | OSCCTRL_DFLLMUL_MUL(0xBB80);

   while (OSCCTRL->DFLLSYNC.bit.DFLLMUL)
      asm("nop");

   OSCCTRL->DFLLCTRLB.reg = 0;
   while (OSCCTRL->DFLLSYNC.bit.DFLLCTRLB)
      asm("nop");

   OSCCTRL->DFLLCTRLA.bit.ENABLE = true;
   while (OSCCTRL->DFLLSYNC.bit.ENABLE)
      asm("nop");

   OSCCTRL->DFLLVAL.reg = OSCCTRL->DFLLVAL.reg;
   while (OSCCTRL->DFLLSYNC.bit.DFLLVAL)
      asm("nop");

   OSCCTRL->DFLLCTRLB.reg = OSCCTRL_DFLLCTRLB_WAITLOCK |
                            OSCCTRL_DFLLCTRLB_CCDIS | OSCCTRL_DFLLCTRLB_USBCRM;

//   while (!OSCCTRL->STATUS.bit.DFLLRDY)
//      asm("nop");

   REG_GCLK_GENCTRL3 = GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
   while (REG_GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL3)
      asm("nop");

#if CFG_TUSB_OS == OPT_OS_FREERTOS
   // If freeRTOS is used, IRQ priority is limit by max syscall ( smaller is higher )
   NVIC_SetPriority(USB_0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
   NVIC_SetPriority(USB_1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
   NVIC_SetPriority(USB_2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
   NVIC_SetPriority(USB_3_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
#endif

}

void board_init_after_tusb(void)
{
}

//--------------------------------------------------------------------+
// Board porting API
//--------------------------------------------------------------------+

size_t board_get_unique_id(uint8_t id[], size_t max_len)
{
   (void)max_len;

   uint32_t did_addr[4] = {0x008061FC, 0x00806010, 0x00806014, 0x00806018};

   for (int i = 0; i < 4; i++)
   {
      uint32_t did = *((uint32_t const *)did_addr[i]);
      did = TU_BSWAP32(did); // swap endian to match samd51 uf2 bootloader
      memcpy(id + i * 4, &did, 4);
   }

   return 16;
}
