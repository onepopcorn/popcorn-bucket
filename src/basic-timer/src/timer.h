#ifndef TIMER_H
#define TIMER_H

#define PIT_CLOCK_FREQ 1193182
#define PIT_TIMER0_DATA_PORT 0x40
#define PIT_TIMER1_DATA_PORT 0x41
#define PIT_TIMER2_DATA_PORT 0x42
#define PIT_CONF_REG 0x43  // PIT control register used to configure which timer, mode, and bit width

#define TIMER_FREQ 100  // 100Hz which triggers our handler every ~10ms
#define PIT_DIVISOR (PIT_CLOCK_FREQ / TIMER_FREQ)

typedef void (*VoidFunc)(void);

void timer_init();
void timer_free();
void timer_register_callback(VoidFunc);

#endif  // TIMER_H