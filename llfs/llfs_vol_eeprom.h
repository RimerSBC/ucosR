/**-----------------------------------------------------------------------------
 * Copyright (c) 2025 Sergey Sanders
 * sergey@sesadesign.com
 * -----------------------------------------------------------------------------
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ---------------------------------------------------------------------------*/
#ifndef _LLFS_VOL_EEPROM_H_INCLUDED
#define _LLFS_VOL_EEPROM_H_INCLUDED
#include "stdint.h"
#include "llfs.h"

#define LLFS_EMPTY_BYTE             0xff
#define LLFS_DEVID_EEPROM           0x0400
#define LLFS_EEPROM_SIZE            (64 * 1024)
#define LLFS_SECTOR_COUNT_EEPROM    256
extern volume_t *volumeEEPROM;


#endif // _LLFS_VOL_EEPROM_H_INCLUDED
