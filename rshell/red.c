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
#define RED_MAX_TEXT_LEN (0x10000 - 2 * RED_MEM_BLOCK_SIZE)

typedef enum
{
   RED_LOAD_OK = 0, // the whole file is in the buffer
   RED_LOAD_NOFILE, // no such file, start a new one
   RED_LOAD_TOOBIG, // too big for uint16_t offsets
   RED_LOAD_NOMEM,  // no room on the heap
   RED_LOAD_ERROR,  // short read
} _red_load_t;

static _editline_t text;

static struct
{
   char *name;
   uint32_t textOffset; // first character of the top displayed line
   uint32_t line;       // line number the cursor is on, counted from 0
   bool changed;
   uint16_t col, row; // where the cursor sits on the screen, see cursor_locate()
} ed;

static char *statusLine;

static void stat_print_at(uint8_t start, uint8_t size)
{
   for (uint8_t c = 0; c < size; c++)
      glyph_xy(start + c, uTerm.lines - 1, glyphChCol(statusLine[c], RED_STAT_COL_FG, RED_STAT_COL_BG));
}

static uint32_t line_start(uint32_t pos) // first character of the line holding pos
{
   while (pos && (text.str[pos - 1] != '\n'))
      pos--;
   return pos;
}

static uint32_t line_end(uint32_t pos) // the '\n' that closes it, or the terminator
{
   while (text.str[pos] && (text.str[pos] != '\n'))
      pos++;
   return pos;
}

static uint32_t line_next(uint32_t pos) // start of the line below, pos's own if it is the last
{
   uint32_t end = line_end(pos);
   return text.str[end] ? end + 1 : line_start(pos);
}

/// same column on the line above, or as near to it as that line reaches
static uint32_t pos_up(uint32_t pos, uint16_t aimCol)
{
   uint32_t start = line_start(pos), up;
   uint32_t len; // characters on the line above
   if (!start)
      return pos; // already on the first line
   up = line_start(start - 1);
   len = (start - 1) - up;
   return up + (len > aimCol ? aimCol : len);
}

/// same column on the line below
static uint32_t pos_down(uint32_t pos, uint16_t aimCol)
{
   uint32_t end = line_end(pos), down, len;
   if (!text.str[end])
      return pos; // already on the last line
   down = end + 1;
   len = line_end(down) - down;
   return down + (len > aimCol ? aimCol : len);
}
static uint32_t lineCnt(void)
{
   uint32_t cnt = 0;
   char *ch = text.str;
   for (uint32_t i = 0; i < text.curPos; i++)
      if (*ch++ == '\n')
         cnt++;
   return cnt;
}

static void red_status_update(void)
{
   uint32_t strCol = text.curPos - line_start(text.curPos) + 1;
   uint8_t nameAt = uTerm.cols - RED_FILE_NAME_LEN + 1;
   char num[12];
   uint8_t len;
   memset(statusLine, ' ', uTerm.cols);
   memcpy(statusLine, "Col     Ln", 10);
   tsnprintf(num, sizeof(num), "%d", strCol);
   len = strlen(num);
   memcpy(statusLine + RED_STAT_COL, num, len > 4 ? 4 : len);
   tsnprintf(num, sizeof(num), "%d", ed.line + 1);
   len = strlen(num);
   memcpy(statusLine + RED_STAT_LINE, num, len > 5 ? 5 : len);
   if (uTerm.cols >= (RED_FILE_NAME_LEN + RED_STAT_NAME)) // room for the name field
   {
      statusLine[nameAt - 1] = ed.changed ? '*' : ' ';
      len = strlen(ed.name);
      memcpy(statusLine + nameAt, ed.name, len > (RED_FILE_NAME_LEN - 1) ? (RED_FILE_NAME_LEN - 1) : len);
   }
   stat_print_at(0, uTerm.cols);
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
      if ((c < (left + width)) && (r < (up + height)))
         glyph_xy(c, r, glyphChCol(*msg, RED_MSG_COL_FG, RED_MSG_COL_BG));
      c++;
      msg++;
   }
   cursor_move(r, c);
}

static void red_message(char *msg)
{
   char cc;
   red_stat_window(24, 5, msg);
   while (!keyboard_getch(&cc))
      taskYIELD();
}

