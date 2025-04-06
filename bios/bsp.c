/**-----------------------------------------------------------------------------
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
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 Sergey Sanders
 * sergey@sesadesign.com
 * ---------------------------------------------------------------------------*/

#include "bsp.h"

#include "colours.h"
#include "dmactrl.h"

sw_cal_area_t *calData = (sw_cal_area_t *)0x00800080;

bool usbConnected = false;
volatile uint32_t regVal[4];

_sysconf_t sysConf = {
    .font = DEF_CONF_FONT,
    .textFG = DEF_CONF_TEXT_FG,
    .textBG = DEF_CONF_TEXT_BG,
    .bright = DEF_CONF_LCD_BRIGHT,
    .volume = DEF_CONF_SPKR_VOL,
    .kbdsnd = DEF_CONF_SPKR_SND,
    .iface = DEF_CONF_INTERFACE,
    .startup = "",
    .checkSum = 0};

void clocks_init(void)
{
   /// *********** Clocks
   /// Configure GEN0 and GCLK_MAIN as 120MHz
   /// Enable external oscillator (1) and tune the PLL
   REG_OSCCTRL_XOSCCTRL1 = OSCCTRL_XOSCCTRL_IMULT(4) | OSCCTRL_XOSCCTRL_IPTAT(3) | OSCCTRL_XOSCCTRL_ENALC |
                           OSCCTRL_XOSCCTRL_XTALEN | OSCCTRL_XOSCCTRL_ENABLE; // enable external oscillator 12MHz
   while (!(REG_OSCCTRL_STATUS & OSCCTRL_STATUS_XOSCRDY1))
      __asm("nop");

   REG_OSCCTRL_DPLLCTRLB0 =
       OSCCTRL_DPLLCTRLB_DIV(2) | OSCCTRL_DPLLCTRLB_REFCLK_XOSC1;                     // XOSC1, divided by 6 -> Fref = 2MHz
   REG_OSCCTRL_DPLLRATIO0 = OSCCTRL_DPLLRATIO_LDR(60) | OSCCTRL_DPLLRATIO_LDRFRAC(0); // 2MHz * (60) = 120MHz
   while (REG_OSCCTRL_DPLLSYNCBUSY0 & OSCCTRL_DPLLSYNCBUSY_DPLLRATIO)
      __asm("nop");
   REG_OSCCTRL_DPLLCTRLA0 = OSCCTRL_DPLLCTRLA_ENABLE;
   while (REG_OSCCTRL_DPLLSYNCBUSY0 & OSCCTRL_DPLLSYNCBUSY_ENABLE)
      __asm("nop");
   while (!(REG_OSCCTRL_DPLLSTATUS0 & OSCCTRL_DPLLSTATUS_CLKRDY))
      __asm("nop");

   /// Set GCLK0 and CLK_MAIN @ 120 MHz
   REG_GCLK_GENCTRL0 = GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_SRC_DPLL0 | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
   while (REG_GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL0)
      __asm("nop");

   /// Set GCLK1 and CLK_MAIN @ 60 MHz for SERCOM_CORE
   REG_GCLK_GENCTRL1 = GCLK_GENCTRL_DIV(2) | GCLK_GENCTRL_SRC_DPLL0 | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
   while (REG_GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL1)
      __asm("nop");

   /// Set GCLK2 and CLK_MAIN @ 12 MHz for SERCOM_SLOW
   REG_GCLK_GENCTRL2 = GCLK_GENCTRL_DIV(10) | GCLK_GENCTRL_SRC_DPLL0 | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
   while (REG_GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL2)
      __asm("nop");
}

uint8_t crc8(uint8_t *data, uint16_t len)
{
   uint8_t crc = 0;
   while (len--)
   {
      crc ^= *data++;
      for (uint8_t i = 0; i < 8; i++)
         crc = crc & 1 ? (crc >> 1) ^ 0x8C : crc >> 1;
   }
   return crc;
}

void power_init(void)
{
   PWR_PORT.DIRSET.reg = PWR_PIN_VDDn; // PWR_PIN_VIO | PWR_PIN_VDDn | PWR_PIN_VSEL;
   PWR_PORT.OUTCLR.reg = PWR_PIN_VDDn; // Enable VDD
}

void bsp_init(void)
{
   clocks_init();
   SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); // FPU access mode: set CP10 and CP11 Full Access
   sysConf.checkSum = crc8((uint8_t *)&sysConf,sizeof(sysConf)-1);
   dma_init();
   rnd_init();
   power_init();
   SERCOM_CLOCKS_CONFIG();
}
