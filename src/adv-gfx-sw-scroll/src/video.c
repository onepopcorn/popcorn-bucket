#include <stdint.h>
#include <dpmi.h>
#include <pc.h>
#include <go32.h>

#include "video.h"
#include "assets/palette.h"

/**
 * Set video to given mode
 *
 * @param mode Video mode to set
 */
void setVideoMode(uint8_t mode) {
    __dpmi_regs regs = {0};
    regs.h.ah = 0x00;
    regs.h.al = mode;
    __dpmi_int(0x10, &regs);
}

void setVideoPalette() {
    outportb(0x3C8, 0);
    for (int i = 0; i < 256; i++) {
        outportb(0x3C9, palette[i].r);
        outportb(0x3C9, palette[i].g);
        outportb(0x3C9, palette[i].b);
    }
}

/**
 * Wait for vertical sync to start (CRT trace finishes)
 *
 */
void waitVSync() {
    // wait until current vsync ends if in the middle of one
    while ((inportb(VGA_STATUS_REG) & VSYNC_BITMASK) != 0);
    // wait until next vsync starts
    while ((inportb(VGA_STATUS_REG) & VSYNC_BITMASK) == 0);
}
