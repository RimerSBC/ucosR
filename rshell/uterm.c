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
 * @file uTerm.c
 * @author Sergey Sanders
 * @date 12 August 2022
 * @brief    Micro terminal for embedded LCD modules
 *           VT100/ANSI coding support
 *           FreeRTOS port
 *           Fixed font 8x12 pt size
 * @see ANSI/VT100
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "freeRTOS.h"
#include "task.h"
#include "lcd.h"
#include "fonts.h"
#include "keyboard.h"
#include "uterm.h"
#include "tstring.h"

glyph_t *screen = NULL; // Text storage memory
uint8_t inputStrLen = 0;
uint8_t savedCursorCol = 0;
uint8_t savedCursorLine = 0;
uint8_t savedFGColour = 0;
uint8_t savedBGColour = 0;

static const uFont_t *SysFonts[] = // system fonts list
    {
        &rimer8x12,
        &rimer6x8,
        &zx8x8,
        &serif8x8,
        &speedway8x8,
};
static struct
{
   uint8_t head;
   uint8_t tail;
   char buff[256]; // the buffer is byte sized, making faster head/tail manipulation
} inStream =
    {
        .head = 0,
        .tail = 0};

_terminal_t uTerm;

TaskHandle_t xuTermTask;

void uterm_putch(char c)
{
   while (((inStream.head + 1) & 0xff) == inStream.tail)
      taskYIELD(); // fifo is full, wait until next read
   inStream.buff[inStream.head++] = c;
}

bool get_stream(char *c)
{
   if (inStream.head == inStream.tail)
      return false;
   *c = inStream.buff[inStream.tail++];
   return true;
}

void flush_stream(void)
{
   while (inStream.head != inStream.tail)
      taskYIELD();
}

void glyph_xy(uint8_t col, uint8_t row, glyph_t glyph)
{
   uint16_t xPos = col * uTerm.font->width;
   uint16_t yPos = row * uTerm.font->height;
   if (glyph.gl.c)
      screen[row * uTerm.cols + col].data = glyph.data;
   else
      glyph.data = screen[row * uTerm.cols + col].data; // redraw the glyph
   const uint8_t *ptr = &uTerm.font->glyphs[(glyph.gl.c - uTerm.font->first) * uTerm.font->height * (uTerm.font->width > 8 ? 2 : 1)];
   for (uint16_t j = 0; j < uTerm.font->height; j++, ptr++)
   {
      for (uint16_t i = 0; i < uTerm.font->width; i++)
      {
         put_pixel(xPos + i, yPos + j, (*ptr & (0x80 >> (i % 8))) ? glyph.gl.fg : glyph.gl.bg);
      }
   }
}

void ut_new_line(bool lineReturn)
{
   if (((uTerm.cursorLine + 2) * uTerm.font->height) > LCD_HEIGHT)
   {
      uint16_t textLineSize = uTerm.font->height * LCD_WIDTH;
      /// Move graphical contents
      memcpy(frameBuffer, (uint8_t *)((uint32_t)frameBuffer + textLineSize), FB_SIZE - textLineSize);
      memset(frameBuffer + FB_SIZE - textLineSize, uTerm.bgColour, textLineSize); // erase the bottom line
      /// Move text contents
      textLineSize = (LCD_WIDTH / uTerm.font->width) * sizeof(glyph_t);
      for (uint8_t row = 0; row < uTerm.lines - 1; row++)
      {
         for (uint8_t col = 0; col < uTerm.cols; col++)
            screen[row * uTerm.cols + col] = screen[(row + 1) * uTerm.cols + col];
      }
      for (uint8_t col = 0; col < uTerm.cols; col++)
         screen[(uTerm.lines - 1) * uTerm.cols + col] = (glyph_t){
             .gl.c = ' ', .gl.fg = uTerm.fgColour, .gl.bg = uTerm.bgColour, .gl.attr = 0};
   }
   else
   {
      glyph_xy(uTerm.cursorCol, uTerm.cursorLine, screen[uTerm.cursorLine * uTerm.cols + uTerm.cursorCol]);
      uTerm.cursorLine++;
   }
   if (lineReturn)
      uTerm.cursorCol = 0;
}

void text_fg_colour(uint8_t colour)
{
   uTerm.fgColour = ANSI_pal256[colour];
}

void text_bg_colour(uint8_t colour)
{
   uTerm.bgColour = ANSI_pal256[colour];
}

