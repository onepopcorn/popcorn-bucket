#ifndef SPEAKER_H
#define SPEAKER_H

#define SPK_PORT 0x61

typedef struct Note {
    int freq;
    int duration;
} Note;

typedef struct Sound {
    const Note* notes;
    int count;
} Sound;

static Note const shot_notes[] = {
    {1000, 3}, {800, 3}, {600, 3}, {400, 3}, {200, 3}};

static Sound const shot = {shot_notes, 5};

static Note const coin_notes[] = {
    {987, 5}, {1318, 8}};

static Sound const coin = {coin_notes, 2};

static Note const explosion_notes[] = {
    {200, 5}, {170, 5}, {140, 5}, {110, 5}, {80, 5}, {50, 5}};

static Sound const explosion = {explosion_notes, 6};

static const Sound fx[] = {coin, shot, explosion};

void spk_init();
void spk_disable();
void spk_enable();
void spk_play(int snd);
void spk_tone(int freq);
void spk_update();

#endif  // SPEAKER_H