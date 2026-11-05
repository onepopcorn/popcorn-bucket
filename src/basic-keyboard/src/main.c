#include <stdio.h>
#include <conio.h>
#include "keyboard.h"

// https://kbdlayout.info/kbdusx/scancodes+virtualkeys
#define KEY_ESC 0x01
#define KEY_SPACE 0x39
#define KEY_UP (EXT_CODE_OFFSET + 0x48)
#define KEY_LEFT (EXT_CODE_OFFSET + 0x4B)
#define KEY_RIGHT (EXT_CODE_OFFSET + 0x4D)
#define KEY_DOWN (EXT_CODE_OFFSET + 0x50)

void msg(char* key, int state, int line) {
    gotoxy(1, line);
    printf("%s %i", key, state);
}

int main(int argc, char* argv[]) {
    keyb_init();

    clrscr();
    _setcursortype(_NOCURSOR);

    printf("Press ESQ to quit\n");

    int init_line = 3;
    int times = 0;

    while (1) {
        if (is_key_just_pressed(KEY_ESC)) {
            break;
        }

        msg("DOWN PRESSED:", keys[KEY_DOWN], init_line);
        msg("UP PRESSED:", keys[KEY_UP], init_line + 1);
        msg("LEFT PRESSED:", keys[KEY_LEFT], init_line + 2);
        msg("RIGHT PRESSED:", keys[KEY_RIGHT], init_line + 3);

        if (is_key_just_pressed(KEY_SPACE)) {
            times++;
        }

        gotoxy(1, init_line + 5);
        printf("SPACE PRESSED %i TIMES \n", times);
    }

    clrscr();
    keyb_free();

    return 0;
}