uint8_t get_text_fg(uint8_t colour)
{
   return uTerm.fgColour; // = ANSI_pal256[colour];
}

uint8_t get_text_bg(uint8_t colour)
{
   return uTerm.bgColour; // = ANSI_pal256[colour];
}

void cursor_invert(void)
{
   if (!uTerm.cursorSize)
      return;
   uint16_t x = uTerm.cursorCol * uTerm.font->width;
   uint16_t y = (uTerm.cursorLine + 1) * uTerm.font->height - uTerm.cursorSize;
   uint8_t bg = screen[uTerm.cursorLine * uTerm.cols + uTerm.cursorCol].gl.bg, fg = screen[uTerm.cursorLine * uTerm.cols + uTerm.cursorCol].gl.fg;
   for (uint8_t line = 0; line < uTerm.cursorSize; line++, y++)
   {
      for (uint8_t i = 0; i < uTerm.font->width; i++)
         put_pixel(x + i, y, get_pixel(x + i, y) == bg ? fg : bg);
   }
}

void cursor_move(uint8_t line, uint8_t col)
{
   if (!(col < uTerm.cols && line < uTerm.lines))
      return; // wrong position
   glyph_xy(uTerm.cursorCol, uTerm.cursorLine, screen[uTerm.cursorLine * uTerm.cols + uTerm.cursorCol]);
   uTerm.cursorCol = col;
   uTerm.cursorLine = line;
}

void text_cls(void)
{
   lcd_cls(uTerm.bgColour);
   for (uint8_t col = 0; col < uTerm.cols; col++)
   {
      for (uint8_t row = 0; row < uTerm.lines; row++)
         screen[row * uTerm.cols + col] = (glyph_t){
             .gl.c = ' ', .gl.fg = uTerm.fgColour, .gl.bg = uTerm.bgColour, .gl.attr = 0};
   }
   uTerm.cursorLine = uTerm.cursorCol = 0;
}

enum state
{
   STATE_IDLE,
   STATE_ESC, // Escape character 0x1b
   STATE_CSI, // Control Sequence Introducer: ESC[
   STATE_OSC, // Operating System Command: ESC]
} state = STATE_IDLE;

typedef struct
{
   uint16_t argv[8];
   uint8_t argptr;
   char cmd;
} _esc_seq_t;

static _esc_seq_t ansiEscSeq =
    {
        .argptr = 0,
        .cmd = 0};

bool get_esc_seq(char c, _esc_seq_t *seq)
{
   static char str[8] = {0};
   static uint8_t ptr = 0;
   if (isdigit((int)c))
   {
      if (ptr < sizeof(str) - 2) // add a digit to the parameter string
      {
         str[ptr++] = c;
         str[ptr] = '\0';
         return false;
      }
      else
      {
         *str = ptr = 0; // clear the parameter string
         seq->cmd = 0;
         return true; // wrong command
      }
   }
   if (seq->argptr < (sizeof(seq->argv) / sizeof(seq->argv[0])))
      seq->argv[seq->argptr++] = (uint16_t)strtol(str, NULL, 10);
   *str = ptr = 0; // clear the parameter string
   if (strchr(",;", c) != NULL)
      return false; // delimiter, cmd is not ready yet
   seq->cmd = c;
   for (uint8_t i = seq->argptr; i < (sizeof(seq->argv) / sizeof(seq->argv[0])); i++)
      seq->argv[i] = 0; // reset unset parameters
   seq->argptr = 0;
   return true; // command is ready
}

