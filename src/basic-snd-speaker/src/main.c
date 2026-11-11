#include "speaker.h"
#include <stdio.h>
#include <conio.h>
#include "keyboard.h"  // borrowing from basic-keyboard example
#include "timer.h"     // borrowing from basic-timer

#define KEY_ESC 0x01
#define KEY_1 0x02
#define KEY_2 0x03
#define KEY_3 0x04

#define FX_COUNT 3
const char* fx_names[] = {"coin", "shoot", "explosion"};

int main() {
    timer_init(100);  // set timer0 to 100hz so our handler will be called every ~10ms
    keyb_init();

    clrscr();
    spk_init();
    timer_register_callback(spk_update);  // register callback to periodically update speaker state

    printf("Press:\n");
    for (int i = 0; i < FX_COUNT; i++) {
        printf(" %i to play %s effect\n", i + 1, fx_names[i]);
    }

    printf("\nor ESC to Quit\n");

    while (1) {
        if (is_key_just_pressed(KEY_ESC)) break;

        if (is_key_just_pressed(KEY_1)) spk_play(0);
        if (is_key_just_pressed(KEY_2)) spk_play(1);
        if (is_key_just_pressed(KEY_3)) spk_play(2);
    }

    clrscr();

    spk_disable();
    timer_free();
    keyb_free();
    return 0;
}