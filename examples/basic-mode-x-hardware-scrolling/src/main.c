#include <bios.h>
#include <dpmi.h>
#include <math.h>
#include <sys/nearptr.h>
#include <stdlib.h>
#include <stdio.h>
#include "vga.h"
#include "types.h"

#define pi 3.14159

int main(void) {

	int i;
	int step = 0;
	word SINEX[256];
	word SINEY[256];


	for (i = 0; i < 256; ++i) {
		SINEX[i] = 320 * ((sin(2.0 * pi * i / 256.0) + 1.0) / 2.0);
		SINEY[i] = 240 * ((sin(2.0 * pi * i / 128.0) + 1.0) / 2.0);
	}


	// Go mode 0x13!
	VGA_SetModeX();

	// Draw some scrap
	__djgpp_nearptr_enable();
	unsigned char *vram = (unsigned char *) (__djgpp_conventional_base + 0xa0000);
	for (int i = 0; i < 200; i++) {
		int x = rand() % 320;
		int y = rand() % 200;
		int color = rand() % 255;
		vram[x + y * 320] = color;
	}
	__djgpp_nearptr_disable();


	// Draw 200 pixels each iteration until a key is pressed
	while (!kbhit()) {

		vga_scroll_x = SINEX[step % 256];
		vga_scroll_y = SINEY[step % 256];
		//rand() % 2;
		step++;

		VGA_HardwareScrolling();
	}


	// Return to text mode
	VGA_SetTextMode();

	return 0;
}