#ifndef _VIDEO_H
#define _VIDEO_H

#define VIDEO_MODE_13h 0x13
#define VIDEO_MODE_Text 0x03
#define VIDEO_MEM_START 0xA0000

#define VGA_STATUS_REG 0x3DA
#define VSYNC_BITMASK 0x08

void setVideoMode(uint8_t mode);
void waitVSync();

#endif  // _VIDEO_H