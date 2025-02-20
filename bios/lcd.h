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
#ifndef _LCD_INCLUDED
#define _LCD_INCLUDED
#include "bsp.h"

#define LCD_WIDTH   320
#define LCD_HEIGHT  240
#define FB_SIZE     (LCD_WIDTH * LCD_HEIGHT)

#define _rgb(_r,_g,_b) ((_r & 0xe0) | ((_g & 0xe0)>>3) | ((_b & 0xc0)>>6))
#define _mux(x) (((x << 5) | (x >> 3)) & 0xff)

#define RGB_COLOR_BLACK       _rgb(0,0,0)      /*   0,   0,   0 */
#define RGB_COLOR_NAVY        _rgb(0,0,128)      /*   0,   0, 128 */
#define RGB_COLOR_DARKGREEN   _rgb(0,128,0)      /*   0, 128,   0 */
#define RGB_COLOR_DARKCYAN    _rgb(0,128,128)      /*   0, 128, 128 */
#define RGB_COLOR_MAROON      _rgb(128,0,0)      /* 128,   0,   0 */
#define RGB_COLOR_PURPLE      _rgb(128,0,128)      /* 128,   0, 128 */
#define RGB_COLOR_OLIVE       _rgb(128,128,0)      /* 128, 128,   0 */
#define RGB_COLOR_LIGHTGREY   _rgb(192,192,192)      /* 192, 192, 192 */
#define RGB_COLOR_DARKGREY    _rgb(128,128,128)      /* 128, 128, 128 */
#define RGB_COLOR_BLUE        _rgb(0,0,255)      /*   0,   0, 255 */
#define RGB_COLOR_GREEN       _rgb(0,255,0)      /*   0, 255,   0 */
#define RGB_COLOR_CYAN        _rgb(0,255,255)      /*   0, 255, 255 */
#define RGB_COLOR_RED         _rgb(255,0,0)      /* 255,   0,   0 */
#define RGB_COLOR_MAGENTA     _rgb(255,0,255)      /* 255,   0, 255 */
#define RGB_COLOR_YELLOW      _rgb(255,255,0)      /* 255, 255,   0 */
#define RGB_COLOR_WHITE       _rgb(255,255,255)      /* 255, 255, 255 */
#define RGB_COLOR_ORANGE      _rgb(255,165,0)      /* 255, 165,   0 */
#define RGB_COLOR_GREENYELLOW _rgb(173,255,47)      /* 173, 255,  47 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void lcd_task(void *);
void init_dsc_lcd(void);
void lcd_cls(uint8_t colour);
void lcd_set_bl(uint8_t pwr);

extern uint8_t *frameBuffer;
extern volatile uint16_t FrameDelay;
extern volatile bool vSync;
extern TaskHandle_t xLcdTask;
extern void (*x_pixel)(uint16_t x,uint8_t y, uint8_t c);
void lcd_vsync_mode(bool enable);
void put_xpixel(uint16_t x,uint8_t y, uint8_t c);
void put_pixel(uint16_t x,uint8_t y, uint8_t c);
uint8_t get_pixel(uint16_t x,uint8_t y);

#endif //_LCD_INCLUDED