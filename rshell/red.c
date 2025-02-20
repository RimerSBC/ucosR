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
/**
 * @file red.c
 * @author Sergey Sanders
 * @date October 2024
 * @brief Rimer text EDitor
 * @brief Part of rshell
 */

#include "FreeRTOS.h"
#include "editline.h"
#include "ff.h"
#include "keyboard.h"
#include "task.h"
#include "uterm.h"
#include "commandline.h" // edit_string(,,)
#include "tstring.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define RED_MEM_BLOCK_SIZE 512
#define RED_STAT_COL_BG RGB_COLOR_LIGHTGREY
#define RED_STAT_COL_FG RGB_COLOR_BLUE
#define RED_MSG_COL_BG RGB_COLOR_BLUE
#define RED_MSG_COL_FG RGB_COLOR_YELLOW
#define RED_STATUS_LINES 1
#define RED_STAT_COL 4
#define RED_STAT_LINE 11
#define RED_STAT_NAME 18
#define RED_FILE_NAME_LEN 16

enum
{
   RENDER_FULL = 0,
   RENDER_FROM_CURSOR,
   RENDER_CLEAR_LAST,
   RENDER_CURSOR_MOVE,
};

_editline_t text;

struct
{
   char *name;
   uint16_t textOffset; // Character position
   uint16_t line;       // Line number
   bool changed;
   uint8_t col, udCol;
   uint8_t row;
} ed;

static char *statusLine;

static void stat_print_at(uint8_t start, uint8_t size)
{
   for (uint8_t c = 0; c < size; c++)
      glyph_xy(start + c, uTerm.lines - 1, glyphChCol(statusLine[c], RED_STAT_COL_FG, RED_STAT_COL_BG));
}
uint16_t lineCnt(void)
{
   uint16_t cnt = 0;
   char *ch = text.str;
   for (uint16_t i = 0; i < text.curPos; i++)
      if (*ch++ == '\n')
         cnt++;
   return cnt;
}
static void red_status_update(bool redraw)
{
   uint16_t strCol = 1;
   uint16_t curPtr = text.curPos;
   if (redraw) // init status line
   {
      memset(statusLine, ' ', uTerm.cols);
      memcpy(statusLine, "Col     Ln", 10);
      stat_print_at(0, uTerm.cols);
      tsprintf(statusLine, "%s", ed.name);
      stat_print_at(uTerm.cols - RED_FILE_NAME_LEN + 1, strlen(statusLine));
   }
   while (curPtr && text.str[curPtr--] != '\n')
      strCol++;
   tsprintf(statusLine, "%d  ", strCol);
   stat_print_at(RED_STAT_COL, 3);
   tsprintf(statusLine, "%d    ", ed.line + 1);
   stat_print_at(RED_STAT_LINE, 5);
   glyph_xy(uTerm.cols - RED_FILE_NAME_LEN, uTerm.lines - 1, glyphChCol(ed.changed ? '*' : ' ', RED_STAT_COL_FG, RED_STAT_COL_BG));
}

uint8_t strLines(void)
{
   uint16_t chrCount = 0, ptr = text.curPos;
   if (text.str[ptr] == '\n')
      return 1;                               // zero character string
   while (ptr && (text.str[ptr - 1] != '\n')) // count from cursor back
   {
      ptr--;
      chrCount++;
   }
   ptr = text.curPos;
   while (text.str[ptr] && (text.str[ptr] != '\n')) // count from cursor forward
   {
      ptr++;
      chrCount++;
   }

   return uTerm.cols ? ((chrCount / uTerm.cols) + (chrCount % uTerm.cols ? 1 : 0)) : 1;
}

void red_stat_window(uint8_t width, uint8_t height, char *msg)
{
   if (width > uTerm.cols)
      width = uTerm.cols;
   if (height > uTerm.lines)
      height = uTerm.lines;
   uint8_t left = (uTerm.cols / 2) - (width / 2);
   uint8_t up = (uTerm.lines / 2) - (height / 2);
   uint8_t r, c;
   for (r = up; r < up + height; r++)
      for (c = left; c < left + width; c++)
         glyph_xy(c, r, glyphChCol(' ', RED_MSG_COL_FG, RED_MSG_COL_BG));
   r = up + 1;
   c = left + 1;
   while (*msg)
   {
      if (*msg == '\n')
      {
         c = left + 1;
         r++;
         if (!*++msg)
            break;
         continue;
      }
      glyph_xy(c++, r, glyphChCol(*msg++, RED_MSG_COL_FG, RED_MSG_COL_BG));
   }
   cursor_move(r, c);
}

