#include <stdio.h>
#include <conio.h>
#include "video.h"
#include <sys/nearptr.h>
#include <string.h>

#define MODE_13h 0x13
#define MODE_Text 0x03
#define VIDEO_MEM_START 0xA0000
#define SCREEN_WIDTH 320

int main(int argc, char* argv[]) {
    // We need to disable DJGPP's memory protection
    // to access memory below 1MB
    if (__djgpp_nearptr_enable() == 0) {
        return 1;
    }

    // Set VGA mode to 13h
    setVideoMode(MODE_13h);

    // wait for vsync before writting to VRAM
    waitVSync();
    uint8_t* screen_buffer = (uint8_t*)(VIDEO_MEM_START + __djgpp_conventional_base);

    uint8_t x = 40, y = 40;
    uint8_t w = 50, h = 50;

    // Draw a red square of 50x50 px writting
    // directly to video memory
    //
    // NOTE:
    //     Writing to video memory directly is not what you would
    //     do in a real game, it's used here for the sake of simplicity
    //     Refere to more advance examples to see better rendering techniques
    for (uint8_t row = 0; row < h; row++) {
        memset(screen_buffer + x + (y + row) * SCREEN_WIDTH, 0x04, w);
    }

    while (!kbhit()) {
    }

    // Go back to text mode
    setVideoMode(MODE_Text);

    // Enable DJGPP memory protection again
    __djgpp_nearptr_disable();
    return 0;
}