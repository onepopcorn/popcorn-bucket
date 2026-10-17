#include <stdint.h>
#include <dpmi.h>
#include <pc.h>
#include <string.h>
#include <sys/movedata.h>

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

void blit(const uint8_t* src, int src_stride, int sx, int sy,
          uint8_t* dst, int dst_stride, int dx, int dy, int w, int h) {
    src += sy * src_stride + sx;
    dst += dy * dst_stride + dx;
    for (int row = 0; row < h; row++) {
        memcpy(dst, src, w);
        src += src_stride;
        dst += dst_stride;
    }
}

void blit_to_vga(const uint8_t* src, int src_stride, int sx, int sy,
                 int dx, int dy, int w, int h) {
    src += sy * src_stride + sx;
    for (int row = 0; row < h; row++) {
        dosmemput(src, w, VIDEO_MEM_START + (dy + row) * 320 + dx);
        src += src_stride;
    }
}

void setPaletteBGRA(const uint8_t* bgra, int count) {
    outportb(0x3C8, 0);
    for (int i = 0; i < count; i++) {
        outportb(0x3C9, bgra[i * 4 + 2] >> 2);
        outportb(0x3C9, bgra[i * 4 + 1] >> 2);
        outportb(0x3C9, bgra[i * 4] >> 2);
    }
}
