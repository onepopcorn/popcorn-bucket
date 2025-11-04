#include <bios.h>
#include <dpmi.h>

#include "vga.h"
#include "types.h"

word vga_page[3] = {0, 64, 336};// page 0 is reserved for split window
word vga_width = 352;
word vga_height = 256;
word vga_visible_width = 320;
word vga_visible_height = 240;
int vga_scroll_y;
int vga_scroll_x;

word vram_LogicalWidth;// screen logical with on bytes in vram

byte pix;
byte p[4] = {0, 2, 4, 6};

/* Wait VSync */
void VGA_VSync(void) {
	/* wait until any previous retrace has ended */
	do {
	} while (inportb(VGA_INPUT_STATUS) & 8);

	/* wait until a new retrace has just begun */
	do {
	} while (!(inportb(VGA_INPUT_STATUS) & 8));
}

/** Sets VGA 256-color palette entry at specified index
 *
 * index must be between 0 and 255, and each color component (r, g, b) must be
 * between 0 and 63.
 */
void VGA_SetPalette(byte *palette) {
	int i;
	outportb(VGA_PALETTE_INDEX_WR, 0);
	for (i = 0; i < 255 * 3; i++) {
		outportb(VGA_PALETTE_DATA, palette[i]);// Set the 256 palette colors
	}
}

static void VGA_MISC_VsyncPolarity(bool negative) {
	byte aux;
	// Miscelaneous output register (0x3C2)
	// - Bit 7. Vertical syncronization signal polarity. 0=positive; 1=negative
	// - Bit 6. Horizontal syncronization signal polarity. 0=positive; 1=negative
	// - Bit 5. Odd/Even page select. 0=low page; 1=high page
	// - Bits 3..2. Clock select. 00=25Mhz; 01=28Mhz; 10=??; 11=?
	// - Bit 1. RAM enable. 0=disables address decode for display buffer
	// - Bit 0. Input/Output address select. Selects the CRT controller address. 0=3Bx; 1=3Dx
	aux = inportb(VGA_MISC_OUT_RD);
	if (negative) aux = aux | 0x80;//0b1000 0000
	else
		aux = aux & 0x7F;//0b0111 1111
	outportb(VGA_MISC_OUT_WR, aux);
}

static void VGA_MISC_HsyncPolarity(bool negative) {
	byte aux;
	// Miscelaneous output register (0x3C2)
	// - Bit 7. Vertical syncronization signal polarity. 0=positive; 1=negative
	// - Bit 6. Horizontal syncronization signal polarity. 0=positive; 1=negative
	// - Bit 5. Odd/Even page select. 0=low page; 1=high page
	// - Bits 3..2. Clock select. 00=25Mhz; 01=28Mhz; 10=??; 11=?
	// - Bit 1. RAM enable. 0=disables address decode for display buffer
	// - Bit 0. Input/Output address select. Selects the CRT controller address. 0=3Bx; 1=3Dx
	aux = inportb(VGA_MISC_OUT_RD);
	if (negative) aux = aux | 0x40;//0b0100 0000
	else
		aux = aux & 0xBF;//0b1011 1111
	outportb(VGA_MISC_OUT_WR, aux);
}

static void VGA_MISC_OddEvenPage(bool low) {
	byte aux;
	// Miscelaneous output register (0x3C2)
	// - Bit 7. Vertical syncronization signal polarity. 0=positive; 1=negative
	// - Bit 6. Horizontal syncronization signal polarity. 0=positive; 1=negative
	// - Bit 5. Odd/Even page select. 0=low page; 1=high page
	// - Bits 3..2. Clock select. 00=25Mhz; 01=28Mhz; 10=??; 11=?
	// - Bit 1. RAM enable. 0=disables address decode for display buffer
	// - Bit 0. Input/Output address select. Selects the CRT controller address. 0=3Bx; 1=3Dx
	aux = inportb(VGA_MISC_OUT_RD);
	if (low) aux = aux | 0x20;//0b0010 0000
	else
		aux = aux & 0xDF;//0b1101 1111
	outportb(VGA_MISC_OUT_WR, aux);
}

