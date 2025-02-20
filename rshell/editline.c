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
 * @file editline.c
 * @brief * ANSI command line editor
 */
#include "editline.h"

#include "FreeRTOS.h"
#include "task.h"
#include "tstring.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

void editline_set(_editline_t* eLine, char* str)
{
   tstrncpy(eLine->str, str, eLine->maxLen - 1);
   eLine->curPos = eLine->length = strlen(eLine->str);
}

bool str_char_ins(_editline_t* eLine, char c)
{
   if(eLine->length >= (eLine->maxLen - 2)) return false;
   for(uint16_t i = eLine->length + 1; i > eLine->curPos; i--) eLine->str[i] = eLine->str[i - 1];
   eLine->str[eLine->curPos++] = c;
   eLine->length++;
   return true;
}

bool str_char_del(_editline_t* eLine)
{
   if(!eLine->length) return false;
   char* str = &eLine->str[eLine->curPos];
   while(*str++) *(str - 1) = *str;
   eLine->length--;
   return true;
}

_ed_stat_t editline(_editline_t* eLine, char cc)
{
   static enum { CHAR, ESC, CMD, PARAM } state = CHAR;
   static char escParam;
   switch(state)
   {
   case CHAR:
      switch(cc)
      {
      case 0:
            return ED_BREAK;
      case 27:
         state = ESC;
         break;
      case 8:
         if(eLine->curPos)
         {
            eLine->curPos--;
            if(str_char_del(eLine)) return ED_BACKSPACE;
         }
         break;
      case '\n':
         return ED_ENTER;
      case ' ' ... '~': /// printable character
         return str_char_ins(eLine, cc) ? ED_CHAR : ED_IN_PROCESS;
      default:
         break;
      }
      break;
   case ESC:
      switch(cc)
      {
      case '[':
         state = CMD;
         break;
      case 'O':
         state = PARAM;
         break;
      case 27:
         return ED_ESCAPE;         
      default:
         state = CHAR;
         break;
      }
      break;
   case CMD:
      state = CHAR;
      switch(cc)
      {
      case 'A':
         return ED_UP;
      case 'B':
         return ED_DOWN;
      case 'C':
         return ED_RIGHT;
      case 'D':
         return ED_LEFT;
      case 'F':
         return ED_END;
      }
      escParam = cc;
      state = PARAM;
      break;
   case PARAM:
      state = CHAR;
      if(cc == '~') switch(escParam)
         {
         case '1':
            return ED_HOME;
         case '3':
            if(eLine->curPos == eLine->length) return ED_IN_PROCESS;
            str_char_del(eLine);
            return ED_DELETE;
         case '5':
            return ED_PGUP;
         case '6':
            return ED_PGDOWN;
         }
      if(cc == 'F') return ED_END;
      break;
   }
   return ED_IN_PROCESS;
}
