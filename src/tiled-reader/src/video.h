#ifndef _VIDEO_H
#define _VIDEO_H

#include <stdint.h>

#define VIDEO_MODE_13h 0x13
#define VIDEO_MODE_Text 0x03
#define VIDEO_MEM_START 0xA0000

#define VGA_STATUS_REG 0x3DA
#define VSYNC_BITMASK 0x08

void setVideoMode(uint8_t mode);
void waitVSync();

/** Sets the VGA palette from `count` BGRA entries (BMP palette layout, 8 bits per channel). */
void setPaletteBGRA(const uint8_t* bgra, int count);

/** Copies a w*h block between 8-bit buffers (src/dst strides in bytes). */
void blit(const uint8_t* src, int src_stride, int sx, int sy,
          uint8_t* dst, int dst_stride, int dx, int dy, int w, int h);

/** Same, but the destination is VGA memory (mode 13h, 320 wide). */
void blit_to_vga(const uint8_t* src, int src_stride, int sx, int sy,
                 int dx, int dy, int w, int h);

#endif  // _VIDEO_H