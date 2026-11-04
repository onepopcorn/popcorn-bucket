#include "speaker.h"
#include <pc.h>
#include <stdint.h>
#include "timer.h"

volatile int beat_counter = 0;
volatile int duration_counter = 0;
volatile int active_sound = -1;

void spk_init() {
    // Configure timer 2 to make sounds
    outportb(PIT_CONF_REG, 0xB6);  // 10110110; Channel 2; access lobyte/high byte; mode 3 square wave; 16-bit binary
}

inline void spk_disable() {
    outportb(SPK_PORT, inportb(SPK_PORT) & ~0x03);
}

inline void spk_enable() {
    outportb(SPK_PORT, inportb(SPK_PORT) | 0x03);
}

void spk_play(int snd) {
    active_sound = snd;
}

void spk_stop() {
}

void spk_tone(int freq) {
    uint16_t divisor = PIT_CLOCK_FREQ / freq;
    outportb(PIT_TIMER2_DATA_PORT, divisor & 0xFF);         // Set lower byte
    outportb(PIT_TIMER2_DATA_PORT, (divisor >> 8) & 0xFF);  // Set higher byte
    spk_enable();
}

void spk_update() {
    if (active_sound < 0) return;

    Sound sound = fx[active_sound];
    if (beat_counter >= sound.count) {
        spk_disable();
        active_sound = -1;
        beat_counter = 0;
        duration_counter = 0;
        return;
    }

    Note note = sound.notes[beat_counter];

    if (duration_counter == 0) spk_tone(note.freq);

    duration_counter++;
    if (duration_counter >= note.duration) {
        beat_counter++;
        duration_counter = 0;
    }
}