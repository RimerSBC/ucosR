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
#ifndef UTERM_H_INCLUDED
#define UTERM_H_INCLUDED

#include <stdint.h>
#include "freeRTOS.h"
#include "task.h"
#include "fonts.h"
#include "colours.h"
#include "lcd.h"

#define UTERM_MAX_STR_LEN 80

#define TERM_CURSOR_SIZE 2
#define TERM_CURSOR_EN 1
#define TERM_FG_COLOUR 0x10   // Darkgreen
#define TERM_BG_COLOUR 0      // Black
#define TERM_BLINK_PERIOD 300 // mSec

#define glyphChar(CC) ((glyph_t){.gl.c = CC, .gl.fg = uTerm.fgColour, .gl.bg = uTerm.bgColour, .gl.attr = 0})
#define glyphChCol(CC, FG, BG) ((glyph_t){.gl.c = CC, .gl.fg = FG, .gl.bg = BG, .gl.attr = 0})

#define ANSI_CLS "\e[2J"
typedef union
{
   struct
   {
      uint8_t c;    // character
      uint8_t fg;   // foreground colour
      uint8_t bg;   // background colour
      uint8_t attr; // charachter attribute
   } gl;
   uint32_t data;
} glyph_t;

typedef struct __attribute__((packed))
{
   uint16_t cols;
   uint16_t lines;
   const uFont_t *font;
   uint8_t cursorCol;
   uint8_t cursorLine;
   uint8_t cursorSize;
   uint8_t cursorEn;
   uint8_t fgColour;
   uint8_t bgColour;
} _terminal_t;

/// shadow character buffer
extern glyph_t *screen;
extern _terminal_t uTerm;
extern TaskHandle_t xuTermTask;

void uterm_task(void *vParam);

void text_fg_colour(uint8_t colour);
void text_bg_colour(uint8_t colour);
void text_cls(void);
void cursor_move(uint8_t line, uint8_t col);
void glyph_xy(uint8_t col, uint8_t row, glyph_t glyph);
void ut_new_line(bool lineReturn);
void cursor_invert(void);
void flush_stream(void);
uint8_t set_system_font(uint8_t fontSelect);

void uterm_putch(char c);
bool uterm_getch(char *cc);

#endif // UTERM_H_INCLUDED