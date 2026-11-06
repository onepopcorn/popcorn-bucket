#ifndef _ASSETS_SND
#define _ASSETS_SND

typedef struct Note {
    int freq;
    int duration;
} Note;

typedef struct Sound {
    const Note* notes;
    int count;
} Sound;

Note const shot_notes[] = {
    {1000, 3}, {800, 3}, {600, 3}, {400, 3}, {200, 3}};

Sound const shot = {shot_notes, 5};

Note const coin_notes[] = {
    {987, 5}, {1318, 8}};

Sound const coin = {coin_notes, 2};

Note const explosion_notes[] = {
    {200, 5}, {170, 5}, {140, 5}, {110, 5}, {80, 5}, {50, 5}};

Sound const explosion = {explosion_notes, 6};

const Sound fx[] = {coin, shot, explosion};
#endif  // _ASSETS_SND