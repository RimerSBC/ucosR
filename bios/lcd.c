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
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "bsp.h"
#include "dmactrl.h"
#include "string.h"
#include "lcd.h"

extern int _svideo; // start of the frame buffer memory described in *.ld file
uint8_t *frameBuffer = (uint8_t *)&_svideo;

volatile bool vSync;
static bool vSyncEnable = false;

#define lcd_dc_data() LCD_PORT.OUTSET.reg = LCD_PIN_DC
#define lcd_dc_cmd() LCD_PORT.OUTCLR.reg = LCD_PIN_DC

void lcd_byte_wr(uint8_t data);
void lcd_wr(uint8_t *data,uint32_t size);
void lcd_cmd_wr(uint8_t cmd);
void lcd_data_wr(uint16_t data);

const uint8_t LcdInitILI9341[] =
{
    4, 0xEF, 0x03, 0x80, 0x02,
    4, 0xCF, 0x00, 0xC1, 0X30,
    5, 0xED, 0x64, 0x03, 0X12, 0X81,
    4, 0xE8, 0x85, 0x00, 0x78,
    6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
    2, 0xF7, 0x20,
    3, 0xEA, 0x00, 0x00,
    2, 0xC0, 0x23, // Power control
    2, 0xC1, 0x10, // Power control
    3, 0xC5, 0x3e, 0x28, // VCM control
    2, 0xC7, 0x86, // VCM control2
    2, 0x36, 0xF8, // Memory Access Control - 320V x 240H
    2, 0x3A, 0x55,
    4, 0xB6, 0x08, 0x82, 0x27, // Display Function Control
    2, 0xF2, 0x00, // Gamma Function Disable
    2, 0x26, 0x01, // Gamma curve selected
    16, 0xE0, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
    16, 0xE1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
    2, 0x35,0x00, // Turn the frame mark signal on
    3, 0xB1, 0x00, 0x10, // FrameRate Control 119Hz
    1, 0x11, // exit sleep
    1, 0x29, // turn the display on
    5, 0x2A, 0x00,0x00,0x01,0x3f,//0x13, // Column range set 0:319
    5, 0x2B, 0x00,0x00,0x00,0xef,//0x00, // Row range set 0:239
    0
};

const uint8_t LcdInitST7789[] =
{
    100+1,0x11,// cmd 0x11, delay 120mS
    2,0x36,0x60, /// MADCTL : Memory Data Access Control
    2,0x3A,0x05,
    6,0xB2,0x0C,0x0C,0x00,0x33,0x33,
    2,0xB7,0x35,
    2,0xBB,0x15,
    2,0xC0,0x2C,
    2,0xC2,0x01,
    2,0xC3,0x0B,
    2,0xC4,0x20,
    2,0xC6,0x0F,
    3,0xD0,0xA4,0xA1,
    5,0x2A,0x00,0x00,0x01,0x3f, // Column range set 0:319
    5,0x2B,0x00,0x00,0x00,0xef, // Row range set 0:239    
    1,0x21,    
    15,0xE0,0xF0,0x00,0x04,0x04,0x04,0x05,0x29,0x33,0x3E,0x38,0x12,0x12,0x28,0x30,
    15,0xE1,0xF0,0x07,0x0A,0x0D,0x0B,0x07,0x28,0x33,0x3E,0x36,0x14,0x14,0x29,0x32,
    100+1,0x11, // cmd 0x11, delay 120mS
    1,0x29,
    0
};

const uint8_t *LcdInitCmd = LcdInitST7789;
//const uint8_t *LcdInitCmd = LcdInitILI9341;

TaskHandle_t xLcdTask;

Sercom *lcdPort = LCD_SERCOM;

bool pixelXor = false;

