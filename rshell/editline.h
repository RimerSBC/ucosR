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

#ifndef _EDITLINE_INCLUDED
#define _EDITLINE_INCLUDED

#include "stdint.h"
#include "tstring.h"

#define EL_LENGTH 128

typedef struct 
{
    char *str;
    uint16_t curPos;
    uint16_t length;
    uint16_t maxLen;
} _editline_t;

typedef enum
{
    ED_IN_PROCESS,  // no process
    ED_BREAK,       // CAPS BREAK
    ED_ENTER,       // re-print/update
    ED_CHAR,        // re-print
    ED_BACKSPACE,   // re-print
    ED_DELETE,      // re-print
    ED_UP,
    ED_RIGHT,
    ED_LEFT,
    ED_DOWN,
    ED_HOME,
    ED_END,
    ED_PGUP,
    ED_PGDOWN,
    ED_ESCAPE
} _ed_stat_t;

void editline_set(_editline_t *eLine,char *str); 
_ed_stat_t editline(_editline_t *eLine,char cc);
bool str_char_ins(_editline_t* eLine, char c);
bool str_char_del(_editline_t* eLine);

#endif //_EDITLINE_INCLUDED