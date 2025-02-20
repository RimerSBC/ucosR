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

#include "dmactrl.h"

/** DMA init */

DmacDescriptor __attribute__((aligned(16))) bspDMABase[DMA_DESCRIPTORS];  // Base descriptors for all channels
DmacDescriptor __attribute__((aligned(16))) bspDMAWback[DMA_CHANNELS]; // Write back descriptors for all channels

DmacChannel *dmaLCD = &DMAC->Channel[DMA_LCD_CHAN];

void dma_init(void)
{
    /// Configure the DMA module
    REG_DMAC_DBGCTRL |= DMAC_DBGCTRL_DBGRUN;
    REG_DMAC_BASEADDR = (uint32_t) bspDMABase;
    REG_DMAC_WRBADDR = (uint32_t) bspDMAWback;
    //REG_DMAC_PRICTRL0 = DMAC_PRICTRL0_RRLVLEN0 | DMAC_PRICTRL0_RRLVLEN1; // enable round robin on priority 0
    REG_DMAC_CTRL |= DMAC_CTRL_LVLEN0 | DMAC_CTRL_DMAENABLE;
}