void init_dsc_lcd(void)
{
    /// Init 3 descriptors for full video memory transfer.
    bspDMABase[DMA_LCD_DESC0].BTCNT.reg = 65535;
    bspDMABase[DMA_LCD_DESC0].BTCTRL.reg = DMAC_BTCTRL_BEATSIZE_BYTE | DMAC_BTCTRL_STEPSIZE_X1 | DMAC_BTCTRL_STEPSEL_SRC\
                                           | DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID | DMAC_BTCTRL_BLOCKACT_NOACT;
    bspDMABase[DMA_LCD_DESC0].SRCADDR.reg = (uint32_t)frameBuffer+bspDMABase[DMA_LCD_DESC0].BTCNT.reg;
    bspDMABase[DMA_LCD_DESC0].DSTADDR.reg = (uint32_t)&lcdPort->SPI.DATA.reg;
    bspDMABase[DMA_LCD_DESC0].DESCADDR.reg = (uint32_t)&bspDMABase[DMA_LCD_DESC1];

    bspDMABase[DMA_LCD_DESC1].BTCNT.reg = 11265; //(320x240-65535)
    bspDMABase[DMA_LCD_DESC1].BTCTRL.reg = DMAC_BTCTRL_BEATSIZE_BYTE | DMAC_BTCTRL_STEPSIZE_X1 | DMAC_BTCTRL_STEPSEL_SRC\
                                           | DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID | DMAC_BTCTRL_BLOCKACT_NOACT;
    bspDMABase[DMA_LCD_DESC1].SRCADDR.reg = (uint32_t)frameBuffer+bspDMABase[DMA_LCD_DESC0].BTCNT.reg+bspDMABase[DMA_LCD_DESC1].BTCNT.reg;
    bspDMABase[DMA_LCD_DESC1].DSTADDR.reg = (uint32_t)&lcdPort->SPI.DATA.reg;
    bspDMABase[DMA_LCD_DESC1].DESCADDR.reg = 0;

    dmaLCD->CHPRILVL.reg = 0;
    dmaLCD->CHINTENSET.bit.TCMPL = 1; // enable transfer complete interrupt.
    dmaLCD->CHCTRLA.reg = DMAC_CHCTRLA_TRIGACT_BURST | DMAC_CHCTRLA_TRIGSRC(0x0D); /// Triger is SERCOM4 TX

    NVIC_SetPriority(DMAC_LCD_IRQn,configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);
    NVIC_EnableIRQ(DMAC_LCD_IRQn);
}

void DMAC_LCD_IRQ_Handler(void)
{
    bool xYieldRequired;
    xYieldRequired = xTaskResumeFromISR(xLcdTask);
    dmaLCD->CHINTFLAG.reg = dmaLCD->CHINTFLAG.reg; // clear the interrupt flags
    portYIELD_FROM_ISR(xYieldRequired);
}

TcCount8 *lcdBLTc = (TcCount8 *)LCD_BL_TIMER;