static void VGA_MISC_ClockSelect(bool mhz25) {
	byte aux;
	// Miscelaneous output register (0x3C2)
	// - Bit 7. Vertical syncronization signal polarity. 0=positive; 1=negative
	// - Bit 6. Horizontal syncronization signal polarity. 0=positive; 1=negative
	// - Bit 5. Odd/Even page select. 0=low page; 1=high page
	// - Bits 3..2. Clock select. 00=25Mhz; 01=28Mhz; 10=??; 11=?
	// - Bit 1. RAM enable. 0=disables address decode for display buffer
	// - Bit 0. Input/Output address select. Selects the CRT controller address. 0=3Bx; 1=3Dx
	aux = inportb(VGA_MISC_OUT_RD);
	if (mhz25) aux = aux & 0xF3;//0bxxxx 00xx
	else {
		aux = aux & 0xF7;//0b1111 0111
		aux = aux | 0x04;//0b0000 0100
	}
	outportb(VGA_MISC_OUT_WR, aux);
}

static void VGA_MISC_RAMEnable(bool enable) {
	byte aux;
	// Miscelaneous output register (0x3C2)
	// - Bit 7. Vertical syncronization signal polarity. 0=positive; 1=negative
	// - Bit 6. Horizontal syncronization signal polarity. 0=positive; 1=negative
	// - Bit 5. Odd/Even page select. 0=low page; 1=high page
	// - Bits 3..2. Clock select. 00=25Mhz; 01=28Mhz; 10=??; 11=?
	// - Bit 1. RAM enable. 0=disables address decode for display buffer
	// - Bit 0. Input/Output address select. Selects the CRT controller address. 0=3Bx; 1=3Dx
	aux = inportb(VGA_MISC_OUT_RD);
	if (enable) aux = aux | 0x02;//0b0000 0010
	else
		aux = aux & 0xFD;//0b1111 1101
	outportb(VGA_MISC_OUT_WR, aux);
}

static void VGA_MISC_IOAddressSelect(bool color) {
	byte aux;
	// Miscelaneous output register (0x3C2)
	// - Bit 7. Vertical syncronization signal polarity. 0=positive; 1=negative
	// - Bit 6. Horizontal syncronization signal polarity. 0=positive; 1=negative
	// - Bit 5. Odd/Even page select. 0=low page; 1=high page
	// - Bits 3..2. Clock select. 00=25Mhz; 01=28Mhz; 10=??; 11=?
	// - Bit 1. RAM enable. 0=disables address decode for display buffer
	// - Bit 0. Input/Output address select. Selects the CRT controller address. 0=3Bx; 1=3Dx
	aux = inportb(VGA_MISC_OUT_RD);
	if (color) aux = aux | 0x01;//0b0000 0001
	else
		aux = aux & 0xFE;//0b1111 1110
	outportb(VGA_MISC_OUT_WR, aux);
}

// Enable Chain-4 *
static void VGA_SEQ_Chain4Disable(bool disable) {
	byte aux;
	// Sequencer port (0x3C4)
	// Register 0x04 - Memory mode register
	// - Bit 3. Chain 4 disable
	// - Bit 2. Odd/Even disable
	// - Bit 1. Extended memory. Enables access to 256 kb of VRAM
	outportb(VGA_SEQ_INDEX, VGA_SEQ_MEMORY_MODE);
	aux = inportb(VGA_SEQ_DATA);
	if (disable) aux = aux | 0x08;//0b0000 1000
	else
		aux = aux & 0xF7;//0b1111 0111
	outportb(VGA_SEQ_DATA, aux);
}

// Odd/even disable *
static void VGA_SEQ_OddEvenDisable(bool disable) {
	byte aux;
	// Sequencer port (0x3C4)
	// Register 0x04 - Memory mode register
	// - Bit 3. Chain 4
	// - Bit 2. Odd/Even disable
	// - Bit 1. Extended memory. Enables access to 256 kb of VRAM
	outportb(VGA_SEQ_INDEX, VGA_SEQ_MEMORY_MODE);
	aux = inportb(VGA_SEQ_DATA);
	if (disable) aux = aux | 0x04;//0bxxxx x1xx
	else
		aux = aux & 0xFB;//01111 1011
	outportb(VGA_SEQ_DATA, aux);
}

// Enable extended mem. *
static void VGA_SEQ_ExtMem(bool enable) {
	byte aux;
	// Sequencer port (0x3C4)
	// Register 0x04 - Memory mode register
	// - Bit 3. Chain 4
	// - Bit 2. Odd/Even disable
	// - Bit 1. Extended memory. Enables access to 256 kb of VRAM
	outportb(VGA_SEQ_INDEX, VGA_SEQ_MEMORY_MODE);// 0x04>>Port 0x3C4. Selects memory mode register (bit3.Chain 4;bit2.Odd/even disable;bit1.Ext.mem.)
	aux = inportb(VGA_SEQ_DATA);
	if (enable) aux = aux | 0x02;//0bxxxx xx1x
	else
		aux = aux & 0xFD;//0b1111 1101
	outportb(VGA_SEQ_DATA, aux);
}

