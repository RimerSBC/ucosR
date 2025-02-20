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

#ifndef _SYS_SERCOM_H_INCLUDED
#define _SYS_SERCOM_H_INCLUDED

#define SYS_SERCOM3_QUEUE_LEN       4

#define I2C_LONG_ADDR_MASK          0x8000

#define I2C_DEVID_PI4IOE5V9554      0x20
#define I2C_DEVID_PI4IOE5V9554A     0x70    // 8 bit notation
#define I2C_IO_EXTENDER(addr)   (I2C_DEVID_PI4IOE5V9554A | ((addr & 0x07) << 1))

#define I2C_DEVID_EEPROM            0xA0    // 8 bit notation
#define I2C_EEPROM_PAGE_SIZE        128
#define I2C_EEPROM(addr)   (I2C_DEVID_EEPROM | ((addr & 0x07) << 1) | I2C_LONG_ADDR_MASK)

#include "stdbool.h"
#include "stdint.h"

void sys_sercom3_task(void *vParam);

bool sys_kbd_write(uint8_t cmd,uint8_t data);
bool sys_kbd_read(uint8_t cmd,uint8_t *data);

bool sys_eeprom_write(uint16_t addr,uint8_t *data,uint16_t size);
bool sys_eeprom_read(uint16_t addr,uint8_t *data,uint16_t size);

#endif //_SYS_SERCOM_H_INCLUDED