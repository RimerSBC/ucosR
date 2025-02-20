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

#include "tstring.h"
#define PRINT_BUFFER_LEN 32
#define PRINT_BUFFER_LAST (PRINT_BUFFER_LEN - 1)

static const char cnvCharH[16] = "0123456789ABCDEF";
static const char cnvCharL[16] = "0123456789abcdef";

static char *buffPtr;
static uint16_t buffSize = 0;
static bool hexUC = false;

void buff_putch(char c)
{
   if (!buffSize)
      return;
   if (buffPtr)
      *buffPtr++ = c;
   buffSize--;
}

void __attribute__((weak)) uart_putch(char c)
{
}

bool __attribute__((weak)) uart_getch(char *cc)
{
   return false;
}

void __attribute__((weak)) uart_newline(void)
{
   uart_putch('\r');
   uart_putch('\n');
}

_stream_io_t UARTStream =
    {
        .putch = uart_putch,
        .getch = uart_getch,
};

_stream_io_t bufStream =
    {
        .putch = buff_putch,
        .getch = uart_getch,
};
_stream_io_t *stdio = &UARTStream;

char *titoa(char *buff, int32_t val, uint8_t padding)
{
   buff[PRINT_BUFFER_LAST] = '\0';
   int i = PRINT_BUFFER_LAST - 1;
   int sign = val < 0 ? -1 : 1;
   if (!val)
      buff[i--] = '0';
   val *= sign;
   for (; val && i; --i, val /= 10)
      buff[i] = '0' + (val % 10);
   if (padding > 1 && padding < 14)
   {
      for (; i > ((PRINT_BUFFER_LAST - 1) - padding); i--)
         buff[i] = '0';
   }
   if (sign < 0)
      buff[i--] = '-';
   return &buff[i + 1];
}
char *thtoa(char *buff, uint32_t val, uint8_t padding)
{
   char const *cnvChar = hexUC ? cnvCharH : cnvCharL;
   buff[PRINT_BUFFER_LAST] = '\0';
   int i = PRINT_BUFFER_LAST - 1;
   if (!val)
      buff[i--] = '0';
   for (; val && i; --i, val /= 16)
      buff[i] = cnvChar[val % 16];
   if (padding > 1 && padding < (PRINT_BUFFER_LAST - 1))
   {
      for (; i > ((PRINT_BUFFER_LAST - 1) - padding); i--)
         buff[i] = '0';
   }
   return &buff[i + 1];
}

char *tftoa(float val, uint8_t decPlaces)
{
   const uint32_t decMuls[] = {10, 100, 1e3, 1e4, 1e5, 1e6, 1e7};
   static char buff[16];
   buff[15] = '\0';
   int i = 14;
   int sign = val < 0 ? -1 : 1;
   val *= sign;
   if (val < 0.0000005)
      return "0.0";
   decPlaces = !decPlaces ? 6 : decPlaces > 6 ? 6 :
                                                decPlaces;
   uint32_t intPart = (uint32_t)val;
   uint32_t decPart = (uint32_t)((val - intPart) * decMuls[decPlaces]);

   if (decPart)
   {
      if (decPart % 10 > 4)
         decPart = decPart / 10 + 1; // round up
      else
         decPart /= 10;
      while (!(decPart % 10) && (decPlaces > 1)) // remove trailing zeros
      {
         decPart /= 10;
         decPlaces--;
      }
      for (; decPlaces && i; --i, decPart /= 10, decPlaces--)
         buff[i] = '0' + (decPart % 10);
      // buff[i] = cnvChar[decPart % 10];
   }
   else
      buff[i--] = '0';

   buff[i--] = '.';
   if (!intPart)
      buff[i--] = '0';
   else
      for (; intPart && i; --i, intPart /= 10)
         buff[i] = '0' + (intPart % 10);
   // buff[i] = cnvChar[intPart % 10];

   if (sign < 0)
      buff[i--] = '-';
   return &buff[i + 1];
}

// void __attribute__((optimize("3"))) tformat(_stream_io_t *stream,const char *str,va_list arg)
void tformat(_stream_io_t *stream, const char *str, va_list *arg)
{
   char numstr[PRINT_BUFFER_LEN];
   char format, c;
   double dblVar;
   char *strPtr;
   uint8_t padding;
   while (*str)
   {
      if (*str == '%')
      {
         hexUC = false;
         if (*++str != '%')
         {
            format = *str++;
            if (format == '.') // decimal places for float set by "padding"
               format = *str++;
            if (format >= '0' && format <= '9')
            {
               padding = format - '0';
               format = *str++;
            }
            else
               padding = 0;
            switch (format)
            {
            case 'c':
               c = (char)(va_arg(*arg, int));
               stream->putch(c);
               break;
            case 'f':
               dblVar = va_arg(*arg, double);
            case 'X':
               hexUC = true;
            case 'x':
            case 'd':
               strPtr = (format == 'f') ? tftoa((float)dblVar, padding) : (format == 'd') ? titoa(numstr, va_arg(*arg, int32_t), padding) :
                                                                                            thtoa(numstr, va_arg(*arg, uint32_t), padding);
               while (*strPtr)
                  stream->putch(*strPtr++);
               break;
            case 's':
               strPtr = va_arg(*arg, char *);
               while (*strPtr)
                  stream->putch(*strPtr++);
               break;
            default:
               stream->putch('%');
            }
            continue;
         }
      }
      stream->putch(*str++);
   }
}

void tprintf(const char *str, ...)
{
   va_list arg;
   va_start(arg, str);
   tformat(stdio, str, &arg);
   va_end(arg);
}

void tfprintf(_stream_io_t *stream, const char *str, ...)
{
   va_list arg;
   va_start(arg, str);
   tformat(stream, str, &arg);
   va_end(arg);
}

int tsprintf(char *dst, const char *str, ...)
{
   va_list arg;
   va_start(arg, str);
   buffSize = 0xffff;
   buffPtr = dst;
   tformat(&bufStream, str, &arg);
   buff_putch('\0');
   va_end(arg);
   return 0xffff - buffSize;
}

int tsnprintf(char *dst, uint16_t size, const char *str, ...)
{
   va_list arg;
   va_start(arg, str);
   if (!size)
      return 0;
   buffSize = size;
   buffPtr = dst;
   tformat(&bufStream, str, &arg);
   if (!buffSize)
      dst[size - 1] = '\0';
   else
      buff_putch('\0');
   return size - buffSize;
   va_end(arg);
}

void tstrncpy(char *dst, char *src, uint16_t size)
{
   if (!size)
      return;
   while (--size && *src)
   {
      *dst++ = *src++;
   }
   *dst = 0;
}

int8_t tget_enum(char *name, const char **list)
{
   int8_t i,index = 0;
   if (*name >= '0' && *name <= '9') // check for numbers
   {
      index = *name++ - '0';
      if (*name) // check for double digit numbers
         index = index * 10 + (*name - '0');
      for (i=0;i<100;i++) // check if the number in the enum range
          if (!*list++) break;
      return index < i ? index : -1;
   }
   while (*list)
   {
      char *ptr1 = name, *ptr2 = (char *)*list;
      while (*ptr1)
      {
         if (*ptr1 != *ptr2++)
            break;
         ptr1++;
      }
      if (!*ptr1)
         return index;
      list++;
      index++;
   }
   return -1;
}