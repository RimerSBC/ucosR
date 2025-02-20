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
#ifndef TSTRING_H_INCLUDED
#define TSTRING_H_INCLUDED

#define TSTRING_VER 1.6

#include "stdarg.h"
#include "stdint.h"
#include "string.h"
#include "stdbool.h"

typedef struct
{
    void (*putch)(char);
    bool (*getch)(char *);
}_stream_io_t;

char* titoa(char *buff,int32_t val,uint8_t padding);
char* thtoa(char *buff,uint32_t val,uint8_t padding);
char* tftoa(float val,uint8_t decPlaces);

void tprintf(const char *str, ... );
void tformat(_stream_io_t *stream,const char *str,va_list *arg);
void tfprintf(_stream_io_t *stream,const char *str, ... );
int  tsprintf(char *dst,const char *str, ... );
int  tsnprintf(char *dst,uint16_t size,const char *str, ... );
void tstrncpy(char *dst,char *src,uint16_t size);
int8_t tget_enum(char *name, const char **list);
extern _stream_io_t *stdio;

#endif // TSTRING_H_INCLUDED
