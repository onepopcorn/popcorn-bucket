#ifndef _KEYB_H
#define _KEYB_H

#include <stdint.h>

#define EXT_CODE_OFFSET 0x80  // We'll store extended characters passed this offest

extern volatile uint8_t keys[0xFF];  // Array big enough to hold all keycodes. It can be queried from other translation units
uint8_t is_key_just_pressed(uint8_t key);

void keyb_init();
void keyb_free();

#endif  // _KEYB_H