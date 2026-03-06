#include <stdint.h>
#include <dpmi.h>
#include <pc.h>

#include "video.h"

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