//Double word addressing *
static void VGA_CRT_DoubleWordAddressing(bool enable) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x14 - Underline location register
	// - Bit 6. Double word. Enables double word addressing
	// - Bit 5. Divide memory address clock by 4
	// - Bits 0..4. Underline location
	outportb(VGA_CRTC_INDEX, VGA_CRTC_ULINE_LOC);
	aux = inportb(VGA_CRTC_DATA);
	if (enable) aux = aux | 0x40;
	else
		aux = aux & 0xBF;
	outportb(VGA_CRTC_DATA, aux);
}

// Divide memory address clock by 4 *
static void VGA_CRT_DivideMemClockBy4(bool enable) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x14 - Underline location register
	// - Bit 6. Double word. Enables double word addressing
	// - Bit 5. Divide memory address clock by 4
	// - Bits 0..4. Underline location
	outportb(VGA_CRTC_INDEX, VGA_CRTC_ULINE_LOC);
	aux = inportb(VGA_CRTC_DATA);
	if (enable) aux = aux | 0x20;
	else
		aux = aux & 0xDF;
	outportb(VGA_CRTC_DATA, aux);
}

// Set underline location *
static void VGA_CRT_SetUnderlineLocation(byte scanline) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x14 - Underline location register
	// - Bit 6. Double word. Enables double word addressing
	// - Bit 5. Divide memory address clock by 4
	// - Bits 0..4. Underline location (set underline on scanline number -1)
	outportb(VGA_CRTC_INDEX, VGA_CRTC_ULINE_LOC);
	aux = inportb(VGA_CRTC_DATA);
	aux = aux | (scanline & 0x1F);
	outportb(VGA_CRTC_DATA, aux);
}

// Sync enable *
static void VGA_CRT_SyncEnable(bool enable) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x17 - Mode control register
	// - Bit 7. Sync enable. Enables vertical and horizontal retrace signals
	// - Bit 6. Word/byte mode selection. 0=word; 1=byte (only available if dword mode is disabled)
	// - Bit 5. Address wrap select. 1= selects MA15 for accessing 256 kb of vram; 0= selects MA13 for 64kb accessing
	// - Bit 3. Divide memory clock by 2.
	// - Bit 2. Divide scanline clock by 2.
	// - Bit 1. Map display address 14.
	// - Bit 0. Map display address 13.
	outportb(VGA_CRTC_INDEX, VGA_CRTC_MODE_CONTROL);
	aux = inportb(VGA_CRTC_DATA);
	if (enable) aux = aux | 0x80;//0b1xxx xxxx
	else
		aux = aux & 0xEF;
	outportb(VGA_CRTC_DATA, aux);
}

// Byte or word addressing mode selection *
static void VGA_CRT_ByteMode(bool byteMode) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x17 - Mode control register
	// - Bit 7. Sync enable. Enables vertical and horizontal retrace signals
	// - Bit 6. Word/byte mode selection. 0=word; 1=byte (only available if dword mode is disabled)
	// - Bit 5. Address wrap select. 1= selects MA15 for accessing 256 kb of vram; 0= selects MA13 for 64kb accessing
	// - Bit 3. Divide memory clock by 2.
	// - Bit 2. Divide scanline clock by 2.
	// - Bit 1. Map display address 14.
	// - Bit 0. Map display address 13.
	outportb(VGA_CRTC_INDEX, VGA_CRTC_MODE_CONTROL);
	aux = inportb(VGA_CRTC_DATA);
	if (byteMode) aux = aux | 0x40;//0bx1xx xxxx
	else
		aux = aux & 0xBF;
	outportb(VGA_CRTC_DATA, aux);
}

