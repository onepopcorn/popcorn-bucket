#include "video.h"

/**
 * Set video to given mode
 *
 * @param mode Video mode to set
 */
void setVideoMode(uint8_t mode) {
    union REGS regs;
    regs.h.ah = 0x00;
    regs.h.al = mode;
    int86(0x10, &regs, &regs);
}

/**
 * Wait for vertical sync to start (CRT trace finishes)
 *
 */
void waitVSync() {
    // wait until retrace starts
    while ((inp(VGA_STATUS_REG) & VSYNC_BITMASK) == 0);
    // wait until retrace ends
    while ((inp(VGA_STATUS_REG) & VSYNC_BITMASK) != 0);
}