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
 
#ifndef FONTS_H_INCLUDED
#define FONTS_H_INCLUDED

/* Max size of bitmap will based on a font24 (17x24) */
#define MAX_HEIGHT_FONT         24
#define MAX_WIDTH_FONT          17

#include "bsp.h"

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

typedef struct 
{    
  const uint8_t *glyphs;
  const uint8_t *glyphs_bold;
  uint8_t height;
  uint8_t width;
  uint8_t first;
  uint8_t last;
}uFont_t;

extern const uFont_t rimer8x12;
extern const uFont_t rimer6x8;
extern const uFont_t serif8x8;
extern const uFont_t speedway8x8;
extern const uFont_t zx8x8;
extern const uFont_t clairsys8x8;
extern const uFont_t outer8x8;

#ifdef __cplusplus
}
#endif
  
#endif /* FONTS_H_INCLUDED */

/// 8x8 fonts source https://damieng.com/typography/zx-origins/