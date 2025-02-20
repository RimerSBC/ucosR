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

#include "keyboard.h"
#include "FreeRTOS.h"
#include "bsp.h"
#include "queue.h"
#include "semphr.h"
#include "string.h"
#include "sys_sercom.h"
#include "task.h"

#define KEY_CAPS_SHIFT 1
#define KEY_SYMBOL_SHIFT 2
#define KEY_BREAK 5
#define KEY_BACKSPACE 8
#define KEY_TAB 9
#define KEY_ESCAPE 27 // use KEY_CTRL(1)

uint8_t keyRows[8];
volatile bool zxKeyboard = false;
volatile bool kbdScanRow = false;

const char KeymapNormal[40] =
    {
        'v', 'c', 'x', 'z', KEY_CAPS_SHIFT,
        'g', 'f', 'd', 's', 'a',
        't', 'r', 'e', 'w', 'q',
        '5', '4', '3', '2', '1',
        '6', '7', '8', '9', '0',
        'y', 'u', 'i', 'o', 'p',
        'h', 'j', 'k', 'l', '\n',
        'b', 'n', 'm', 2, ' '};

const char KeymapCaps[40] =
    {
        'V', 'C', 'X', 'Z', KEY_CAPS_SHIFT,
        'G', 'F', 'D', 'S', 'A',
        'T', 'R', 'E', 'W', 'Q',
        '%', '$', '#', '@', '!',
        '&', '\'', '(', ')', '_',
        'Y', 'U', 'I', 'O', 'P',
        'H', 'J', 'K', 'L', '\n',
        'B', 'N', 'M', KEY_SYMBOL_SHIFT, KEY_BREAK};

const char KeymapSymbol[40] =
    {
        '/', '?', 'x', ':', KEY_CAPS_SHIFT,
        '}', '{', '\\', '|', '~',
        '>', '<', 'e', 'w', KEY_TAB,
        KEY_CTRL(5), '4', '3', '`', KEY_CTRL(1),
        KEY_CTRL(6), KEY_CTRL(7), KEY_CTRL(8), '9', KEY_BACKSPACE,
        '[', ']', 'i', ';', '"',
        '^', '-', '+', '=', '\n',
        '*', ',', '.', KEY_SYMBOL_SHIFT, ' '};

const char KeymapCapsSymbol[40] =
    {
        KEY_CTRL('V'), KEY_CTRL('C'), KEY_CTRL('X'), KEY_CTRL('Z'), KEY_CAPS_SHIFT,
        KEY_CTRL('G'), KEY_CTRL('F'), KEY_CTRL('D'), KEY_CTRL('S'), KEY_CTRL('A'),
        KEY_CTRL('T'), KEY_CTRL('R'), KEY_CTRL('E'), KEY_CTRL('W'), KEY_CTRL('Q'),
        KEY_CTRL('5'), KEY_CTRL('4'), KEY_CTRL('3'), KEY_CTRL('2'), KEY_CTRL('1'),
        KEY_CTRL('6'), KEY_CTRL('7'), KEY_CTRL('8'), KEY_CTRL('9'), KEY_CTRL('0'),
        KEY_CTRL('Y'), KEY_CTRL('U'), KEY_CTRL('I'), KEY_CTRL('O'), KEY_CTRL('P'),
        KEY_CTRL('H'), KEY_CTRL('J'), KEY_CTRL('K'), KEY_CTRL('L'), KEY_CTRL('\n'),
        KEY_CTRL('B'), KEY_CTRL('N'), KEY_CTRL('M'), KEY_SYMBOL_SHIFT, KEY_CTRL(' ')};

static const char *keymap[4] =
    {
        KeymapNormal,
        KeymapCaps,
        KeymapSymbol,
        KeymapCapsSymbol};
uint8_t keyTimer[40] = {0};

QueueHandle_t xKeyboartQueue = NULL;
volatile bool xBreak = false;
bool CapsShift = false;
bool SymbolShift = false;