bool process_csi(_esc_seq_t *escSeq) // Control Sequence Introducer
{
   uint16_t nParam = escSeq->argv[0] ? escSeq->argv[0] : 1;
   uint16_t mParam = escSeq->argv[1] ? escSeq->argv[1] : 1;
   int16_t i, j;
   switch (escSeq->cmd)
   {
   case 'A': // Cursor up
      cursor_move(nParam < uTerm.cursorLine ? uTerm.cursorLine - nParam : 0, uTerm.cursorCol);
      break;
   case 'B': // Cursor down
      cursor_move(nParam < (uTerm.lines - uTerm.cursorLine - 1) ? uTerm.cursorLine + nParam : uTerm.lines - 1, uTerm.cursorCol);
      break;
   case 'C': // Cursor forward
      if ((inputStrLen + nParam) >= UTERM_MAX_STR_LEN - 1)
         nParam = UTERM_MAX_STR_LEN - inputStrLen - 1;
      if (nParam < (i = uTerm.cols - uTerm.cursorCol))
         i = uTerm.cursorCol + nParam; // same line
      else                             // proceed to new line(s)
      {
         i -= (uTerm.cols - uTerm.cursorCol);
         for (j = 0; j <= i / uTerm.cols; j++)
            if (uTerm.cursorLine < uTerm.lines - 1)
               uTerm.cursorLine++;
         i %= uTerm.cols;
      }
      cursor_move(uTerm.cursorLine, i);
      break;
   case 'D': // Cursor back
      if ((inputStrLen + nParam) >= UTERM_MAX_STR_LEN - 1)
         nParam = UTERM_MAX_STR_LEN - inputStrLen - 1;
      if (nParam < ((i = uTerm.cursorCol) + 1))
         i = uTerm.cursorCol - nParam; // same line
      else                             // proceed to upper line(s)
      {
         for (j = 0; j <= i / uTerm.cols; j++)
            if (uTerm.cursorLine)
               uTerm.cursorLine--;
         i = uTerm.cols - (i % uTerm.cols) - 1;
      }
      cursor_move(uTerm.cursorLine, i);
      break;
   case 'E': // Cursor down, beginning of the line
      cursor_move(nParam < (uTerm.lines - uTerm.cursorLine - 1) ? uTerm.cursorLine + nParam : uTerm.lines - 1, 0);
      break;
   case 'F': // Cursor up, beginning of the line
      cursor_move(nParam < uTerm.cursorLine ? uTerm.cursorLine - nParam : 0, 0);
      break;
   case 'G': // Set column
      cursor_move(uTerm.cursorLine, nParam < uTerm.cols ? nParam - 1 : uTerm.cols - 1);
      break;
   case 'H': // Set cursor (1-columns, 1-rows)
      cursor_move(nParam < uTerm.lines ? nParam - 1 : uTerm.lines - 1, mParam < uTerm.cols ? mParam - 1 : uTerm.cols - 1);
      break;
   case 'J': // Erase screen
      switch (escSeq->argv[0])
      {
      case 0: // erase from cursor down
         if (uTerm.cursorCol < (uTerm.cols - 1))
            for (i = uTerm.cursorCol; i < uTerm.cols; i++)
               glyph_xy(i, uTerm.cursorLine, glyphChar(' ')); // clear to end of the line
         for (j = uTerm.cursorLine + 1; j < uTerm.lines; j++)
            for (i = 0; i < uTerm.cols; i++)
               glyph_xy(i, j, glyphChar(' ')); // clear to end of the screen
         break;
      case 1: // erase from cursor up
         if (uTerm.cursorCol)
            for (i = uTerm.cursorCol; i >= 0; i--)
               glyph_xy(i, uTerm.cursorLine, glyphChar(' ')); // clear to begin of the line
         if (uTerm.cursorLine)
            for (j = uTerm.cursorLine; j >= 0; j--)
               for (i = 0; i < uTerm.cols; i++)
                  glyph_xy(i, j, glyphChar(' ')); // clear to end of the screen
         break;
      case 2: // erase whole screen
      case 3: // erase whole screen
         text_cls();
         break;
      }
      break;
   case 'K': // Erase line
      switch (escSeq->argv[0])
      {
      case 0: // erase from cursor to the end
         for (i = uTerm.cursorCol; i < uTerm.cols; i++)
            glyph_xy(i, uTerm.cursorLine, glyphChar(' ')); // clear to end of the line
         break;
      case 1: // erase from cursor to the start
         for (i = uTerm.cursorCol; i >= 0; i--)
            glyph_xy(i, uTerm.cursorLine, glyphChar(' ')); // clear to begin of the line
         break;
      case 2: // erase whole line
         for (i = 0; i < uTerm.cols; i++)
            glyph_xy(i, uTerm.cursorLine, glyphChar(' ')); // clear whole line
      }
      break;
   case 's': // save current cursor position
      savedCursorCol = uTerm.cursorCol;
      savedCursorLine = uTerm.cursorLine;
      break;
   case 'u': // restore saved cursor position
      cursor_move(savedCursorLine, savedCursorCol);
      break;
   case 'a': // save current color attributes
      savedFGColour = uTerm.fgColour;
      savedBGColour = uTerm.bgColour;
      break;
   case 'r': // restore saved color attributes
      uTerm.fgColour = savedFGColour;
      uTerm.bgColour = savedBGColour;
      break;
   case 'm':
      if (escSeq->argv[0] / 10 == 3) // set foreground color
      {
         switch (escSeq->argv[0])
         {
         case 38:
            if (escSeq->argv[1] == 5)
               text_fg_colour(escSeq->argv[2]); // Set colour by index
            else if (escSeq->argv[1] == 2)
               uTerm.fgColour = _rgb(escSeq->argv[2], escSeq->argv[3], escSeq->argv[4]); // Set colour by RGB values
            return true;
         case 39:
            uTerm.fgColour = TERM_FG_COLOUR; // Set default colour
            return true;
         default:
            text_fg_colour((escSeq->argv[0] - 30) & 0x07); // Set standard colour
         }
      }
      if (escSeq->argv[0] / 10 == 4) // set background color
      {
         switch (escSeq->argv[0])
         {
         case 48:
            if (escSeq->argv[1] == 5)
               text_bg_colour(escSeq->argv[2]); // Set colour by index
            else if (escSeq->argv[1] == 2)
               uTerm.bgColour = _rgb(escSeq->argv[2], escSeq->argv[3], escSeq->argv[4]); // Set colour by RGB values
            return true;
         case 49:
            uTerm.bgColour = TERM_BG_COLOUR; // Set standard colour
            return true;
         default:
            text_bg_colour((escSeq->argv[0] - 40) & 0x07); // set background color
         }
      }
      break;
   default:
      return false;
   }
   return true;
}