bool red_save(char *name)
{
   FIL edFile;
   UINT bw;
   char tmpName[RED_FILE_NAME_LEN + 4]; // "name.$$$"
   if (!(*name))
      return false;
   tsnprintf(tmpName, sizeof(tmpName), "%s.$$$", name);
   if (f_open(&edFile, tmpName, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
      return false;
   if ((f_write(&edFile, text.str, text.length, &bw) != FR_OK) || (bw != text.length))
   {
      f_close(&edFile);
      f_unlink(tmpName);
      return false;
   }
   if (f_close(&edFile) != FR_OK)
   {
      f_unlink(tmpName);
      return false;
   }
   f_unlink(name); // may not be there yet, f_rename() would refuse to overwrite
   if (f_rename(tmpName, name) != FR_OK)
      return false; // the temporary file is the only copy now, leave it alone
   ed.changed = false;
   return true;
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
   red_status_update();
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
            if (!red_save(ed.name))
            {
               red_message("  Save failed\n\n  Press any key");
               return false;
            }
            return true; // Save
         }
      taskYIELD();
   }
}

static void red_window_down(uint8_t linesCount) // window moves towards the end
{
   while (linesCount--)
   {
      uint32_t next = line_next(ed.textOffset);
      if (next == ed.textOffset)
         break; // the last line is already at the top
      ed.textOffset = next;
   }
}

static void cell_advance(char ch, uint16_t *col, uint16_t *row)
{
   *col = (ch == '\n') ? uTerm.cols : *col + 1; // a newline fills the rest of the row
   if (*col >= uTerm.cols)
   {
      *col = 0;
      (*row)++;
   }
}

static void cursor_locate(void)
{
   uint16_t col = 0, row = 0;
   for (uint32_t ptr = ed.textOffset; ptr < text.curPos; ptr++)
      cell_advance(text.str[ptr], &col, &row);
   ed.col = col;
   ed.row = row;
}

static void window_follow(void)
{
   uint16_t textRows = uTerm.lines - RED_STATUS_LINES;
   if (text.curPos < ed.textOffset) // above the window, put its line on top
      ed.textOffset = line_start(text.curPos);
   cursor_locate();
   while (ed.row >= textRows) // below it, scroll a line at a time
   {
      uint32_t was = ed.textOffset;
      red_window_down(1);
      if (ed.textOffset == was)
         break; // nothing left to scroll
      cursor_locate();
   }
}

static void red_render(void)
{
   uint32_t charPtr = ed.textOffset;
   uint16_t col = 0, row = 0;
   uint16_t textRows = uTerm.lines - RED_STATUS_LINES;
   while (text.str[charPtr] && (row < textRows))
   {
      switch (text.str[charPtr])
      {
      case ' ' ... '~':
         glyph_xy(col, row, glyphChar(text.str[charPtr]));
         break;
      case '\n':
         for (uint16_t c = col; c < uTerm.cols; c++)
            glyph_xy(c, row, glyphChar(' '));
         break;
      default:
         glyph_xy(col, row, glyphChar(' '));
         break;
      }
      cell_advance(text.str[charPtr], &col, &row);
      charPtr++;
   }
   for (; row < textRows; row++, col = 0) // blank the rest of the window
      for (; col < uTerm.cols; col++)
         glyph_xy(col, row, glyphChar(' '));
   cursor_move(ed.row, ed.col);
}

static bool red_text_grow(void)
{
   char *newStr;
   if (text.maxLen > (RED_MAX_TEXT_LEN - RED_MEM_BLOCK_SIZE))
      return false; // uint16_t offsets are exhausted
   if (!(newStr = vPortReAlloc(text.str, text.maxLen + RED_MEM_BLOCK_SIZE)))
      return false;
   text.str = newStr;
   text.maxLen += RED_MEM_BLOCK_SIZE;
   return true;
}

