#include "timer.h"
#include <stdint.h>
#include <dpmi.h>  // needed for _go32_dpmi_* definitions
#include <go32.h>  // needed for _go32_my_cs definition
#include <pc.h>    // needed for outportb
#include <dos.h>   // needed for enable and disable definitions
#include <stddef.h>

#define IRS_TIMER_0 0x08

#define TIMER_DIVISOR (PIT_CLOCK_FREQ / 100)  // 100Hz

static _go32_dpmi_seginfo old_handler_0, new_handler_0;

VoidFunc callbacks[10] = {NULL};
int callback_count = 0;

static void timer_handler() {
    for (int i = 0; i < callback_count; i++) {
        if (callbacks[i] != NULL) {
            callbacks[i]();
        }
    }
}

void timer_init() {
    // Save old timer handlers
    _go32_dpmi_get_protected_mode_interrupt_vector(IRS_TIMER_0, &old_handler_0);

    // Install timer handler
    new_handler_0.pm_offset = (uint32_t)timer_handler;
    new_handler_0.pm_selector = _go32_my_cs();  // get current code segment selector
    _go32_dpmi_chain_protected_mode_interrupt_vector(IRS_TIMER_0, &new_handler_0);

    // Configure timer to 100Hz
    outportb(PIT_CONF_REG, 0x34);  // 00110100 -> 00 counter select timer 0 | 11 16bit read/write low / high bytes | 010 Mode 2 (rate generator / repeating) | 0 non BCD format
    outportb(PIT_TIMER0_DATA_PORT, (uint16_t)TIMER_DIVISOR & 0xFF);
    outportb(PIT_TIMER0_DATA_PORT, ((uint16_t)TIMER_DIVISOR >> 8) & 0xFF);
}

void timer_free() {
    disable();
    // Restore timer original rate
    outportb(PIT_CONF_REG, 0x36);
    outportb(PIT_TIMER0_DATA_PORT, 0x00);
    outportb(PIT_TIMER0_DATA_PORT, 0x00);

    // Restore old timer handlers
    _go32_dpmi_set_protected_mode_interrupt_vector(IRS_TIMER_0, &old_handler_0);
    enable();
}

void timer_register_callback(VoidFunc func) {
    callbacks[callback_count++] = func;
}