bool process_osc(char c) // Operating System Command
{
   return false;
}

uint8_t set_system_font(uint8_t fontSelect)
{
   uint8_t fontsCount = sizeof(SysFonts) / sizeof(uFont_t *);
   if (fontSelect >= fontsCount)
      fontSelect = 0;
   uTerm.font = SysFonts[fontSelect];
   uTerm.cols = LCD_WIDTH / uTerm.font->width;
   uTerm.lines = LCD_HEIGHT / uTerm.font->height;
   screen = vPortReAlloc(screen, uTerm.lines * uTerm.cols * sizeof(glyph_t));
   text_cls();
   return fontSelect;
}

void uterm_task(void *vParam)
{
   char cc;
   TickType_t blinkTime;
   _stream_io_t utermStream =
       {
           .putch = uterm_putch,
           .getch = keyboard_getch,
       };
   stdio = &utermStream;

   uTerm.cursorSize = TERM_CURSOR_SIZE;
   uTerm.cursorEn = TERM_CURSOR_EN;

   text_fg_colour(sysConf.textFG);
   text_bg_colour(sysConf.textBG);

   blinkTime = xTaskGetTickCount();
   set_system_font(sysConf.font);
   while (1)
   {
      if (get_stream(&cc))
         switch (state)
         {
         case STATE_IDLE:
            switch (cc)
            {
            case '\e':
               state = STATE_ESC;
               break;
            case '\n':
               inputStrLen = 0;
               ut_new_line(1);
               break;
            case '\r':
               cursor_move(uTerm.cursorLine, 0);
               break;
            default:
               if (inputStrLen < (UTERM_MAX_STR_LEN - 1))
               {
                  glyph_xy(uTerm.cursorCol, uTerm.cursorLine, glyphChar(cc));
                  if (++uTerm.cursorCol == uTerm.cols)
                     ut_new_line(1);
               }
            }
            break;
         case STATE_ESC:
            switch (cc)
            {
            case '[':
               state = STATE_CSI;
               break;
            case ']':
               state = STATE_OSC;
               break;
            default:
               state = STATE_IDLE;
            }
            break;
         case STATE_CSI:
            if (get_esc_seq(cc, &ansiEscSeq))
            {
               if (ansiEscSeq.cmd)
                  process_csi(&ansiEscSeq);
               state = STATE_IDLE;
            }
            break;
         case STATE_OSC:
            if (process_osc(cc))
               state = STATE_IDLE;
            break;
         }
      if (xTaskGetTickCount() - blinkTime >= TERM_BLINK_PERIOD)
      {
         blinkTime = xTaskGetTickCount();
         if (uTerm.cursorEn)
            cursor_invert();
      }
      taskYIELD();
   }
}