// Address wrap select *
static void VGA_CRT_AddressWrapSelect(bool MA15) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x17 - Mode control register
	// - Bit 7. Sync enable. Enables vertical and horizontal retrace signals
	// - Bit 6. Word/byte mode selection. 0=word; 1=byte (only available if dword mode is disabled)
	// - Bit 5. Address wrap select. 1= selects MA15 for accessing 256 kb of vram; 0= selects MA13 for 64kb accessing
	// - Bit 3. Divide memory clock by 2.
	// - Bit 2. Divide scanline clock by 2.
	// - Bit 1. Map display address 14.
	// - Bit 0. Map display address 13.
	outportb(VGA_CRTC_INDEX, VGA_CRTC_MODE_CONTROL);
	aux = inportb(VGA_CRTC_DATA);
	if (MA15) aux = aux | 0x20;//0bxx1x xxxx
	else
		aux = aux & 0xDF;
	outportb(VGA_CRTC_DATA, aux);
}

// Divide mem clock by 2 *
static void VGA_CRT_DivideMemClockBy2(bool enable) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x17 - Mode control register
	// - Bit 7. Sync enable. Enables vertical and horizontal retrace signals
	// - Bit 6. Word/byte mode selection. 0=word; 1=byte (only available if dword mode is disabled)
	// - Bit 5. Address wrap select. 1= selects MA15 for accessing 256 kb of vram; 0= selects MA13 for 64kb accessing
	// - Bit 3. Divide memory clock by 2.
	// - Bit 2. Divide scanline clock by 2.
	// - Bit 1. Map display address 14.
	// - Bit 0. Map display address 13.
	outportb(VGA_CRTC_INDEX, VGA_CRTC_MODE_CONTROL);
	aux = inportb(VGA_CRTC_DATA);
	if (enable) aux = aux | 0x08;//0bxxxx 1xxx
	else
		aux = aux & 0xF7;//0b1111 0111
	outportb(VGA_CRTC_DATA, aux);
}

// Divide scanline clock by 2 *
static void VGA_CRT_DivideScanlineClockBy2(bool enable) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x17 - Mode control register
	// - Bit 7. Sync enable. Enables vertical and horizontal retrace signals
	// - Bit 6. Word/byte mode selection. 0=word; 1=byte (only available if dword mode is disabled)
	// - Bit 5. Address wrap select. 1= selects MA15 for accessing 256 kb of vram; 0= selects MA13 for 64kb accessing
	// - Bit 3. Divide memory clock by 2.
	// - Bit 2. Divide scanline clock by 2.
	// - Bit 1. Map display address 14.
	// - Bit 0. Map display address 13.
	outportb(VGA_CRTC_INDEX, VGA_CRTC_MODE_CONTROL);
	aux = inportb(VGA_CRTC_DATA);
	if (enable) aux = aux | 0x04;//0bxxxx x1xx
	else
		aux = aux & 0xFB;//0b1111 1011
	outportb(VGA_CRTC_DATA, aux);
}

// Memory address 14 *
static void VGA_CRT_SetMapMemoryAddress14(bool enable) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x17 - Mode control register
	// - Bit 7. Sync enable. Enables vertical and horizontal retrace signals
	// - Bit 6. Word/byte mode selection. 0=word; 1=byte (only available if dword mode is disabled)
	// - Bit 5. Address wrap select. 1= selects MA15 for accessing 256 kb of vram; 0= selects MA13 for 64kb accessing
	// - Bit 3. Divide memory clock by 2.
	// - Bit 2. Divide scanline clock by 2.
	// - Bit 1. Map display address 14.
	// - Bit 0. Map display address 13.
	outportb(VGA_CRTC_INDEX, VGA_CRTC_MODE_CONTROL);
	aux = inportb(VGA_CRTC_DATA);
	if (enable) aux = aux | 0x02;//0bxxxx xx1x
	else
		aux = aux & 0xFD;//0b1111 1101
	outportb(VGA_CRTC_DATA, aux);
}

// Memory address 13 *
static void VGA_CRT_SetMapMemoryAddress13(bool enable) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x17 - Mode control register
	// - Bit 7. Sync enable. Enables vertical and horizontal retrace signals
	// - Bit 6. Word/byte mode selection. 0=word; 1=byte (only available if dword mode is disabled)
	// - Bit 5. Address wrap select. 1= selects MA15 for accessing 256 kb of vram; 0= selects MA13 for 64kb accessing
	// - Bit 3. Divide memory clock by 2.
	// - Bit 2. Divide scanline clock by 2.
	// - Bit 1. Map display address 14.
	// - Bit 0. Map display address 13.
	outportb(VGA_CRTC_INDEX, VGA_CRTC_MODE_CONTROL);
	aux = inportb(VGA_CRTC_DATA);
	if (enable) aux = aux | 0x01;//0bxxxx xxx1
	else
		aux = aux & 0xFE;//0b1111 1110
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_SEQ_AsyncReset(void) {
	// Sequencer port (0x3C4)
	// Register 0x00 - Reset register
	// - Bit 2. Sync reset
	// - Bit 1. Async reset
	outportb(VGA_SEQ_INDEX, VGA_SEQ_RESET);
	outportb(VGA_SEQ_DATA, 0x01);
}

