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

#ifndef _DMACTRL_H_INCLUDED
#define _DMACTRL_H_INCLUDED

#include "sam.h"
#include "stdint.h"
#include "stdbool.h"

enum
{
    DMA_LCD_CHAN,
    DMA_CHANNELS
};

enum
{
    DMA_LCD_DESC0,
    DMA_LCD_DESC1,
    DMA_DESCRIPTORS
};

extern DmacDescriptor bspDMABase[DMA_DESCRIPTORS];  // Base descriptors for all channels
extern DmacDescriptor bspDMAWback[DMA_CHANNELS]; // Write back descriptors for all channels

#define DMAC_LCD_IRQn           DMAC_0_IRQn
#define DMAC_LCD_IRQ_Handler    DMAC_0_Handler
extern DmacChannel *dmaLCD;

void dma_init(void);

#endif //_DMACTRL_H_INCLUDED