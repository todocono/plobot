
#include "sound.h"

#include <Arduino.h>

const int pin_spk_en = 15;
const int pin_spk_dir = 23;

void init_sound() {
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);
}

void chip_tone(unsigned long for_millis, unsigned long period_micros, int volume) {
  analogWrite(pin_spk_en, volume);
  for(const unsigned long sm=millis();(millis() - sm) < for_millis;) {
    digitalWrite(pin_spk_dir, HIGH);
    digitalWrite(pin_spk_en, HIGH);
    delayMicroseconds(period_micros / 2);
    digitalWrite(pin_spk_en, LOW);
    delayMicroseconds(10);

    digitalWrite(pin_spk_dir, LOW);
    digitalWrite(pin_spk_en, HIGH);
    delayMicroseconds(period_micros / 2);
    digitalWrite(pin_spk_en, LOW);
    delayMicroseconds(10);
  }
}