bool red_save(char *name)
{
   FIL edFile;
   UINT bw;
   if (!(*name) || (f_open(&edFile, name, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK))
      return false;
   f_write(&edFile, text.str, text.length, &bw);
   f_close(&edFile);
   if (bw == text.length)
   {
      ed.changed = false;
      return true;
   }
   return false;
}

bool red_get_filename(void)
{
   bool res;
   red_stat_window(18, 5, "Enter file name\n\n");
   tprintf("\e[a\e[38;5;14m\e[48;5;9m"); // save colour attributes, and set message colours
   res = edit_string(ed.name, RED_FILE_NAME_LEN, stdio);
   tprintf("\e[r");
   flush_stream();
   if (res && !*ed.name)
      strcpy(ed.name, "noname");
   red_status_update(true);
   return res;
}

bool red_exit_check(void)
{
   char cc;
   red_stat_window(22, 5, "     Save file?\n\n[Y]es [N]o [C]ancel?");
   while (1)
   {
      if (keyboard_getch(&cc))
         switch (cc)
         {
         case 0:
            return true; // BREAK hit
         case 'c':
         case 'C':
            return false; // Cancel
         case 'n':
         case 'N':
            return true; // Don't save
         case 'y':
         case 'Y':
            red_save(ed.name);
            return true; // Save
         }
      taskYIELD();
   }
}

void red_line_up(uint8_t linesCount)
{
   if (!linesCount)
      return;
   while (ed.textOffset < text.length - 1)
   {
      if (text.str[ed.textOffset++] == '\n')
      {
         if (!--linesCount)
            return;
      }
   }
}

void red_line_down(uint8_t linesCount)
{
   if (!linesCount)
      return;
   while (ed.textOffset)
   {
      if (text.str[ed.textOffset] == '\n')
      {
         if (!--linesCount)
            return;
      }
      ed.textOffset--;
   }
   return;
}

static void red_render(uint8_t mode)
{
   uint16_t charPtr = ed.textOffset;
   uint8_t col = 0, row = 0;
   bool print = (mode == RENDER_FULL) ? true : (text.curPos < 2) ? true :
                                                                   false;
   if (!text.curPos)
   {
      ed.col = ed.row = 0;
      cursor_move(ed.row, ed.col);
   }
   while (text.str[charPtr])
   {
      switch (text.str[charPtr])
      {
      case ' ' ... '~':
         if (print)
            glyph_xy(col, row, glyphChar(text.str[charPtr]));
         col++;
         break;
      case '\n':
         if (print)
            for (; col < uTerm.cols; col++)
               glyph_xy(col, row, glyphChar(' '));
         else
            col = uTerm.cols;
         break;
      }
      if (col == uTerm.cols)
      {
         if (row == (uTerm.lines - RED_STATUS_LINES - 1))
            return; // last line was printed
         col = 0;
         row++;
      }
      charPtr++;
      if (charPtr == text.curPos)
      {
         ed.col = col;
         ed.row = row;
         cursor_move(ed.row, ed.col);
         if (mode == RENDER_CURSOR_MOVE)
            return;
      }
      if (charPtr == text.curPos - 1)
         if (mode == RENDER_FROM_CURSOR || mode == RENDER_CLEAR_LAST)
            print = true;
   }
   for (uint8_t cc = col; cc < uTerm.cols; cc++)
      glyph_xy(cc, row, glyphChar(' ')); // clear rest of the string
   if ((mode == RENDER_CLEAR_LAST) && row < (uTerm.lines - RED_STATUS_LINES))
      for (uint8_t cc = 0; cc < uTerm.cols; cc++)
         glyph_xy(cc, row + 1, glyphChar(' ')); // clear the last string (bacspace or delete)
   if (mode == RENDER_FULL)
      //for (uint8_t rr = row + 1; rr < (uTerm.lines - RED_STATUS_LINES - 1); rr++)
      for (uint8_t rr = row + 1; rr < (uTerm.lines - RED_STATUS_LINES); rr++)
         for (uint8_t cc = 0; cc < uTerm.cols; cc++)
            glyph_xy(cc, rr, glyphChar(' ')); // clear the last string (bacspace or delete)
}

bool red_process(void)
{
   char cc;
   uint8_t lastCol = 0;
   uint8_t lineLen;
   uint8_t renderMode = RENDER_FROM_CURSOR;
   _ed_stat_t edStat;
   cursor_move(0, 0);
   while (1)
   {
      taskYIELD();
      renderMode = RENDER_FROM_CURSOR;
      if (keyboard_getch(&cc)) // if char been received
      {
         if (cc & KEY_CTRL_MASK)
         {
            cc &= ~KEY_CTRL_MASK;
            switch (cc)
            {
            case 'S':
               red_save(ed.name);
               break;
            case 'A':
               if (red_get_filename())
                  red_save(ed.name);
               renderMode = RENDER_FULL;
               break;
            }
         }
         else
            switch (edStat = editline(&text, cc))
            {
            case ED_BREAK:
               if (!ed.changed)
                  return true;
               if (red_exit_check())
                  return true;
               renderMode = RENDER_FULL;
               break;
            case ED_BACKSPACE:
            case ED_DELETE:
               renderMode = RENDER_CLEAR_LAST;
               lastCol = ed.col;
               ed.line = lineCnt();
               ed.changed = true;
               break;
            case ED_ENTER:
               str_char_ins(&text, cc);
               ed.line++;
               if (ed.row >= (uTerm.lines - RED_STATUS_LINES - 1))
               {
                  red_line_up(1);
                  renderMode = RENDER_FULL;
               }
               ed.changed = true;
            case ED_CHAR:
               lastCol = ed.col;
               if (text.length == text.maxLen)
                  vPortReAlloc(text.str, text.maxLen + RED_MEM_BLOCK_SIZE);
               if ((ed.col >= uTerm.cols - 1) && ed.row == (uTerm.lines - RED_STATUS_LINES - 1))
               {
                  red_line_up(1);
                  renderMode = RENDER_FULL;
               }
               ed.changed = true;
               break;
            case ED_UP ... ED_PGDOWN:
               renderMode = RENDER_CURSOR_MOVE;
               switch (edStat)
               {
               case ED_RIGHT:
                  if (text.curPos < text.length)
                     text.curPos++;
                  lastCol = ed.col;
                  break;
               case ED_LEFT:
                  if (text.curPos)
                     text.curPos--;
                  lastCol = ed.col;
                  break;
               case ED_HOME:
                  if (text.curPos)
                     text.curPos -= ed.col;
                  lastCol = ed.col;
                  break;
               case ED_END:
                  if (text.curPos < text.length)
                     while (text.str[text.curPos] && (text.str[text.curPos] != '\n'))
                        text.curPos++;
                  lastCol = ed.col;
                  break;
               case ED_UP:
                  lineLen = 0;
                  while (text.curPos && (text.str[--text.curPos] != '\n'))
                     ; // find current string home
                  while (text.curPos && (text.str[--text.curPos] != '\n'))
                     lineLen++; // find upper string home, and take its lenght
                  if (!ed.row)
                  {
                     red_line_down(1);
                     renderMode = RENDER_FULL;
                  }
                  if (text.curPos && text.curPos < text.length)
                     text.curPos++;
                  if (text.curPos < ed.textOffset)
                     ed.textOffset = text.curPos;
                  text.curPos += (lineLen > lastCol) ? lastCol : lineLen;
                  break;
               case ED_DOWN:
                  lineLen = 0;
                  uint8_t sLines = strLines();
                  while ((text.curPos < text.length) && (text.str[text.curPos] != '\n'))
                     text.curPos++; // find current string end
                  if (text.curPos == text.length)
                     break;
                  text.curPos++; // skip /n
                  if (ed.row >= (uTerm.lines - RED_STATUS_LINES - (sLines)))
                  {
                     red_line_up(sLines);
                     renderMode = RENDER_FULL;
                  }
                  while (text.str[text.curPos + lineLen] && (text.str[text.curPos + lineLen] != '\n'))
                     lineLen++;
                  text.curPos += (lineLen > lastCol) ? lastCol : lineLen;
                  break;
               default:
                  break;
               }
               ed.line = lineCnt();
               break;
            case ED_ESCAPE:
               break;
            case ED_IN_PROCESS:
               continue;
            }
         red_render(renderMode);
         red_status_update(false);
      }
   }
}

bool red_load(char *name)
{
   FIL edFile;
   UINT br;
   if (!(*name) || (f_open(&edFile, name, FA_READ) != FR_OK))
      return false;
   text.maxLen = (edFile.obj.objsize / RED_MEM_BLOCK_SIZE + 1) * RED_MEM_BLOCK_SIZE;
   text.str = vPortReAlloc(text.str, text.maxLen);
   f_read(&edFile, text.str, edFile.obj.objsize, &br);
   f_close(&edFile);
   text.length = br;
   return edFile.obj.objsize == br ? true : false;
}

bool red(char *fileName)
{
   memset(&ed, 0x00, sizeof(ed));
   memset(&text, 0x00, sizeof(text));
   if (!(ed.name = pvPortMalloc(RED_FILE_NAME_LEN)))
      return false;

   if (!red_load(fileName))
   {
      tsprintf(ed.name, "noname");
      if (!(text.str = vPortReAlloc(text.str, RED_MEM_BLOCK_SIZE)))
         return false;
      text.maxLen = RED_MEM_BLOCK_SIZE;
      editline_set(&text, "");
   }
   else
   {
      tsnprintf(ed.name, RED_FILE_NAME_LEN - 1, fileName);
   }
   statusLine = pvPortCalloc(1, uTerm.cols + 1);
   text_cls();
   taskYIELD();
   red_render(RENDER_FULL);
   red_status_update(true);
   red_process();
   vPortFree(statusLine);
   vPortFree(text.str);
   vPortFree(ed.name);
   text_cls();
   return true;
}

/** \TODO

 * Fix BS/DELETE - Done
 * File load - Done
 * Up at the top line -
 * Down at bootom line - done
 * ENTER at the bottom - done
 * PgUp
 * PgDn
 * Implement RENDER_FROM_CURSOR - done

*/