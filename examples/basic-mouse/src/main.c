#include <pc.h>
#include <stdio.h>
#include <dpmi.h>
#include "mouse.h"

int main(int argc, char* argv[]) {
    // Set video mode 13h
    __dpmi_regs regs;
    regs.h.ah = 0x00;
    regs.h.al = 0x13;
    __dpmi_int(0x10, &regs);

    MouseState mouse_state;
    mouse_get_status(&mouse_state);

    if (!mouse_state.detected) {
        printf("Mouse NOT detected\n");
        return 1;
    }

    printf("Mouse detected\n");
    printf("%d buttons available\n", mouse_state.num_buttons);

    while (!kbhit()) {
        mouse_get_data(&mouse_state);

        // Mouse driver returns coordinates baed on text mode 0-639 to 0-199
        // we need to transform them for our screen size 320x200 (mode 13h)
        // that means x / 2 (or x >> 1)
        //
        // btn = buttons state
        //       - 1 = left click
        //       - 2 = right click
        //       - 3 = left & right click
        //       - 4 = middle click (if supported)
        printf("\rx=%d y=%d btn=%d  ", mouse_state.x >> 1, mouse_state.y, mouse_state.buttons);

        if (mouse_state.buttons == 1) {
            mouse_hide_pointer();
        }

        if (mouse_state.buttons == 2) {
            mouse_show_pointer();
        }
    }

    return 0;
}