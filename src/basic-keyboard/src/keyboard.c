#include "keyboard.h"
#include <dpmi.h>  // for dpmi functions
#include <go32.h>  // for helper to get current code section my_cs
#include <pc.h>    // for read / write ports functions

/**
 * Keyboard controller port returns state in bit 7 and keycode in bits 0-6
 * 00000000
 * ^———————— 1 = key released | 0 = key pressed
 *
 * 00000000
 *  ^^^^^^^———— key code
 */
#define KEY_RELEASED_MASK 0x80  // 0b10000000
#define KEYCODE_MASK 0x7f       // 0b01111111

#define IRQ1 0x09
#define PIC_CMD_PORT 0x20
#define KEYB_PORT 0x60

volatile uint8_t keys[0xFF] = {0};                 // Array to hold all held down keys (initialized to 0, so false)
volatile static uint8_t just_pressed[0xFF] = {0};  // Array to hold all keys that has been pressed (we'll debounce them so they trigger only once)

/**
 * Create variables for our handler and to store old handler so we can restore it when exit our program
 * failing to restore old handler would make keyboard stop working or event crash the system
 *
 */
static _go32_dpmi_seginfo old_handler, new_handler;

static void handler() {
    // Read keyboard state from keyboard port
    uint8_t state = inportb(KEYB_PORT);
    // extract key state
    uint8_t key = state & KEYCODE_MASK;
    // extract keycode
    uint8_t released = state & KEY_RELEASED_MASK;

    if (released) {
        // Key has been released. Clear it from both arrays
        keys[key] = 0;
        just_pressed[key] = 0;
    } else {
        // Key has been pressed
        if (!keys[key]) just_pressed[key] = 1;  // handler is called multiple times while key is being held, this debounces the events so we can track when it was just pressed only
        keys[key] = 1;
    }

    outportb(PIC_CMD_PORT, 0x20);  // send EOI (end of interrupt) command (0x20) to PIC (programmable interrupt controller) to re-enable interrupts
}

uint8_t is_key_just_pressed(uint8_t key) {
    if (just_pressed[key]) {
        just_pressed[key] = 0;  // with this we are sure we only track the pressed once
        return 1;
    }

    return 0;
}

void keyb_init() {
    // Get current keyboard IRQ9 handler and stored in to be restored later
    _go32_dpmi_get_protected_mode_interrupt_vector(IRQ9, &old_handler);

    // Setup new handler
    new_handler.pm_offset = (uint32_t)handler;  // set our handler function pointer
    new_handler.pm_selector = _go32_my_cs();    // pass current code section so interrupt knows where our handler function lives

    _go32_dpmi_allocate_iret_wrapper(&new_handler);                      // create a protected mode -> real mode wrapper to translate calls from one to the oter (returns via real mode iret)
    _go32_dpmi_set_protected_mode_interrupt_vector(IRQ9, &new_handler);  // install our handler
}

void keyb_free() {
    // restore old handler
    if (_go32_dpmi_set_protected_mode_interrupt_vector(IRQ9, &old_handler) == -1) {
        // something went wrong while trying to restore old handler
        /* It's up to you what to do when this happens */
    } else {
        _go32_dpmi_free_iret_wrapper(&new_handler);  // clear iret wrapper resources
    }
}