static void VGA_SEQ_ClearReset(void) {
	// Sequencer port (0x3C4)
	// Register 0x00 - Reset register
	// - Bit 2. Sync reset
	// - Bit 1. Async reset
	outportb(VGA_SEQ_INDEX, VGA_SEQ_RESET);
	outportb(VGA_SEQ_DATA, 0x00);
}

static void VGA_SEQ_EnableMemoryMap(bool plane1, bool plane2, bool plane3, bool plane4) {
	byte aux;
	// Sequencer port (0x3C4)
	// Register 0x02 - Map mask register
	// - Bit 3. Enable plane 4
	// - Bit 2. Enable plane 3
	// - Bit 1. Enable plane 2
	// - Bit 0. Enable plane 1
	outportb(VGA_SEQ_INDEX, VGA_SEQ_MAP_MASK);
	aux = inportb(VGA_SEQ_DATA);
	if (plane1) aux = aux | 0x01;
	else
		aux = aux & 0xFE;

	if (plane2) aux = aux | 0x02;
	else
		aux = aux & 0xFD;

	if (plane3) aux = aux | 0x04;
	else
		aux = aux & 0xFB;

	if (plane4) aux = aux | 0x08;
	else
		aux = aux & 0xF7;
	outportb(VGA_SEQ_DATA, aux);
}

// Turn on/off registers write protection *
static void VGA_CRT_WriteProtection(bool enable) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x11 - Vertical retrace end register
	// - Bit 7. Protect. 1=Locks writing on CRT registers 0x01..0x07. 0=Unlocks writing
	// - Bit 6. Bandwidth. Memory refresh bandwith. This enables vram to refresh more often
	// - Bit 3..0. Vertical retrace end
	outportb(VGA_CRTC_INDEX, VGA_CRTC_V_RETRACE_END);
	aux = inportb(VGA_CRTC_DATA);
	if (enable) aux = aux | 0x80;// 0b1000 0000
	else
		aux = aux & 0x7F;//0b0xxx xxxx
	outportb(VGA_CRTC_INDEX, VGA_CRTC_V_RETRACE_END);
	outportb(VGA_CRTC_DATA, aux);
}

