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
/// Envious Serif font from https://damieng.com/zx-origins
#include "./fonts.h"
//
//  Font data for font_serif8x8
///
static const uint8_t serif8x8_glyphs[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //
        0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x10, 0x00, // !
        0x28, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // "
        0x00, 0x28, 0x7c, 0x28, 0x7c, 0x28, 0x00, 0x00, // #
        0x10, 0x3c, 0x50, 0x38, 0x14, 0x78, 0x10, 0x00, // $
        0x60, 0x64, 0x08, 0x10, 0x20, 0x4c, 0x0c, 0x00, // %
        0x30, 0x48, 0x40, 0x3c, 0x48, 0x48, 0x3c, 0x00, // &
        0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '
        0x08, 0x10, 0x20, 0x20, 0x20, 0x20, 0x10, 0x08, // (
        0x20, 0x10, 0x08, 0x08, 0x08, 0x08, 0x10, 0x20, // )
        0x00, 0x28, 0x38, 0x7c, 0x38, 0x28, 0x00, 0x00, // *
        0x00, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x00, 0x00, // +
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20, // ,
        0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, // -
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, // .
        0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, // /
        0x38, 0x44, 0x4c, 0x54, 0x64, 0x44, 0x38, 0x00, // 0
        0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x7c, 0x00, // 1
        0x38, 0x44, 0x04, 0x08, 0x10, 0x24, 0x7c, 0x00, // 2
        0x7c, 0x44, 0x08, 0x18, 0x04, 0x44, 0x38, 0x00, // 3
        0x08, 0x18, 0x28, 0x48, 0x7c, 0x08, 0x1c, 0x00, // 4
        0x3c, 0x20, 0x40, 0x78, 0x04, 0x44, 0x38, 0x00, // 5
        0x18, 0x20, 0x40, 0x78, 0x44, 0x44, 0x38, 0x00, // 6
        0x7c, 0x44, 0x04, 0x08, 0x08, 0x10, 0x10, 0x00, // 7
        0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x38, 0x00, // 8
        0x38, 0x44, 0x44, 0x3c, 0x04, 0x08, 0x30, 0x00, // 9
        0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00, // :
        0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x20, // ;
        0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x00, // <
        0x00, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x00, 0x00, // =
        0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x00, // >
        0x38, 0x44, 0x44, 0x08, 0x10, 0x00, 0x10, 0x00, // ?
        0x18, 0x24, 0x4c, 0x54, 0x4c, 0x20, 0x1c, 0x00, // @
        0x30, 0x10, 0x28, 0x28, 0x7c, 0x44, 0xee, 0x00, // A
        0xf8, 0x44, 0x44, 0x78, 0x44, 0x44, 0xf8, 0x00, // B
        0x38, 0x44, 0x44, 0x40, 0x40, 0x44, 0x38, 0x00, // C
        0xf0, 0x48, 0x44, 0x44, 0x44, 0x48, 0xf0, 0x00, // D
        0xfc, 0x44, 0x40, 0x70, 0x40, 0x44, 0xfc, 0x00, // E
        0xfc, 0x44, 0x40, 0x70, 0x40, 0x40, 0xe0, 0x00, // F
        0x38, 0x44, 0x40, 0x4c, 0x44, 0x4c, 0x34, 0x00, // G
        0xee, 0x44, 0x44, 0x7c, 0x44, 0x44, 0xee, 0x00, // H
        0x7c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7c, 0x00, // I
        0x0e, 0x04, 0x04, 0x04, 0x44, 0x44, 0x38, 0x00, // J
        0xec, 0x48, 0x50, 0x60, 0x50, 0x48, 0xec, 0x00, // K
        0xe0, 0x40, 0x40, 0x40, 0x40, 0x44, 0xfc, 0x00, // L
        0xc6, 0x6c, 0x6c, 0x54, 0x54, 0x44, 0xee, 0x00, // M
        0xce, 0x44, 0x64, 0x54, 0x4c, 0x44, 0xee, 0x00, // N
        0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, // O
        0xf8, 0x44, 0x44, 0x78, 0x40, 0x40, 0xe0, 0x00, // P
        0x38, 0x44, 0x44, 0x44, 0x54, 0x48, 0x34, 0x04, // Q
        0xf8, 0x44, 0x44, 0x78, 0x50, 0x48, 0xec, 0x00, // R
        0x3c, 0x44, 0x40, 0x38, 0x04, 0x44, 0x78, 0x00, // S
        0x7c, 0x54, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, // T
        0xee, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, // U
        0xee, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10, 0x00, // V
        0xee, 0x44, 0x54, 0x54, 0x6c, 0x6c, 0x44, 0x00, // W
        0xee, 0x44, 0x28, 0x10, 0x28, 0x44, 0xee, 0x00, // X
        0xee, 0x44, 0x28, 0x28, 0x10, 0x10, 0x38, 0x00, // Y
        0x7c, 0x44, 0x08, 0x10, 0x20, 0x44, 0x7c, 0x00, // Z
        0x1c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1c, // [
        0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, // \ (backslash)
        0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, // ]
        0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, // ^
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, // _
        0x18, 0x24, 0x20, 0x78, 0x20, 0x20, 0x7c, 0x00, // £
        0x00, 0x00, 0x38, 0x04, 0x3c, 0x44, 0x3c, 0x00, // a
        0xc0, 0x40, 0x58, 0x64, 0x44, 0x44, 0xf8, 0x00, // b
        0x00, 0x00, 0x38, 0x44, 0x40, 0x40, 0x3c, 0x00, // c
        0x0c, 0x04, 0x34, 0x4c, 0x44, 0x44, 0x3e, 0x00, // d
        0x00, 0x00, 0x38, 0x44, 0x7c, 0x40, 0x3c, 0x00, // e
        0x18, 0x24, 0x20, 0x70, 0x20, 0x20, 0x70, 0x00, // f
        0x00, 0x00, 0x36, 0x4c, 0x44, 0x3c, 0x04, 0x38, // g
        0xc0, 0x40, 0x58, 0x64, 0x44, 0x44, 0xee, 0x00, // h
        0x10, 0x00, 0x30, 0x10, 0x10, 0x10, 0x38, 0x00, // i
        0x08, 0x00, 0x1c, 0x08, 0x08, 0x08, 0x48, 0x30, // j
        0xe0, 0x40, 0x4c, 0x48, 0x70, 0x48, 0xec, 0x00, // k
        0x30, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, // l
        0x00, 0x00, 0xe8, 0x54, 0x54, 0x44, 0xee, 0x00, // m
        0x00, 0x00, 0xd8, 0x64, 0x44, 0x44, 0xee, 0x00, // n
        0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00, // o
        0x00, 0x00, 0xd8, 0x64, 0x44, 0x78, 0x40, 0xe0, // p
        0x00, 0x00, 0x36, 0x4c, 0x44, 0x3c, 0x04, 0x0e, // q
        0x00, 0x00, 0xd8, 0x64, 0x44, 0x40, 0xe0, 0x00, // r
        0x00, 0x00, 0x3c, 0x40, 0x38, 0x04, 0x78, 0x00, // s
        0x20, 0x20, 0x78, 0x20, 0x20, 0x24, 0x18, 0x00, // t
        0x00, 0x00, 0xee, 0x44, 0x44, 0x4c, 0x36, 0x00, // u
        0x00, 0x00, 0xee, 0x44, 0x28, 0x28, 0x10, 0x00, // v
        0x00, 0x00, 0xee, 0x44, 0x54, 0x54, 0x28, 0x00, // w
        0x00, 0x00, 0xcc, 0x48, 0x30, 0x48, 0xcc, 0x00, // x
        0x00, 0x00, 0xee, 0x44, 0x28, 0x10, 0x10, 0x60, // y
        0x00, 0x00, 0x7c, 0x48, 0x10, 0x24, 0x7c, 0x00, // z
        0x0c, 0x10, 0x10, 0x60, 0x10, 0x10, 0x10, 0x0c, // {
        0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // |
        0x60, 0x10, 0x10, 0x0c, 0x10, 0x10, 0x10, 0x60, // }
        0x00, 0x00, 0x24, 0x54, 0x48, 0x00, 0x00, 0x00, // ~
        0x38, 0x44, 0x92, 0xaa, 0xa2, 0x9a, 0x44, 0x38, // ©
};
static const uint8_t serif8x8_glyphs_B[] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //
        0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00, // !
        0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // "
        0x00, 0x2c, 0x7e, 0x2c, 0x7e, 0x2c, 0x00, 0x00, // #
        0x08, 0x3e, 0x60, 0x3c, 0x06, 0x7c, 0x10, 0x00, // $
        0x62, 0x66, 0x0c, 0x18, 0x30, 0x66, 0x46, 0x00, // %
        0x38, 0x6c, 0x60, 0x3e, 0x6c, 0x6c, 0x3a, 0x00, // &
        0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '
        0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c, // (
        0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x30, // )
        0x00, 0x28, 0x38, 0x7c, 0x38, 0x28, 0x00, 0x00, // *
        0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00, // +
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, // ,
        0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, // -
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, // .
        0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60, // /
        0x18, 0x24, 0x66, 0x66, 0x66, 0x24, 0x18, 0x00, // 0
        0x18, 0x38, 0x58, 0x18, 0x18, 0x18, 0x7e, 0x00, // 1
        0x3c, 0x46, 0x06, 0x0c, 0x18, 0x32, 0x7e, 0x00, // 2
        0x7e, 0x46, 0x0c, 0x1c, 0x06, 0x66, 0x3c, 0x00, // 3
        0x0c, 0x1c, 0x2c, 0x4c, 0x7e, 0x0c, 0x1e, 0x00, // 4
        0x3e, 0x30, 0x60, 0x7c, 0x06, 0x46, 0x3c, 0x00, // 5
        0x1c, 0x30, 0x60, 0x7c, 0x66, 0x66, 0x3c, 0x00, // 6
        0x7e, 0x46, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x00, // 7
        0x3c, 0x66, 0x66, 0x3c, 0x66, 0x66, 0x3c, 0x00, // 8
        0x3c, 0x66, 0x66, 0x3e, 0x06, 0x0c, 0x38, 0x00, // 9
        0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, // :
        0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x30, // ;
        0x0c, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0c, 0x00, // <
        0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00, // =
        0x60, 0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00, // >
        0x3c, 0x66, 0x66, 0x0c, 0x18, 0x00, 0x18, 0x00, // ?
        0x3c, 0x66, 0xce, 0xd6, 0xce, 0x60, 0x3c, 0x00, // @
        0x18, 0x18, 0x2c, 0x2c, 0x7e, 0x46, 0xef, 0x00, // A
        0xfc, 0x66, 0x66, 0x7c, 0x66, 0x66, 0xfc, 0x00, // B
        0x3a, 0x66, 0xc6, 0xc0, 0xc0, 0x66, 0x3c, 0x00, // C
        0xf8, 0x6c, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00, // D
        0xfe, 0x62, 0x60, 0x78, 0x60, 0x62, 0xfe, 0x00, // E
        0xfe, 0x62, 0x60, 0x78, 0x60, 0x60, 0xf0, 0x00, // F
        0x3c, 0x62, 0xc0, 0xce, 0xc6, 0x66, 0x3a, 0x00, // G
        0xe7, 0x66, 0x66, 0x7e, 0x66, 0x66, 0xe7, 0x00, // H
        0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, // I
        0x1e, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00, // J
        0xf7, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0xf7, 0x00, // K
        0xf0, 0x60, 0x60, 0x60, 0x60, 0x62, 0xfe, 0x00, // L
        0xe7, 0x7e, 0x7e, 0x66, 0x66, 0x66, 0xef, 0x00, // M
        0xef, 0x66, 0x76, 0x7e, 0x6e, 0x66, 0xef, 0x00, // N
        0x38, 0x6c, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x00, // O
        0xfc, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00, // P
        0x38, 0x6c, 0xc6, 0xc6, 0xda, 0x6c, 0x36, 0x00, // Q
        0xfc, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0xf7, 0x00, // R
        0x7a, 0xe6, 0x70, 0x38, 0x1c, 0xce, 0xbc, 0x00, // S
        0x7e, 0x5a, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, // T
        0xef, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, // U
        0xef, 0x46, 0x46, 0x2c, 0x2c, 0x18, 0x18, 0x00, // V
        0xe7, 0x66, 0x66, 0x7e, 0x7e, 0x66, 0x42, 0x00, // W
        0xef, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xf7, 0x00, // X
        0xef, 0x46, 0x2c, 0x3c, 0x18, 0x18, 0x3c, 0x00, // Y
        0x7e, 0x46, 0x0c, 0x18, 0x30, 0x62, 0x7e, 0x00, // Z
        0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, // [
        0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, // \ (backslash)
        0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, // ]
        0x18, 0x3c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, // ^
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, // _
        0x1c, 0x32, 0x30, 0x7c, 0x30, 0x30, 0x7e, 0x00, // £
        0x00, 0x00, 0x3c, 0x06, 0x3e, 0x66, 0x3e, 0x00, // a
        0xe0, 0x60, 0x6c, 0x76, 0x66, 0x66, 0xfc, 0x00, // b
        0x00, 0x00, 0x3c, 0x62, 0x60, 0x60, 0x3e, 0x00, // c
        0x1c, 0x0c, 0x6c, 0xdc, 0xcc, 0xcc, 0x7e, 0x00, // d
        0x00, 0x00, 0x3c, 0x66, 0x7e, 0x60, 0x3e, 0x00, // e
        0x1c, 0x32, 0x30, 0x78, 0x30, 0x30, 0x78, 0x00, // f
        0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0x78, // g
        0xe0, 0x60, 0x6c, 0x76, 0x66, 0x66, 0xef, 0x00, // h
        0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00, // i
        0x0c, 0x00, 0x1c, 0x0c, 0x0c, 0x0c, 0x4c, 0x38, // j
        0xe0, 0x60, 0x6e, 0x6c, 0x78, 0x6c, 0xee, 0x00, // k
        0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, // l
        0x00, 0x00, 0xec, 0x7e, 0x7e, 0x66, 0xf7, 0x00, // m
        0x00, 0x00, 0xec, 0x76, 0x66, 0x66, 0xef, 0x00, // n
        0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x3c, 0x00, // o
        0x00, 0x00, 0xec, 0x76, 0x66, 0x7c, 0x60, 0xf0, // p
        0x00, 0x00, 0x6e, 0xdc, 0xcc, 0x7c, 0x0c, 0x1e, // q
        0x00, 0x00, 0xdc, 0x76, 0x66, 0x60, 0xf0, 0x00, // r
        0x00, 0x00, 0x3e, 0x70, 0x3c, 0x0e, 0x7c, 0x00, // s
        0x30, 0x30, 0x7c, 0x30, 0x30, 0x32, 0x1c, 0x00, // t
        0x00, 0x00, 0xef, 0x66, 0x66, 0x6e, 0x37, 0x00, // u
        0x00, 0x00, 0xef, 0x46, 0x2c, 0x2c, 0x18, 0x00, // v
        0x00, 0x00, 0xef, 0x66, 0x7e, 0x7e, 0x2c, 0x00, // w
        0x00, 0x00, 0xce, 0x7c, 0x38, 0x7c, 0xe6, 0x00, // x
        0x00, 0x00, 0xef, 0x46, 0x3c, 0x18, 0x30, 0xe0, // y
        0x00, 0x00, 0x7e, 0x4c, 0x18, 0x32, 0x7e, 0x00, // z
        0x0e, 0x18, 0x18, 0x70, 0x18, 0x18, 0x18, 0x0e, // {
        0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, // |
        0x70, 0x18, 0x18, 0x0e, 0x18, 0x18, 0x18, 0x70, // }
        0x00, 0x00, 0x66, 0xd6, 0xcc, 0x00, 0x00, 0x00, // ~
        0x3c, 0x42, 0x99, 0xb5, 0xb1, 0x9d, 0x42, 0x3c, // ©
};

const uFont_t serif8x8 =
    {
        .glyphs = serif8x8_glyphs,
        .glyphs_bold = serif8x8_glyphs_B,
        .width = 8,
        .height = 8,
        .first = 32,
        .last = 127,
        .name = "Serif",
};
