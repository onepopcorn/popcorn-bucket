#include "timer.h"
#include <stdint.h>
#include <dpmi.h>  // needed for _go32_dpmi_* definitions
#include <go32.h>  // needed for _go32_my_cs definition
#include <pc.h>    // needed for outportb
#include <dos.h>   // needed for enable and disable definitions
#include <stddef.h>

#define IRQ0_VECTOR 0x08

static _go32_dpmi_seginfo old_handler_0, new_handler_0;

#define MAX_CALLBACKS 10
static VoidFunc callbacks[MAX_CALLBACKS] = {NULL};
static int callback_count = 0;

static void timer_handler() {
    for (int i = 0; i < callback_count; i++) {
        if (callbacks[i] != NULL) {
            callbacks[i]();
        }
    }
}

void timer_init() {
    disable();  // be sure IRQ0 is not triggered mid-reprogramming

    // Save old timer handlers
    _go32_dpmi_get_protected_mode_interrupt_vector(IRQ0_VECTOR, &old_handler_0);

    // Install timer handler
    new_handler_0.pm_offset = (uint32_t)timer_handler;
    new_handler_0.pm_selector = _go32_my_cs();  // get current code segment selector
    _go32_dpmi_chain_protected_mode_interrupt_vector(IRQ0_VECTOR, &new_handler_0);

    // Configure timer
    outportb(PIT_CONF_REG, 0x34);  // 00110100 -> 00 counter select timer 0 | 11 16bit read/write low / high bytes | 010 Mode 2 (rate generator / repeating) | 0 non BCD format
    outportb(PIT_TIMER0_DATA_PORT, (uint16_t)PIT_DIVISOR & 0xFF);
    outportb(PIT_TIMER0_DATA_PORT, ((uint16_t)PIT_DIVISOR >> 8) & 0xFF);

    enable();
}

void timer_free() {
    disable();  // be sure IRQ0 is not triggered mid-reprogramming

    // Restore timer original rate
    outportb(PIT_CONF_REG, 0x36);
    outportb(PIT_TIMER0_DATA_PORT, 0x00);
    outportb(PIT_TIMER0_DATA_PORT, 0x00);

    // Restore old timer handlers
    _go32_dpmi_set_protected_mode_interrupt_vector(IRQ0_VECTOR, &old_handler_0);

    enable();
}

void timer_register_callback(VoidFunc func) {
    // In this example, this fails silently but in a real world
    // program it should throw an error or show a warning
    if (callback_count >= MAX_CALLBACKS) return;
    callbacks[callback_count++] = func;
}