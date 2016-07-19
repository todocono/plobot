
#ifndef SOUND_H
#define SOUND_H

#define DEFAULT_VOLUME 0 // 75

void init_sound();
void chip_tone(unsigned long for_millis, unsigned long period_micros, int volume=DEFAULT_VOLUME);

#endif//SOUND_H