void send_key(uint8_t scanCode)
{
   char *escSeq;
   if (scanCode < KEY_BACKSPACE)
      return;                    // caps or symbol shift
   if (scanCode & KEY_CTRL_MASK) // control character
   {
      escSeq = NULL;
      switch (scanCode & ~KEY_CTRL_MASK)
      {
      case 1: // Escape
         escSeq = "\e\e";
         break;
      case 5: // arrow left 'D'
         escSeq = "\e[D";
         break;
      case 6: // arrow down 'B'
         escSeq = "\e[B";
         break;
      case 7: // arrow up 'A'
         escSeq = "\e[A";
         break;
      case 8: // arrow right 'C'
         escSeq = "\e[C";
         break;
      case '0': // Delete
         escSeq = "\e[3~";
         break;
      case '5': // home 'H'
         escSeq = "\e[1~";
         break;
      case '6': // pgDown
         escSeq = "\e[6~";
         break;
      case '7': // pgUp
         escSeq = "\e[5~";
         break;
      case '8': // end 'F'
         escSeq = "\e[F";
         break;
      }
      if (escSeq)
      {
         vPortEnterCritical();
         while (*escSeq)
            xQueueSend(xKeyboartQueue, escSeq++, 0);
         vPortExitCritical();
         return;
      }
   }
   // else send regular character or unprocessed CTRL code
   xQueueSend(xKeyboartQueue, &scanCode, portMAX_DELAY);
}

void __attribute__((optimize("1"))) keyboard_task(void *vParam)
{
   uint8_t row, column, colInput;
   uint8_t *currentKey;
   uint8_t ctrlKeys = 0;
   xKeyboartQueue = xQueueCreate(KEYBOARD_QUEUE_LEN, sizeof(uint8_t));
   sys_kbd_write(0x03, 0xf8); // 3 lsbs are outputs, rest are inputs

   while (true)
   {
      taskYIELD();
      currentKey = keyTimer;
      for (row = 0; row < 8; row++)
      {
         sys_kbd_write(0x01, row);
         sys_kbd_read(0x00, &colInput);
         if (zxKeyboard)
         {
            keyRows[row] = (colInput >> 3) | 0xe0;
            if (!((keyRows[0] & 0x01) | (keyRows[7] & 0x03))) /// Lookinng for CS+SS+Break to for break signal
               xBreak = true;
         }
         else
         {
            for (column = 0; column < 5; column++)
            {
               *currentKey = (~colInput & (0x80 >> column)) ? (*currentKey < 255 ? *currentKey + 1 : *currentKey) : 0;
               if (*currentKey >= (KEYBOARD_KEY_DELAY + KEYBOARD_REPEAT_DELAY))
                  *currentKey -= KEYBOARD_REPEAT_DELAY; // implement
               currentKey++;
            }
            vTaskDelay(2);
         }
      }
      if (zxKeyboard)
         continue;
      ctrlKeys = keyTimer[4] ? 0x01 : 0x00;   // check for CAPS SHIFT
      ctrlKeys |= keyTimer[38] ? 0x02 : 0x00; // check for SYMBOL SHIFT
      const char *keyPtr = keymap[ctrlKeys];
      for (uint8_t k = 0; k < 40; k++)
      {
         if (keyPtr[k] == KEY_BREAK)
         {
            if (keyTimer[k] == 1)
               xBreak = true;
         }
         else if (keyTimer[k] == 1 || keyTimer[k] == KEYBOARD_KEY_DELAY)
            send_key(keyPtr[k]);
      }
   }
}

bool keyboard_getch(char *cc)
{
   while (xKeyboartQueue == NULL)
      taskYIELD();
   if (xBreak)
   {
      xBreak = false;
      *cc = 0;
      return true;
   }
   return xQueueReceive(xKeyboartQueue, cc, 0);
}

bool keyboard_wait(char *keys)
{
   char cc;
   xQueueReceive(xKeyboartQueue, &cc, portMAX_DELAY);
   return strchr(keys, cc) ? true : false;
}

bool keyboard_break(void)
{
   if (!xBreak)
      return false;
   xBreak = false;
   return true;
}

bool keyboard_pressed(void)
{
   for (uint8_t k = 0; k < 40; k++)
      if (keyTimer[k])
         return true;
   return false;
}

uint8_t keyboard_ch_ready(void)
{
   return uxQueueMessagesWaiting(xKeyboartQueue);
}

void keyboard_flush(void)
{
    xQueueReset(xKeyboartQueue);
    xBreak = false;
    return;
}