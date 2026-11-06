#ifndef SPEAKER_H
#define SPEAKER_H

#define SPK_PORT 0x61

void spk_init();
void spk_disable();
void spk_enable();
void spk_play(int snd);
void spk_tone(int freq);
void spk_update();

#endif  // SPEAKER_H