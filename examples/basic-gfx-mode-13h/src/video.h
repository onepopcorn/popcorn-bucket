#ifndef _VIDEO_H
#define _VIDEO_H

#include <stdint.h>
#include <dos.h>

#define VGA_STATUS_REG 0x3DA
#define VSYNC_BITMASK 0x08

void setVideoMode(uint8_t mode);
void waitVSync();

#endif  // _VIDEO_H