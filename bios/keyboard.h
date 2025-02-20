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

#ifndef _KEYBOARD_H_INCLUDED
#define _KEYBOARD_H_INCLUDED

#include "stdbool.h"
#include "stdint.h"

#define KEYBOARD_QUEUE_LEN  16

#define KEYBOARD_KEY_DELAY  60
#define KEYBOARD_REPEAT_DELAY  6

#define KEY_CTRL_MASK   0x80
#define KEY_CTRL(k)     (k | KEY_CTRL_MASK)

extern uint8_t keyRows[8];
extern volatile bool zxKeyboard;
extern volatile bool kbdScanRow;
void keyboard_task(void *vParam);
bool keyboard_getch(char *cc); // wait for a key from keyboard and returns it in *cc
bool keyboard_wait(char *keys); // wait for a specifik key(s) described in "keys"
bool keyboard_break(void); // return false if Caps Shift + BREAK was hit
bool keyboard_pressed(void); // returns true if any key is pressed
uint8_t keyboard_ch_ready(void); // returns amount of available keys in the buffer
void keyboard_flush(void); // clear keyboard queue

#endif //_KEYBOARD_H_INCLUDED