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

#ifndef _GRAPH_H_INCLUDED
#define _GRAPH_H_INCLUDED

void line(int16_t x1, uint8_t y1, int16_t x2, uint8_t y2, uint8_t c);
void line_h(int16_t x, uint8_t y, uint16_t len, uint8_t c);
void line_v(int16_t x, uint8_t y, uint16_t len, uint8_t c);
void rect(uint16_t x, uint8_t y, uint16_t sizeX, uint8_t sizeY, uint8_t c);
void rect_fill(uint16_t x, uint8_t y, uint16_t sizeX, uint8_t sizeY, uint8_t c);
void circle(uint16_t x, uint8_t y, int16_t r, uint8_t c);
void circle_fill(uint16_t x, uint8_t y, int16_t r, uint8_t c);
void draw_icon_rle(uint16_t x, uint16_t y, uint8_t cFG, uint8_t cBG, const uint8_t *data);
#endif //_GRAPH_H_INCLUDED