// Selects video DRAM refresh bandwith *
// - Active it for compatibility with horizontal refresh rate of 15kHz compatibility
static void VGA_CRT_Bandwith(bool increaseBandwith) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x11 - Vertical retrace end register
	// - Bit 7. Protect. 1=Locks writing on CRT registers 0x01..0x07. 0=Unlocks writing
	// - Bit 6. Bandwidth. Memory refresh bandwith. This enables vram to refresh more often
	// - Bit 3..0. Vertical retrace end
	outportb(VGA_CRTC_INDEX, VGA_CRTC_V_RETRACE_END);
	aux = inportb(VGA_CRTC_DATA);
	if (increaseBandwith) aux = aux | 0x40;// 0b0100 0000
	else
		aux = aux & 0xBF;//0bx0xx xxxx
	outportb(VGA_CRTC_INDEX, VGA_CRTC_V_RETRACE_END);
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_CRT_VerticalTotal(int scanlines) {
	byte aux;
	bool bit8;
	bool bit9;
	// Vertical total (10 bit)
	// - Bits 0..7 are on reg 0x06
	// - Bit 8 is on bit 0 of reg 0x07
	// - Bit 9 is on bit 5 of reg 0x07
	// - Whoever made this deserves the hell :(
	outportb(VGA_CRTC_INDEX, VGA_CRTC_V_TOTAL_REG);
	aux = scanlines & 0x00FF;
	outportb(VGA_CRTC_DATA, aux);

	aux = scanlines >> 8;
	bit8 = aux & 0x01;
	aux = scanlines >> 9;
	bit9 = aux & 0x01;

	outportb(VGA_CRTC_INDEX, VGA_CRTC_OVERFLOW_REG);
	aux = inportb(VGA_CRTC_DATA);
	if (bit8 == true) aux = aux | 0x01;
	else
		aux = aux & 0xFE;
	if (bit9 == true) aux = aux | 0x20;
	else
		aux = aux & 0xEF;
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_CRT_VerticalScanDoubling(bool enable) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x09 - Maximum scanlines register
	// - Bit 7. Scan doubling. Double scanlines by dividing row scanline clock by 2
	// - Bit 6. Line compare (bit 9).
	// - Bit 5. Start vertical blanking (bit 9).
	// - Bit 4..0. Max. scanlines. Causes each scanline to be repeated the value of this field.
	outportb(VGA_CRTC_INDEX, VGA_CRTC_MAX_SCANLINE);
	aux = inportb(VGA_CRTC_DATA);
	if (enable) aux = aux | 0x80;
	else
		aux = aux & 0x7F;
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_CRT_MaxScanlines(int scanlines) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x09 - Maximum scanlines register
	// - Bit 7. Scan doubling. Double scanlines by dividing row scanline clock by 2
	// - Bit 6. Line compare (bit 9).
	// - Bit 5. Start vertical blanking (bit 9).
	// - Bit 4..0. Max. scanlines. Causes each scanline to be repeated the value of this field.
	outportb(VGA_CRTC_INDEX, VGA_CRTC_MAX_SCANLINE);
	aux = inportb(VGA_CRTC_DATA);
	aux = aux & 0xE0;
	aux = aux | (scanlines & 0x1F);
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_CRT_VerticalRetraceStart(int scanline) {
	byte aux;
	bool bit8;
	bool bit9;
	// CRT controller port (0x3D4)
	// Register 0x10 - Vertical retrace start (10 bits)
	// - Bits 0..7 are on reg 0x10
	// - Bit 8 is on bit 2 of reg 0x07
	// - Bit 9 is on bit 7 of reg 0x07
	outportb(VGA_CRTC_INDEX, VGA_CRTC_V_RETRACE_START);
	aux = scanline & 0x00FF;
	outportb(VGA_CRTC_DATA, aux);

	aux = scanline >> 8;
	bit8 = aux & 0x01;
	aux = scanline >> 9;
	bit9 = aux & 0x01;

	outportb(VGA_CRTC_INDEX, VGA_CRTC_OVERFLOW_REG);
	aux = inportb(VGA_CRTC_DATA);
	if (bit8 == true) aux = aux | 0x04;
	else
		aux = aux & 0xFB;
	if (bit9 == true) aux = aux | 0x80;
	else
		aux = aux & 0x7F;
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_CRT_VerticalRetraceEnd(int scanlines) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x11 - Vertical retrace end (4 bits). Number of scanlines of vertical retrace
	// - Bit 7. Protect
	// - Bit 6. Bandwidth
	// - Bit 3..0 Vertical retrace end
	outportb(VGA_CRTC_INDEX, VGA_CRTC_V_RETRACE_END);
	aux = inportb(VGA_CRTC_DATA);
	aux = aux & 0xF0;
	aux = aux | (scanlines & 0x0F);
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_CRT_VerticalDisplayEnd(int scanline) {
	byte aux;
	bool bit8;
	bool bit9;
	// CRT controller port (0x3D4)
	// Register 0x12 - Vertical display end (10 bits)
	// - Bits 0..7 are on reg 0x12
	// - Bit 8 is on bit 1 of reg 0x07
	// - Bit 9 is on bit 6 of reg 0x07
	outportb(VGA_CRTC_INDEX, VGA_CRTC_V_DISPLAY_END);
	aux = scanline & 0x00FF;
	outportb(VGA_CRTC_DATA, aux);

	aux = scanline >> 8;
	bit8 = aux & 0x01;
	aux = scanline >> 9;
	bit9 = aux & 0x01;

	outportb(VGA_CRTC_INDEX, VGA_CRTC_OVERFLOW_REG);
	aux = inportb(VGA_CRTC_DATA);
	if (bit8 == true) aux = aux | 0x02;
	else
		aux = aux & 0xFD;
	if (bit9 == true) aux = aux | 0x40;
	else
		aux = aux & 0xBF;
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_CRT_VerticalBlankingStart(int scanline) {
	byte aux;
	bool bit8;
	bool bit9;
	// Vertical blanking start (10 bit)
	// - Bits 0..7 are on reg 0x15
	// - Bit 8 is on bit 3 of reg 0x07
	// - Bit 9 is on bit 5 of reg 0x09
	outportb(VGA_CRTC_INDEX, VGA_CRTC_V_BLANK_START);
	aux = scanline & 0x00FF;
	outportb(VGA_CRTC_DATA, aux);

	aux = scanline >> 8;
	bit8 = aux & 0x01;
	aux = scanline >> 9;
	bit9 = aux & 0x01;

	outportb(VGA_CRTC_INDEX, VGA_CRTC_OVERFLOW_REG);
	aux = inportb(VGA_CRTC_DATA);
	if (bit8 == true) aux = aux | 0x08;
	else
		aux = aux & 0xF7;
	outportb(VGA_CRTC_DATA, aux);

	outportb(VGA_CRTC_INDEX, VGA_CRTC_MAX_SCANLINE);
	aux = inportb(VGA_CRTC_DATA);
	if (bit9 == true) aux = aux | 0x20;
	else
		aux = aux & 0xDF;
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_CRT_VerticalBlankingEnd(byte scanlines) {
	byte aux;
	// CRT controller port (0x3D4)
	// Register 0x16 - Vertical retrace end (7 bits).
	// - Bits 0..6 are on register 0x16
	outportb(VGA_CRTC_INDEX, VGA_CRTC_V_BLANK_END);
	aux = inportb(VGA_CRTC_DATA);
	aux = aux & 0x80;
	aux = aux | (scanlines & 0x7F);
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_CRT_Offset(byte offset) {
	// CRT controller port (0x3D4)
	// Register 0x13 - Offset (7 bits).
	// - Bits 0..7 are on register 0x13
	outportb(VGA_CRTC_INDEX, VGA_CRTC_OFFSET_REG);
	outportb(VGA_CRTC_DATA, offset);
}

static void VGA_CRT_HorizontalTotal(byte chars) {
	int real_chars;
	// Horizontal total (8 bit)
	// - Bits 0..7 are on reg 0x00
	// - Real value is this reg value + 5
	real_chars = chars - 5;
	outportb(VGA_CRTC_INDEX, VGA_CRTC_H_TOTAL_REG);
	outportb(VGA_CRTC_DATA, real_chars);
}

static void VGA_CRT_HorizontalDisplayEnd(byte chars) {
	// Horizontal display end (8 bit)
	// - Bits 0..7 are on reg 0x01
	outportb(VGA_CRTC_INDEX, VGA_CRTC_H_DISPLAY_END);
	outportb(VGA_CRTC_DATA, chars);
}

static void VGA_CRT_HorizontalBlankingStart(byte chars) {
	// Horizontal blanking start (8 bit)
	// - Bits 0..7 are on reg 0x02
	outportb(VGA_CRTC_INDEX, VGA_CRTC_H_BLANK_START);
	outportb(VGA_CRTC_DATA, chars);
}

static void VGA_CRT_DisplayEnableSkew(byte chars) {
	byte aux;
	// Display enable skew (8 bit)
	// - Bits 6..5 of reg 0x03
	outportb(VGA_CRTC_INDEX, VGA_CRTC_H_BLANK_END);
	aux = inportb(VGA_CRTC_DATA);
	aux = aux & 0x60;// set to false both bits
	aux = aux | ((chars & 0x03) << 5);
	outportb(VGA_CRTC_DATA, aux);
}

static void VGA_CRT_HorizontalBlankingEnd(byte chars) {
	byte aux;
	// Display enable skew (8 bit)
	// - Bits 6..5 of reg 0x03
	// - Possible values 1..64 character clocks
	outportb(VGA_CRTC_INDEX, VGA_CRTC_H_BLANK_END);
	aux = inportb(VGA_CRTC_DATA);
	aux = aux & 0x1F;// set to false both bits
	aux = aux | (chars & 0x1F);
	outportb(VGA_CRTC_DATA, aux);
}

void VGA_SetTextMode(void) {
	__dpmi_regs regs = {0};
	regs.x.ax = 0x03;
	__dpmi_int(0x10, &regs);
}

void VGA_SetModeX(void) {

	asm("cli");

	__dpmi_regs regs = {0};
	regs.x.ax = 0x13;
	__dpmi_int(0x10, &regs);

	VGA_SEQ_Chain4Disable(false);// enable chain 4 planar mode
	VGA_SEQ_ExtMem(true);        // enables external mem
	VGA_SEQ_OddEvenDisable(true);// disables external mem

	VGA_SEQ_EnableMemoryMap(true, true, true, true);// Enable all planes

	VGA_CRT_DoubleWordAddressing(false);// Disables double word addressing
	VGA_CRT_DivideMemClockBy4(false);   // Disables divide mem clock by 4
	VGA_CRT_SetUnderlineLocation(0);    // Set underline location at scaline number 0

	VGA_CRT_SyncEnable(true);             // Enable vertical and horizontal retrace signals
	VGA_CRT_ByteMode(true);               // Enable byte mode
	VGA_CRT_AddressWrapSelect(true);      // Enable MA15 for 256kb addressing
	VGA_CRT_DivideMemClockBy2(false);     // Disable memory address clock division by 2
	VGA_CRT_DivideScanlineClockBy2(false);//Disable scanline clock division by 2
	VGA_CRT_SetMapMemoryAddress14(true);  //Enable memory address 14 (required for 256 kb addressing)
	VGA_CRT_SetMapMemoryAddress13(true);  //Enable memory address 13 (required for 256 kb addressing)

	VGA_CRT_WriteProtection(false);//Enable write protecion on CRT registers
	VGA_CRT_Bandwith(false);


	VGA_SEQ_AsyncReset();

	VGA_MISC_VsyncPolarity(true);// Set positive polarity for VSync
	VGA_MISC_HsyncPolarity(true);// Set positive polarity for HSync
	VGA_MISC_OddEvenPage(true);  // Set low page
	VGA_MISC_ClockSelect(true);  // Set 25MHz for 320/640 wide screen resolution
	VGA_MISC_RAMEnable(true);
	VGA_MISC_IOAddressSelect(true);// For color video card compatibility

	VGA_SEQ_ClearReset();

	VGA_CRT_WriteProtection(false);//Disable write protecion on CRT registers

	/////////// HORIZONTAL CRT CONFIGURATION //////////
	// ** Horizontal mode is already on 320 px width
	VGA_CRT_HorizontalTotal(96);
	VGA_CRT_HorizontalDisplayEnd(88);
	VGA_CRT_HorizontalBlankingStart(80);
	VGA_CRT_HorizontalBlankingEnd(8);
	VGA_CRT_DisplayEnableSkew(2);


	/////////// VERTICAL CRT CONFIGURATION //////////
	VGA_CRT_VerticalTotal(525);
	VGA_CRT_VerticalScanDoubling(false);// Disable vertical scan doubling
	VGA_CRT_MaxScanlines(1);            // Doubles scanlines to make each scanline to be printed twice
	VGA_CRT_VerticalRetraceStart(490);
	VGA_CRT_VerticalRetraceEnd(12);
	VGA_CRT_VerticalDisplayEnd(479);
	VGA_CRT_VerticalBlankingStart(487);
	VGA_CRT_VerticalBlankingEnd(6);


	VGA_CRT_WriteProtection(true);                  //Enable write protecion on CRT registers
	VGA_SEQ_EnableMemoryMap(true, true, true, true);// Enable all planes

	// logical width = screen_width / 4 because mode x active
	vram_LogicalWidth = vga_width >> 2;// 352 / 4 = 88

	VGA_CRT_Offset(44);

	asm("sti");
}

/** Hardware scrolling
 *
 */
void VGA_HardwareScrolling(void) {

	word aux;
	word vram_pointer;

	vram_pointer = vga_scroll_y;

	// Calculate pointer
	vram_pointer *= vram_LogicalWidth;
	vram_pointer += vga_scroll_x >> 2;

	//change scroll registers:
	aux = vram_pointer & 0x00FF;
	outportb(VGA_CRTC_INDEX, 0x0D);
	outportb(VGA_CRTC_DATA, aux);

	aux = vram_pointer & 0xFF00;
	aux = aux >> 8;
	outportb(VGA_CRTC_INDEX, 0x0c);
	outportb(VGA_CRTC_DATA, aux);

	//The smooth panning magic happens here
	//disable interrupts
	asm("cli");

	//Wait Vsync
	VGA_VSync();

	inportb(VGA_INPUT_STATUS);//Read input status, to Reset the VGA flip/flop

	// Update the horizontal pixel panning register
	// - Attribute address register
	// -- bits 0..4 - Attribute address
	// -- bit 5 - PAS (palette address source)
	pix = p[vga_scroll_x & 3];
	outportb(VGA_AC_WRITE, 0x33);
	outportb(VGA_AC_WRITE, pix);

	//enable interrupts
	asm("sti");
}