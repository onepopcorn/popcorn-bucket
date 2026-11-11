#include "timer.h"
#include <stdio.h>
#include <conio.h>
#include <stdint.h>
#include <crt0.h>

/**
 * With this we are disabling memory paging globally so
 * our interrupt handlers and the variables it touches
 * are always in the active memory page and can't try
 * to access non existing memory or garbage accidentally
 *
 * This is the pragmatic but drastic solution. A more fine
 * tuned alternative exists but it means cherry picking
 * variables and functions to lock individually and we will
 * avoid it in this example for the sake of simplicity
 *
 * more info https://delorie.com/djgpp/v2faq/faq18_9.html
 *
 */
int _crt0_startup_flags = _CRT0_FLAG_LOCK_MEMORY;

volatile uint32_t ticks = 0;    // both need to be volatile because they are modified inside an interrupt
volatile uint32_t seconds = 0;  // and volatile prevents the compiler to do some optimizations that could cause bugs

void update() {
    ticks++;
    if (ticks % TIMER_FREQ == 0) seconds++;
}

int main() {
    clrscr();
    _setcursortype(_NOCURSOR);

    timer_register_callback(update);  // register before starting the timer
    timer_init();

    gotoxy(1, 1);
    printf("Game ticks (every 10ms): ");

    gotoxy(1, 2);
    printf("Seconds: ");

    while (!kbhit()) {
        gotoxy(26, 1);
        printf("%lu", ticks);
        gotoxy(10, 2);
        printf("%lu", seconds);
    }

    timer_free();
    return 0;
}