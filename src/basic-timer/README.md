# Timers
A simple example to show how to setup timer interrupts to have a stable tick counter

## What you learn
The goal of this example is to teach how to use PIT (Programmable Interval Timer) to setup a stable game tick counter and derive ctimers at different rates from it (10ms ticks, 1 second).

## How it works
PCs of MS-DOS era has Programmable Interval Timer (PIT) with 3 channels we can program in different ways:
- Channel 0: Its input is an oscillator which runs at ~1193182Hz and it's connected to IRQ0 so we can use it to trigger interrupts based on given divisor
- Channel 1: In some older PCs it was used to trigger a refresh of DRAM to compensate capacitator leaks and prevent memory loss or corruption. It's mostly unusable for us and in some systems it may even not exist anymore.
- Channel 2: It's connected to the PC Speaker and can be used to make sound fx and music with it (as seen in other examples on this repo) or be used for other purposes too.

We'll focus on the use of Channel 0 in this example. As said above, the oscillator runs at a rate of 1193182Hz and the channel has a 16bit counter that triggers an interrupt when reaching 0 and it's decreased by 1 at oscillator rate. By default, Channel 0 has a divisor of 0 which means the divisor is 65536. At this frequency and divisor our interrupt handler will be called every ~55ms as per the following formula

```
interrupt freq (Hz) = PIT oscillator / divisor
interrupt rate = 1 / interrupt freq (seconds)

1 / (1193182 / 65536) * 1000 = ~55ms
```

But the default interrupt frequency can be too coarse for our needs and also forces us to only be able to trigger functions in multiples of 55ms which is not a very friendly number. That's why in this example we'll program the  PIT Channel 0 to trigger interrupts every 10ms with the following values:

```
int divisor = 1193182 / 100 = 11931 (decimals are truncated because it's a int)

interrupt freq = 1193182 / 11931 = ~100Hz
interrupt rate = 1 / ~100 * 1000 = ~10ms
```

We must keep in mind that increasing the frequency at which our interrupt is being triggered means that it will be triggered more often and hence taking more CPU time. So we must be careful of what we use these interrupts for and try always to do small operations such as setting flags or updating counters.

## Demo
https://github.com/user-attachments/assets/a0ce9cbc-4b23-4b2d-90b9-29806cbadad3


