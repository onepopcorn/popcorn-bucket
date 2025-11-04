/*  VGA header
    - Contains all VGA registers and values
 */

#define VGA_VIDEO_INT 0x10// Video interrupt number
#define VGA_MODE_13 0x13
#define VGA_TEXT_MODE 0x03// use to set back to 80x25 text mode.

#define VGA_ADDRESS 0xA000// VGA VRAM Adrress

#define VGA_MISC_OUT_WR 0x03c2
#define VGA_MISC_OUT_RD 0x03cc


#define VGA_INPUT_STATUS 0x03da// VGA status bits
#define VGA_DISPLAY_ENABLE 0x01
#define VGA_OVERFLOW 0x07
#define VGA_VRETRACE 0x08
#define VGA_MAX_SCANLINE 0x09

#define VGA_TEXT_MODE 0x03
#define VGA_GRAPHIC_MODE 0x13

#define VGA_SEQ_INDEX 0x03c4// Sequencer controller
#define VGA_SEQ_DATA 0x03c5
#define VGA_SEQ_RESET 0x00
#define VGA_SEQ_MAP_MASK 0x02
#define VGA_SEQ_MEMORY_MODE 0x04

#define VGA_ALL_PLANES 0xff02

#define VGA_CRTC_INDEX 0x03d4// CRT Controller Index
#define VGA_CRTC_DATA 0x03d5
#define VGA_CRTC_H_TOTAL_REG 0x00    // 0..7 Horizontal total
#define VGA_CRTC_H_DISPLAY_END 0x01  // 0..7 Horizontal display end
#define VGA_CRTC_H_BLANK_START 0x02  // 0..7 Horizontal blanking start
#define VGA_CRTC_H_BLANK_END 0x03    // 0..4 Horizontal blanking end(bits 0..4); 5..6 Horizontal display skew
#define VGA_CRTC_H_RETRACE_START 0x04// 0..7 Horizontal retrace start
#define VGA_CRTC_H_RETRACE_END 0x05  // 0..4 Horizontal retrace end; 7 Horizontal blanking ed (bit 5)
#define VGA_CRTC_V_TOTAL_REG 0x06
#define VGA_CRTC_OVERFLOW_REG 0x07
#define VGA_CRTC_MAX_SCANLINE 0x09   // 0..4 Max scanline; 5 Start vertical blanking (bit9); 6 Line compare(bit 9); 7 Scan doubling
#define VGA_CRTC_V_RETRACE_START 0x10// 0..7 Vertical retrace start
#define VGA_CRTC_V_RETRACE_END 0x11  // 0..3 Vertical retrace end; 6 Bandwidth; 7 Protect
#define VGA_CRTC_V_DISPLAY_END 0x12
#define VGA_CRTC_OFFSET_REG 0x13
#define VGA_CRTC_ULINE_LOC 0x14
#define VGA_CRTC_V_BLANK_START 0x15
#define VGA_CRTC_V_BLANK_END 0x16
#define VGA_CRTC_MODE_CONTROL 0x17
#define VGA_CRTC_LINE_COMPARE 0x18


#define VGA_CRTC_HIGHADDRESS 0x0C
#define VGA_CRTC_LOWADDRESS 0x0D


#define VGA_GC_INDEX 0x03ce// VGA graphics controller
#define VGA_GC_DATA 0x03cf

#define VGA_MAP_MASK 0x02

#define VGA_PALETTE_INDEX_RD 0x03c7
#define VGA_PALETTE_INDEX_WR 0x03c8
#define VGA_PALETTE_DATA 0x03c9

#define VGA_AC_WRITE 0x3C0
#define VGA_AC_READ 0x3C1
#define VGA_AC_HPP_REG 0X20 | 0X13// Horizontal Pel Panning Register
#define VGA_AC_MODE_CONTROL 0x10  //Index of Mode COntrol register in AC
#define VGA_AC_INDEX 0x03c0       //Attribute controller index register

/* Attribute controller registers */
#define VGA_PEL_PANNING 0x13

extern int vga_scroll_y;
extern int vga_scroll_x;

void VGA_SetModeX(void);
void VGA_SetTextMode(void);
void VGA_HardwareScrolling(void);