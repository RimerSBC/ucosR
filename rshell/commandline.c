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
 * @file commandline.c
 * @brief * command line control
 */
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "FreeRTOS.h"
#include "task.h"
#include "tstring.h"
#include "editline.h"
#include "commandline.h"

char clString[EL_LENGTH];
_editline_t commandLine =
    {
        .str = clString,
        .curPos = 0,
        .length = 0,
        .maxLen = EL_LENGTH};

static struct
{
   char line[HISTORY_DEPTH][EL_LENGTH];
   uint8_t append;
   uint8_t oldest;
   uint8_t current;
} history =
    {
        .line = {{0}},
        .append = 0,
        .oldest = 0,
        .current = 0

};

static char *history_add(char *str)
{
   if (!*str)
      return history.line[history.current]; // string is empty, don't advance
   uint8_t latest = history.append ? history.append - 1 : HISTORY_DEPTH - 1;
   if (strcmp(str, history.line[latest])) // the new line is not a copy of the previous one
   {
      strcpy(history.line[history.append], str);
      if (++history.append >= HISTORY_DEPTH)
         history.append = 0;
      if (history.append == history.oldest)
         if (++history.oldest == HISTORY_DEPTH)
            history.oldest = 0;
   }
   history.current = history.append;
   *history.line[history.append] = '\0';
   return str;
}
/*
static char *history_get_latest(void)
{
    return history.line[history.append];
}*/

static bool history_read_up(_editline_t *eLine)
{
   bool update = true;

   if (history.current == history.append)
      strcpy(history.line[history.append], eLine->str); // store the new line

   if (history.oldest == history.append)
   {
      editline_set(eLine, history.line[history.append]);
      return false; // the history is empty
   }
   if (history.current != history.oldest)
   {
      if (!history.current)
         history.current = HISTORY_DEPTH - 1;
      else
         history.current--;
   }
   else
      update = false;
   editline_set(eLine, history.line[history.current]);
   return update;
}

static bool history_read_down(_editline_t *eLine)
{
   bool update = true;
   if (history.current != history.append)
   {
      if (++history.current == HISTORY_DEPTH)
         history.current = 0;
      editline_set(eLine, history.line[history.current]);
   }
   else
      update = false;

   return update;
}

char *get_command_line(const char *prompt, _stream_io_t *stream)
{
   char cc;
   uint8_t lastLen = 0;
   _ed_stat_t edStat;
   if (*prompt)
      tfprintf(stream, "%s>", prompt);
   editline_set(&commandLine, "");
   while (1)
   {
      taskYIELD();
      if (stream->getch(&cc)) // if char been received
      {
         switch (edStat = editline(&commandLine, cc))
         {
         case ED_CHAR:
            if ((commandLine.curPos))
               tfprintf(stream, "\e[s%s\e[u\e[C", &commandLine.str[commandLine.curPos - 1]); // save,print,restore, advance
            break;
         case ED_BACKSPACE:
            tfprintf(stream, "\e[s\e[D%s \e[u\e[D", &commandLine.str[commandLine.curPos]); // save, back, print, delete(' '), restore, back
            break;

         case ED_DELETE:
            tfprintf(stream, "\e[s%s \e[u", &commandLine.str[commandLine.curPos]); // save cursor position
            break;

         case ED_ENTER:
            tfprintf(stream, "\r\n");
            return history_add(commandLine.str);

         case ED_RIGHT:
            if (commandLine.curPos < commandLine.length)
            {
               commandLine.curPos++;
               tfprintf(stream, "\e[C");
            }
            break;
         case ED_LEFT:
            if (commandLine.curPos)
            {
               commandLine.curPos--;
               tfprintf(stream, "\e[D");
            }
            break;

         case ED_HOME:
            if (commandLine.curPos)
            {
               tfprintf(stream, "\e[%dD", commandLine.curPos);
               commandLine.curPos = 0;
            }
            break;

         case ED_END:
            if (commandLine.curPos < commandLine.length)
            {
               tfprintf(stream, "\e[%dC", commandLine.length - commandLine.curPos);
               commandLine.curPos = commandLine.length;
            }
            break;

         case ED_UP:
            lastLen = commandLine.curPos;
            if (history_read_up(&commandLine))
            {
               // update the string
               if (lastLen)
                  tfprintf(stream, "\e[%dD", lastLen);
               tfprintf(stream, "%s\e[0J", commandLine.str);
            }
            break;

         case ED_DOWN:
            lastLen = commandLine.curPos;
            if (history_read_down(&commandLine))
            {
               // update the string
               if (lastLen)
                  tfprintf(stream, "\e[%dD", lastLen);
               tfprintf(stream, "%s\e[0J", commandLine.str);
            }
            break;
         case ED_ESCAPE:
         case ED_BREAK:
                // scrap the line
            tfprintf(stream, "\r\n");
            *commandLine.str = '\0';
            return commandLine.str;
         case ED_IN_PROCESS:
         case ED_PGUP:
         case ED_PGDOWN:
            break;
         }
      }
   }
}
// read string from console and store in str
bool edit_string(char *str, uint16_t size, _stream_io_t *stream)
{
   _editline_t tmpStr = {.maxLen = size};
   char cc;
   if (!(tmpStr.str = pvPortMalloc(size)))
      return false;
   tstrncpy(tmpStr.str, str, size);
   tmpStr.length = strlen(tmpStr.str);
   editline_set(&tmpStr, str);
   tfprintf(stream, "%s", str); // set the cursor at the end
   tmpStr.curPos = tmpStr.length;
   while (1)
   {
      taskYIELD();
      if (stream->getch(&cc)) // if char been received
      {
         switch (editline(&tmpStr, cc))
         {
         case ED_CHAR:
            if ((tmpStr.curPos))
               tfprintf(stream, "\e[s%s\e[u\e[C", &tmpStr.str[tmpStr.curPos - 1]); // save,print,restore, advance
            break;
         case ED_BACKSPACE:
            tfprintf(stream, "\e[s\e[D%s \e[u\e[D", &tmpStr.str[tmpStr.curPos]); // save, back, print, delete(' '), restore, back
            break;
         case ED_DELETE:
            tfprintf(stream, "\e[s%s \e[u", &tmpStr.str[tmpStr.curPos]); // save cursor position
            break;

         case ED_RIGHT:
            if (tmpStr.curPos < tmpStr.length)
            {
               tmpStr.curPos++;
               tfprintf(stream, "\e[C");
            }
            break;
         case ED_LEFT:
            if (tmpStr.curPos)
            {
               tmpStr.curPos--;
               tfprintf(stream, "\e[D");
            }
            break;
         case ED_HOME:
            if (tmpStr.curPos)
            {
               tfprintf(stream, "\e[%dD", tmpStr.curPos);
               tmpStr.curPos = 0;
            }
            break;
         case ED_END:
            if (tmpStr.curPos < tmpStr.length)
            {
               tfprintf(stream, "\e[%dC", tmpStr.length - tmpStr.curPos);
               tmpStr.curPos = tmpStr.length;
            }
            break;
         case ED_ENTER:
            tstrncpy(str, tmpStr.str, size);
            vPortFree(tmpStr.str);
            return true;
         case ED_ESCAPE:
         case ED_BREAK:
            vPortFree(tmpStr.str);
            return false;
            break;
         case ED_UP:
         case ED_DOWN:
         case ED_IN_PROCESS:
         case ED_PGUP:
         case ED_PGDOWN:
            break;
         }
      }
   }
}