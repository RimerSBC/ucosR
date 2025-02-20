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
#include "FreeRTOS.h"
#include "semphr.h"
#include <string.h>
#include "bsp.h"
#include "graph.h"
#include "lcd.h"
#include "fonts.h"
#include "tstring.h"

void line(int16_t x1, uint8_t y1, int16_t x2, uint8_t y2, uint8_t c)
{
   // Bresenham algorithm
   int16_t dx = x2 - x1 >= 0 ? x2 - x1 : x1 - x2;
   int16_t sx = x1 < x2 ? 1 : -1;
   int16_t dy = y2 - y1 <= 0 ? y2 - y1 : y1 - y2;
   int16_t sy = y1 < y2 ? 1 : -1;
   int16_t err = dx + dy;

   if (!dx) // verical line
   {
      for (uint16_t i = y1 < y2 ? y1 : y2; i < (y1 > y2 ? y1 : y2); i++)
         x_pixel(x1, i, c);
      return;
   }

   if (!dy) // horizontal line
   {
      for (uint16_t i = x1 < x2 ? x1 : x2; i < (x1 > x2 ? x1 : x2); i++)
         x_pixel(i, y1, c);
      return;
   }
   while (1)
   {
      x_pixel(x1, y1, c);
      sx = (x1 == x2) ? 0 : sx;
      sy = (y1 == y2) ? 0 : sy;
      if (!sx && !sy)
         return;
      if (2 * err >= dy)
      {
         err += dy;
         x1 += sx;
      }
      if (2 * err <= dx)
      {
         err += dx;
         y1 += sy;
      }
   }
}

void line_h(int16_t x, uint8_t y, uint16_t len, uint8_t c)
{
   if (x < 0)
   {
      len += x;
      x = 0;
   }
   for (uint16_t i = x; i < (uint16_t)(x + len); i++)
      x_pixel(i, y, c);
}
void line_v(int16_t x, uint8_t y, uint16_t len, uint8_t c)
{
   if (x < 0)
   {
      len += x;
      x = 0;
   }
   for (uint16_t i = y; i < (uint16_t)(y + len); i++)
      x_pixel(x, i, c);
}

void rect(uint16_t x, uint8_t y, uint16_t sizeX, uint8_t sizeY, uint8_t c)
{
   line_h(x, y, sizeX, c);
   line_h(x, y + sizeY, sizeX, c);
   line_v(x, y, sizeY, c);
   line_v(x + sizeX, y, sizeY + 1, c);
}

void rect_fill(uint16_t x, uint8_t y, uint16_t sizeX, uint8_t sizeY, uint8_t c)
{
   for (uint16_t i = y; i <= y + sizeY; i++)
      line_h(x, i, sizeX, c);
}

void circle(uint16_t x, uint8_t y, int16_t r, uint8_t c)
{
   /* Bresenham algorithm */
   int16_t x_pos = -r;
   int16_t y_pos = 0;
   int16_t err = 2 - 2 * r;
   int16_t e2;
   do
   {
      x_pixel(x - x_pos, y + y_pos, c);
      x_pixel(x + x_pos, y + y_pos, c);
      x_pixel(x + x_pos, y - y_pos, c);
      x_pixel(x - x_pos, y - y_pos, c);
      e2 = err;
      if (e2 <= y_pos)
      {
         err += ++y_pos * 2 + 1;
         if (-x_pos == y_pos && e2 <= x_pos)
         {
            e2 = 0;
         }
      }
      if (e2 > x_pos)
      {
         err += ++x_pos * 2 + 1;
      }
   } while (x_pos <= 0);
}

void circle_fill(uint16_t x, uint8_t y, int16_t r, uint8_t c)
{
   // Bresenham algorithm
   int16_t x_pos = -r;
   int16_t y_pos = 0;
   int16_t err = 2 - 2 * r;
   int16_t e2;

   do
   {
      line_h(x + x_pos, y + y_pos, 2 * (-x_pos) + 1, c);
      if (y_pos)
      {
         line_h(x + x_pos, y - y_pos, 2 * (-x_pos) + 1, c);
      }
      e2 = err;
      if (e2 <= y_pos)
      {
         err += ++y_pos * 2 + 1;
         if (-x_pos == y_pos && e2 <= x_pos)
         {
            e2 = 0;
         }
      }
      if (e2 > x_pos)
      {
         err += ++x_pos * 2 + 1;
      }
   } while (x_pos <= 0);
}

void draw_icon_rle(uint16_t x, uint16_t y, uint8_t cFG, uint8_t cBG, const uint8_t *data)
{
#define COMPRESS_FLAG 0x80
#define COLOUR_FLAG 0x40
   uint8_t w = *data++;
   uint8_t h = *data++;
   uint8_t size = *data++;
   uint8_t xx = 0, yy = 0;

   while (size--)
   {
      if (*data & COMPRESS_FLAG)
      {
         for (uint8_t i = 0; i < (*data & 0x3f); i++)
         {
            x_pixel(x + xx, y + yy, (*data & COLOUR_FLAG) ? cFG : cBG);
            if (++xx == w)
            {
               if (++yy == h)
                  return;
               xx = 0;
            }
         }
      }
      else
      {
         for (uint8_t i = 0; i < 7; i++)
         {
            x_pixel(x + xx, y + yy, (*data & (0x01 << i)) ? cFG : cBG);
            if (++xx == w)
            {
               if (++yy == h)
                  return;
               xx = 0;
            }
         }
      }
      data++;
   }
}