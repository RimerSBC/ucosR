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


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#undef errno
extern int errno;

#define MAX_STACK_SIZE 0x2000

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

extern int _end;

caddr_t _sbrk(int incr)
{
   static unsigned char *heap = NULL;
   unsigned char *prev_heap;
   if (heap == NULL)
   {
      heap = (unsigned char *)&_end;
   }
   prev_heap = heap;
   heap += incr;
   return (caddr_t)prev_heap;
}

int _getpid(void)
{
   return 1;
}

int _kill(int pid, int sig)
{
   errno = EINVAL;
   return -1;
}

void _exit(int status)
{
   _kill(status, -1);
   while (1)
   {
   }
}

int _write(int file, char *ptr, int len)
{
   int DataIdx;

   for (DataIdx = 0; DataIdx < len; DataIdx++)
   {
      __io_putchar(*ptr++);
   }
   return len;
}

int _close(int file)
{
   return -1;
}

int _fstat(int file, struct stat *st)
{
   st->st_mode = S_IFCHR;
   return 0;
}

int _isatty(int file)
{
   return 1;
}

int _lseek(int file, int ptr, int dir)
{
   return 0;
}

int _read(int file, char *ptr, int len)
{
   int DataIdx;

   for (DataIdx = 0; DataIdx < len; DataIdx++)
   {
      *ptr++ = __io_getchar();
   }

   return len;
}