bool red_process(void)
{
   char cc;
   uint16_t lastCol = 0; // column to aim for when moving up or down
   _ed_stat_t edStat = ED_IN_PROCESS;
   window_follow();
   red_render();
   while (1)
   {
      taskYIELD();
      if (!keyboard_getch(&cc)) // nothing pressed
         continue;
      if (cc & KEY_CTRL_MASK)
      {
         cc &= ~KEY_CTRL_MASK;
         switch (cc)
         {
         case 's':
         case 'S':
            if (!red_save(ed.name))
               red_message("  Save failed\n\n  Press any key");
            break;
         case 'a':
         case 'A':
            if (red_get_filename() && !red_save(ed.name))
               red_message("  Save failed\n\n  Press any key");
            break;
         }
      }
      else
      {
         if (text.length >= (text.maxLen - 3))
            red_text_grow();
         switch (edStat = editline(&text, cc))
         {
         case ED_BREAK:
            if (!ed.changed)
               return true;
            if (red_exit_check())
               return true;
            break;
         case ED_BACKSPACE:
         case ED_DELETE:
            ed.line = lineCnt();
            ed.changed = true;
            break;
         case ED_ENTER:
            if (!str_char_ins(&text, cc))
               break;
            ed.line++;
            ed.changed = true;
            break;
         case ED_CHAR:
            ed.changed = true;
            break;
         case ED_UP ... ED_PGDOWN:
            switch (edStat)
            {
            case ED_RIGHT:
               if (text.curPos < text.length)
                  text.curPos++;
               break;
            case ED_LEFT:
               if (text.curPos)
                  text.curPos--;
               break;
            case ED_HOME:
               text.curPos = line_start(text.curPos);
               break;
            case ED_END:
               text.curPos = line_end(text.curPos);
               break;
            case ED_UP:
               text.curPos = pos_up(text.curPos, lastCol);
               break;
            case ED_DOWN:
               text.curPos = pos_down(text.curPos, lastCol);
               break;
            case ED_PGUP:
            case ED_PGDOWN:
            {
               uint16_t rows = uTerm.lines - RED_STATUS_LINES - 1;
               while (rows--)
               {
                  uint32_t was = text.curPos;
                  text.curPos = (edStat == ED_PGUP) ? pos_up(text.curPos, lastCol) : pos_down(text.curPos, lastCol);
                  if (text.curPos == was)
                     break; // hit the top or the bottom of the text
               }
               break;
            }
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
      }
      window_follow();
      if ((edStat != ED_UP) && (edStat != ED_DOWN) && (edStat != ED_PGUP) && (edStat != ED_PGDOWN))
         lastCol = ed.col; // ... and the column to come back to is the real one
      red_render();
      red_status_update();
   }
}

_red_load_t red_load(char *name)
{
   FIL edFile;
   UINT br;
   uint32_t fSize, blockLen; // file size and the buffer it needs, in full width
   char *newStr;
   if (!(*name) || (f_open(&edFile, name, FA_READ) != FR_OK))
      return RED_LOAD_NOFILE;
   fSize = (uint32_t)edFile.obj.objsize;
   blockLen = (fSize / RED_MEM_BLOCK_SIZE + 1) * RED_MEM_BLOCK_SIZE;
   if (blockLen > RED_MAX_TEXT_LEN) // uint16_t offsets cannot reach that far
   {
      f_close(&edFile);
      return RED_LOAD_TOOBIG;
   }
   if (!(newStr = vPortReAlloc(text.str, blockLen)))
   {
      f_close(&edFile);
      return RED_LOAD_NOMEM;
   }
   text.str = newStr;
   text.maxLen = (uint16_t)blockLen;
   f_read(&edFile, text.str, fSize, &br);
   f_close(&edFile);
   text.length = br;
   text.str[text.length] = '\0';
   return fSize == br ? RED_LOAD_OK : RED_LOAD_ERROR;
}

bool red(char *fileName)
{
   _red_load_t loaded;
   memset(&ed, 0x00, sizeof(ed));
   memset(&text, 0x00, sizeof(text));
   if (!(ed.name = pvPortMalloc(RED_FILE_NAME_LEN)))
      return false;

   if ((loaded = red_load(fileName)) == RED_LOAD_OK)
      tsnprintf(ed.name, RED_FILE_NAME_LEN, "%s", fileName);
   else if (loaded == RED_LOAD_NOFILE)
   {
      tsnprintf(ed.name, RED_FILE_NAME_LEN, "%s", *fileName ? fileName : "noname");
      if (!(text.str = vPortReAlloc(text.str, RED_MEM_BLOCK_SIZE)))
      {
         vPortFree(ed.name); // CLAUDE: this exit used to leak ed.name
         return false;
      }
      text.maxLen = RED_MEM_BLOCK_SIZE;
      editline_set(&text, "");
   }
   else // the file is there but cannot be held: leave it alone
   {
      tprintf(loaded == RED_LOAD_TOOBIG ? "red: %s is too big\r\n" : "red: cannot read %s\r\n", fileName);
      vPortFree(text.str);
      vPortFree(ed.name);
      return false;
   }
   if (!(statusLine = pvPortCalloc(1, uTerm.cols + 1)))
   {
      vPortFree(text.str);
      vPortFree(ed.name);
      return false;
   }
   text_cls();
   taskYIELD();
   red_render();
   red_status_update();
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
 * Up at the top line - done, window_follow()
 * Down at bootom line - done, window_follow()
 * ENTER at the bottom - done
 * PgUp - done
 * PgDn - done
 * Implement RENDER_FROM_CURSOR - CLAUDE: dropped, glyph_xy() skips the cells
   that did not change, so a full repaint costs what a partial one used to
*/