void lcd_init(void)
{
    uint16_t cmdPtr = 0;
    uint8_t cmdBlockSize = LcdInitCmd[cmdPtr++];

    /// PINs configuration

    LCD_PORT.OUTCLR.reg = LCD_PIN_MUX;
    LCD_PORT.OUTCLR.reg = LCD_PIN_DC;

    LCD_PORT.DIRSET.reg = LCD_PIN_DC | LCD_PIN_MUX;

    /// Backlight LED init
    TIMER_LCD_BL_CONFIG();
    lcdBLTc->CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT8_Val;
    lcdBLTc->CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV8_Val;
    lcdBLTc->CTRLA.bit.CAPTEN1 = true;
    lcdBLTc->WAVE.reg = TC_WAVE_WAVEGEN_NPWM;
    lcdBLTc->PER.reg = 100;
    lcdBLTc->CC[1].reg = 80; // Default brightness
    lcdBLTc->CTRLA.bit.ENABLE = true;
    while (lcdBLTc->SYNCBUSY.bit.ENABLE)
        __asm("nop");
    lcdBLTc->CTRLBSET.bit.CMD = TC_CTRLBSET_CMD_RETRIGGER_Val;

    /// DMA init
    init_dsc_lcd();
    /// SPI port init
    SERCOM_LCD_CONFIG();

    lcdPort->SPI.CTRLA.bit.MODE = 0x03; // SPI master operation
    lcdPort->SPI.CTRLA.bit.FORM = 0x00; // SPI frame
    lcdPort->SPI.CTRLA.bit.DORD = 0x00; // MSB first
    lcdPort->SPI.CTRLA.bit.CPOL = 0x00; // Idle high
    lcdPort->SPI.CTRLA.bit.CPHA = 0x00; // Rising edge sample, falling edge change
    lcdPort->SPI.CTRLA.bit.DIPO = 0x03; // SPI DI at PAD[3]
    lcdPort->SPI.CTRLA.bit.DOPO = 0x00; // DO - PAD[0],SCK - PAD[1], *SS - PAD[2]
    lcdPort->SPI.CTRLB.bit.MSSEN = 0x01; // Hardware *SS enable
    lcdPort->SPI.CTRLB.bit.RXEN = 0x00; //RX disable
    lcdPort->SPI.CTRLC.bit.ICSPACE = 0x04; //Inercharacter space: 0x04 (254 ns per pixel) for 50 HZ, 0 for max: 196 ns per pixel - > ~ 66 Hz
    lcdPort->SPI.BAUD.reg = 0x00; // 60MHz clock
    lcdPort->SPI.CTRLA.bit.ENABLE = 0x01; // Enable the port
    while (lcdPort->SPI.SYNCBUSY.bit.ENABLE)
        __asm("nop");

    vTaskDelay(20); // wait for LCD reset

    while (cmdBlockSize)
    {
        lcd_dc_cmd();
        lcd_byte_wr(LcdInitCmd[cmdPtr++]);
        if(cmdBlockSize>100) 
            {
                vTaskDelay(120);
                cmdBlockSize -= 100;
            }
        lcd_dc_data();
        for (uint8_t i=0; i<cmdBlockSize-1; i++) lcd_byte_wr(LcdInitCmd[cmdPtr++]);
        cmdBlockSize = LcdInitCmd[cmdPtr++];
    }
}
volatile uint16_t FrameDelay=0;
void __attribute__((optimize("1"))) lcd_task(void *vParam)
{
    TickType_t xStart, xEnd;
    xStart = xEnd = xTaskGetTickCount();
    lcd_init();
    while(1)
    {
        if (vSyncEnable)
            {
                while (!vSync) taskYIELD();
                vSync = false;
            }
        LCD_PORT.OUTCLR.reg = LCD_PIN_MUX;
        lcd_dc_cmd();
        lcd_byte_wr(0x2C);//RAMWR
        LCD_PORT.OUTSET.reg = LCD_PIN_MUX;
        lcd_dc_data();
        dmaLCD->CHCTRLA.bit.ENABLE = true;
        lcdPort->SPI.DATA.reg = *frameBuffer;
        xEnd = xTaskGetTickCount();
        FrameDelay = xEnd - xStart;
        xStart = xEnd;
        vTaskSuspend(NULL);
    }
}

void lcd_vsync_mode(bool enable)
{
    vSyncEnable = enable;
}

void lcd_byte_wr(uint8_t data)
{
    while (!lcdPort->SPI.INTFLAG.bit.DRE)
        __asm("nop");
    lcdPort->SPI.DATA.reg = _mux(data);
    while (!lcdPort->SPI.INTFLAG.bit.TXC)
        __asm("nop");
}

void lcd_cls(uint8_t colour)
{
    memset(frameBuffer,colour,FB_SIZE);
}

void lcd_set_bl(uint8_t pwr) // in %
{
    if (pwr > 100) pwr = 100;
    lcdBLTc->CCBUF[1].reg = pwr;
}

void (*x_pixel)(uint16_t x,uint8_t y, uint8_t c) = put_pixel;
void put_xpixel(uint16_t x,uint8_t y, uint8_t c) // put xored pixel
{
    if ((x < LCD_WIDTH) && (y < LCD_HEIGHT))
        frameBuffer[x+y*LCD_WIDTH] = frameBuffer[x+y*LCD_WIDTH] ^ c;
}

void put_pixel(uint16_t x,uint8_t y, uint8_t c)
{
    if ((x < LCD_WIDTH) && (y < LCD_HEIGHT))
        frameBuffer[x+y*LCD_WIDTH] = c;
}

uint8_t get_pixel(uint16_t x,uint8_t y)
{
    return frameBuffer[x+y*LCD_WIDTH];
}
