
#include "sound.h"

#include <Arduino.h>

const int pin_spk_en = 3;
const int pin_spk_dir = 4;

void init_sound() {
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);
  
  /*
  // Off: don't burn speaker
  OCR2A = 0;
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  // 62.5Khz fast PWM

  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));

  // Set fast PWM mode  (p.157)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);

  // Do non-inverting PWM on pin OC2A (p.155)
  // On the Arduino this is pin 11.
  TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);
  TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  */
}

void chip_tone(unsigned long for_millis, unsigned long period_micros, int volume) {
  for(const unsigned long sm=millis();(millis() - sm) < for_millis;) {
    digitalWrite(pin_spk_dir, HIGH);
 //  OCR2A = volume;
    delayMicroseconds(period_micros / 2);

//    OCR2A = 0;
    delayMicroseconds(10);

    digitalWrite(pin_spk_dir, LOW);
//    OCR2A = volume;
    delayMicroseconds(period_micros / 2);

//    OCR2A = 0;
    delayMicroseconds(10);
  }
//  OCR2A = 0;
}





